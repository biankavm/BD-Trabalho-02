#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring> // Para as funções de manipulação de strings C
#include <vector>
#include <limits>
#include <unordered_map>
//#include "C:/Program Files/Boost/boost_1_84_0"

//#include <boost/filesystem.hpp>
using namespace std;
 

// declaração do registro
typedef struct Registro {
    int ID;
    char titulo[300];
    int ano;
    char autores[150];
    int citacoes;
    char atualizacao[20];
    char snippet[1024];
} Registro;

// ======================================================================================================
// Funções de gerenciamento de structs de dados

// função que cria um registro (construtor)
Registro criaRegistro(int id, const char titulo[], int ano, const char autores[], int citacoes,
                      const char atualizacao[], const char snippet[]){
    Registro R;
    R.ID = id;
    strncpy(R.titulo, titulo, sizeof(R.titulo));
    R.titulo[sizeof(R.titulo) - 1] = '\0';
    R.ano = ano;
    strncpy(R.autores, autores, sizeof(R.autores));
    R.autores[sizeof(R.autores) - 1] = '\0';
    R.citacoes = citacoes;
    strncpy(R.atualizacao, atualizacao, sizeof(R.atualizacao));
    R.atualizacao[sizeof(R.atualizacao) - 1] = '\0';
    strncpy(R.snippet, snippet, sizeof(R.snippet));
    R.snippet[sizeof(R.snippet) - 1] = '\0';
    return R;
}

char* converter_int_para_chars(int x) {
    int size_int = sizeof(int);
    int int_bits = (size_int*8);

    // Convert the integer to a sequence of chars
    char* as = (char*) malloc(size_int);
    
    // Use bitwise operations to extract the bytes of the integer
    for (int i = 0; i < size_int; i++) {
        as[i] = (x >> (8*i)) & 0xFF; //(8*(size_int-i-1))
    }
    
    return as;
}


int converter_chars_para_int(char* cs) {
    int retorno = 0;
    int num_char = sizeof(int);

    for (int i = 0; i < num_char; i++) {
        retorno |= ((unsigned char)cs[i] << (8*i)); //(num_char - i - 1)
    }
    
    return retorno;
}


void escreve_int_str(char* int_str, char* dest_str, int pos) {
    for (int i = 0; i < sizeof(int); i++) {
        dest_str[pos+i] = int_str[i];
    }
}


// Função que transforma os dados do registro em uma string contínua de dados (para o bloco)
char* registro_to_char(Registro* r) {
    Registro a;
    char* reg_char = (char*) malloc(sizeof(Registro));
    int campo = 0;

    // id
    char* id = converter_int_para_chars(r->ID);
    for (int i = 0; i < sizeof(int); i++) {
        reg_char[i] = id[i];
    }
    free(id);
    campo += sizeof(int);

    // título
    for (int i = 0; i < sizeof(r->titulo); i++) {
        reg_char[i+campo] = r->titulo[i];
    }
    campo += sizeof(r->titulo);

    // ano
    char* ano = converter_int_para_chars(r->ano);
    for (int i = 0; i < sizeof(int); i++) {
        reg_char[i+campo] = ano[i];
    }
    free(ano);
    campo += sizeof(int);

    // autores
    for (int i = 0; i < sizeof(r->autores); i++) {
        reg_char[i+campo] = r->autores[i];
    }
    campo += sizeof(r->autores);

    // citação
    char* citacao = converter_int_para_chars(r->citacoes);
    for (int i = 0; i < sizeof(int); i++) {
        reg_char[i+campo] = citacao[i];
    }
    free(citacao);
    campo += sizeof(int);

    // atualização
    for (int i = 0; i < sizeof(r->atualizacao); i++) {
        reg_char[i+campo] = r->atualizacao[i];
    }
    campo += sizeof(r->atualizacao);

    // snippet
    for (int i = 0; i < sizeof(r->snippet); i++) {
        reg_char[i+campo] = r->snippet[i];
    }
    /*
    for (int i = 0; i < sizeof(Registro); i++) {
        printf("%d ", reg_char[i]);
    }
    */

    return reg_char;
}


// ======================================================================================================
// Funções de acesso à memória secundária

// Função para alocar a quantidade de bytes total para um arquivo
// (necessária porque é possível que um bloco vazio seja lido, e não queremos acessar memória que não é nossa)
int aloca_memoria_arquivo(string nome_arquivo_saida, int num_blocos, int tam_bloco) {
    // Preenche o arquivo com 0s (NULL)
    int batch_size = 16;
    int bytes = tam_bloco * batch_size;
    int iteracoes = num_blocos/batch_size;
    char *bloco = (char*) malloc(bytes);
    memset(bloco, 0, bytes);

    FILE* pFile;
    pFile = fopen(nome_arquivo_saida.c_str(), "wb");
    if (!pFile) {
        printf("Erro ao abrir arquivo para escrita.\n");
        return 1;
    }

    for (int j = 0; j < iteracoes; j++){
        printf("\tIteração %d/%d\n", j, iteracoes);
        fwrite(bloco, 1, bytes, pFile);
    }

    fclose(pFile);
    free(bloco);
    printf("\nMemória alocada em \"%s\", de tamanho %d bytes\n", nome_arquivo_saida.c_str(), tam_bloco * num_blocos);

    return 0;
}


// Função para ler um bloco de um arquivo, dado o seu endereço
char* ler_bloco_do_arquivo(long endereco_bloco, string nome_arquivo_entrada, int tam_bloco) {
    FILE *arquivo = fopen(nome_arquivo_entrada.c_str(), "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", nome_arquivo_entrada.c_str());
    }
    char *bloco = (char*) malloc(tam_bloco);
    
    // Posicionar no endereço do bloco no arquivo
    if (fseek(arquivo, endereco_bloco, SEEK_SET) != 0) {
        printf("Erro ao se posicionar no arquivo para leitura. %ld\n", endereco_bloco);
        fclose(arquivo);
        return NULL;
    }

    int num_lidos = fread(bloco, tam_bloco, 1, arquivo);
    if (num_lidos != 1) {
        printf("Erro ao ler o arquivo.\n");
        fclose(arquivo);
        return NULL;
    }
    
    fclose(arquivo);
    return bloco;
}


// Função para ler um registro de um bloco de arquivo de dados, dado um id 
Registro ler_registro_do_bloco_dados(char* bloco, int id, int tam_bloco, int tam_reg) {
    Registro r;
    r.ID = -1;
    vector<int> sizes = {sizeof(r.ID), sizeof(r.titulo), sizeof(r.ano), sizeof(r.autores), sizeof(r.citacoes), sizeof(r.atualizacao), sizeof(r.snippet)};
    char id_str[sizeof(int)];
    int pos = 0;
    int id_atual = -1;

    for (int reg = 0; reg < tam_bloco/tam_reg; reg++) {
        memcpy(id_str, bloco + pos, sizeof(int));
        //printf("idstr = %s\n", id_str);
        //id_atual = stoi(id_str
        id_atual = converter_chars_para_int(id_str);

        if (id_atual == id) {
            //printf("Achou o registro no bloco\n");
            r.ID = id_atual;

            memcpy(r.titulo, bloco + sizes[0], sizes[1]);

            memcpy(id_str, bloco + sizes[1], sizes[2]);
            r.ano = converter_chars_para_int(id_str);
            
            memcpy(r.autores, bloco + sizes[2], sizes[3]);

            memcpy(id_str, bloco + sizes[3], sizes[4]);
            r.citacoes = converter_chars_para_int(id_str);

            memcpy(r.atualizacao, bloco + sizes[4], sizes[5]);
            memcpy(r.snippet, bloco + sizes[5], sizes[6]);

            
            return r;
        }
        pos += tam_reg;
    }

    return r;
}


// Escreve um bloco em um dado endereço em um arquivo
int escrever_bloco_no_arquivo(char* bloco, long endereco, string nome_arquivo_saida, int tam_bloco) {
    FILE* arquivo = fopen(nome_arquivo_saida.c_str(), "rb+");
    if (!arquivo) {
        printf("Erro ao abrir arquivo para escrita.\n");
        return 1;
    }

    // Posicionar no endereço do bloco no arquivo
    if (fseek(arquivo, endereco, SEEK_SET) != 0) {
        printf("Erro ao se posicionar no arquivo para escrita.\n");
        fclose(arquivo);
        return 1;
    }

    // Escrever bloco no arquivo
    int num_escritos = fwrite(bloco, tam_bloco, 1, arquivo);

    if (num_escritos != 1) {
        printf("Erro ao escrever no arquivo.\n");
        fclose(arquivo);
        return 1;
    }

    fclose(arquivo);

    return 0;
}


// Função que escreve um registro em um dado bloco de dados
int escrever_registro_no_bloco_dados(Registro* r, char* bloco, int tam_bloco) {
    int pos = 0;
    int id_atual = -1;
    int reg = 0;
    char id_str[sizeof(int)];
    char achou_espaco = 0;
    
    // Achar um registro vazio no bloco
    for (reg = 0; reg < tam_bloco/sizeof(Registro); reg++) {
        memcpy(id_str, bloco + pos, sizeof(int));

        // Se estiver vazio, ou se for equivalente a um registro que já existe
        if ((id_str[0] == 0) || (converter_chars_para_int(id_str) == r->ID)) {
            achou_espaco = 1;
            break;
        }
        pos += sizeof(Registro);
    }

    // Escreve se achou
    if (achou_espaco) {
        char* reg = registro_to_char(r);
        //printf("Achou o espaço, vai escrever o registro \n");
        for (int i = 0; i < sizeof(Registro); i++) {
            bloco[pos + i] = reg[i];
        }
        free(reg);
        return 1;
    }

    // Não escreve se estiver cheio
    printf("Não conseguiu espaço.\n");
    return 0;
}


void teste_leitura_dados(Registro* registro, long endereco, string nome_arquivo_saida, int num_blocos_hash, int tam_bloco) {
    // READ_ITEM()
    char* bloco = ler_bloco_do_arquivo(endereco, nome_arquivo_saida, tam_bloco);
    Registro lido = ler_registro_do_bloco_dados(bloco, registro->ID, tam_bloco, sizeof(Registro));
    printf("%d\n", lido.ID);

    free(bloco);
}


// Função para contar as linhas do arquivo para definirmos o tamanho do arquivo de dados
int conta_linhas_arquivo(string nome_arquivo){
    ifstream arquivo(nome_arquivo);
    if (!arquivo.is_open()){
        cerr << "Erro ao abrir o arquivo: " << nome_arquivo << endl;
        return -1;
    }
    int cont = 0;
    string linha;
    while(getline(arquivo, linha)){

        cont++;
    }
    arquivo.close();
    return cont;
}


// Função que faz uma chamada de sistema para descobrir o tamanho do bloco de dados no dispositivo
int acha_tamanho_dos_blocos(){
    FILE* pipe = popen("sudo blockdev --getbsz /dev/sda", "r");    
    if (!pipe){
        cerr << "Erro ao executar o comando." << endl;
        return -1;
    } 

    char buffer[128];
    string result = "";

    // ler a saída do comando linha por linha
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }

    // Fecha o pipe
    pclose(pipe);

    // Converte a string resultante para um número inteiro e retorna
    return stoi(result);
}


// Função que calcula o tamanho do registro do arquivo de dados em bytes
int tam_registro_hash() {
    return sizeof(Registro);
}


// Função que calcula a quantidade de blocos necessários para um dado arquivo com um dado tamanho de registro
int calcula_num_blocos(int tam_bloco, int tam_reg, int num_artigos) {
    int num_regs_bloco = tam_bloco / tam_reg;
    int num_blocos_art = (num_artigos / num_regs_bloco) + 1;
    
    return num_blocos_art;
}


// ======================================================================================================
// Funções de parsing do arquivo de entrada

// Função para ler uma linha do arquivo de entrada
// Criada para ajudar a tornar o parser mais universal para versões de Linux diferentes
string le_linha(FILE *arquivo) {
    // Cria uma string do tamanho máximo de um registro 
    char* linha = (char*) malloc(sizeof(Registro) + (7*3));

    int size = fscanf(arquivo, "%[^\r\n]%*[\r\n]", linha);
    if (size > -1) {
        string line(linha);
        free(linha);
        return line;
    }
    else {
        string line("");
        free(linha);
        return line;
    }
    
}


// Função que faz a separação de uma linha do arquivo em campos de um Registro
Registro parse(string linha, FILE* arquivo) {
    int count = 0;
    int linha_size = linha.size();
    int comeco_ultimo = 0;

    vector<char> campo_atual;
    vector<string> campos_completos;

    Registro r;
    r.ID = -1;

    // Campo inválido
    if (linha_size < 6) {
        return r;
    } 
    for (int i = 1; i < linha_size - 2; i++) {
        // Checa os casos bases de divisão de campos por ' "; '
        if ((linha[i] == '"') && (linha[i+1] == ';')) {

            // Trata campos vazios
            if (linha[i+2] == ';') {

                string novo_campo(campo_atual.begin(), campo_atual.end());
                campos_completos.push_back(novo_campo);
                campos_completos.push_back(" ");
                campo_atual.clear();

                i++;
                count++;
            }

            // Caso comum
            else {

                string novo_campo(campo_atual.begin(), campo_atual.end());
                campos_completos.push_back(novo_campo);
                campo_atual.clear();
            }
            
            count++;
            i +=2;
            comeco_ultimo = i;
        }

        // Trata o caso do último campo da  linha
        else if (i == linha_size - 3) {
            if (linha[comeco_ultimo] != 'N') {
                comeco_ultimo ++;
            }

            string prim_pedaco = linha.substr(comeco_ultimo, linha_size - comeco_ultimo);
            vector<char> pedaco;

            // Caso o artigo esteja quebrado em múltiplas linhas
            int nao_eh_fim_do_snippet = !((linha[i] == '.') && (linha[i+1] == '.') && (linha[i+2] == '"'));
            int nao_eh_snippet_null = !((linha[i] == 'U') && (linha[i+1] == 'L') && (linha[i+2] == 'L'));
            int nao_eh_snippet_estranho = (linha[i+2] != '"');

            if (nao_eh_fim_do_snippet && nao_eh_snippet_null && nao_eh_snippet_estranho) {
                string nova_linha;

                // Lê a próxima linha e a atribui como a nova linha do parsing
                //getline(arquivo, nova_linha);
                nova_linha = le_linha(arquivo);
                linha_size = nova_linha.size();
                linha = nova_linha;
                i = 0;
                int j = 0;

                // Adiciona o restante do campo quebrado, e volta para o loop principal com a nova linha como entrada
                for (j = 0; j < linha_size - 2; j++) {
                    if ((linha[j] == '"') && (linha[j+1] == ';')) {
                        string novo_pedaco(pedaco.begin(), pedaco.end());
                        string novo_campo = prim_pedaco + novo_pedaco;
                        campos_completos.push_back(novo_campo);
                        count++;
                        break;
                    }
                    else {
                        pedaco.push_back(linha[j]);
                    }
                }
                i = j +2;
            }

            // Caso comum do último campo
            else {
                campos_completos.push_back(prim_pedaco);
                count++;
            }
            campo_atual.clear();
        }

        else {
            campo_atual.push_back(linha[i]);
        }
    }
    
    // Todo artigo deve ter 7 campos
    if (campos_completos.size() != 7) {
        if (campos_completos.size() > 0) {
            cout << "Erro no artigo: ID " << campos_completos[0] << endl;
        }
        else {
            cout << "Erro no artigo: vazio. " << endl;
        }
        return r;
    }

    // Cria e retorna um Registro com as informações destiladas
    r = criaRegistro(stoi(campos_completos[0].c_str()), 
                    campos_completos[1].c_str(), 
                    stoi(campos_completos[2]), 
                    campos_completos[3].c_str(), 
                    stoi(campos_completos[4]), 
                    campos_completos[5].c_str(), 
                    campos_completos[6].c_str());
    return r;

}


// Hash mais básica, um vetor
long funcao_hash_temp(Registro* registro, int num_blocos_hash, int tam_bloco) {
    return ((long) registro->ID) * tam_bloco;
} 


// TO-DO - hash perfeito
long funcao_hash_perfeito(Registro* registro, int num_blocos_hash, int tam_bloco) {
    return 1;
} 

// Aplica uma função hashing no id do registro
long funcao_hash(Registro* registro, int num_blocos_hash, int tam_bloco) {
    return funcao_hash_temp(registro, num_blocos_hash, tam_bloco);
} 


// Vai reunir as informações necessárias para o cálculo dos endereços de hash da findrec
void escreve_metadados_hash_arquivo(int tam_bloco, int num_blocos, int num_linhas, int tam_reg, string nome_arquivo_saida) {
    int endereco_metadados = 0;
    char* bloco = ler_bloco_do_arquivo(endereco_metadados, nome_arquivo_saida, tam_bloco);
    int size_int = sizeof(int);

    // Escreve cada int em uma posição consecutiva do bloco
    char* str = converter_int_para_chars(tam_bloco);
    escreve_int_str(str, bloco, 0);

    str = converter_int_para_chars(num_blocos);
    escreve_int_str(str, bloco, 1*size_int);

    str = converter_int_para_chars(num_linhas);
    escreve_int_str(str, bloco, 2*size_int);

    str = converter_int_para_chars(tam_reg);
    escreve_int_str(str, bloco, 3*size_int);

    escrever_bloco_no_arquivo(bloco, endereco_metadados, nome_arquivo_saida, tam_bloco);

    free(bloco);
    free(str);

    return;
}


int cria_arq_dados(int num_blocos, int tam_bloco, string nome_arquivo_saida){
    aloca_memoria_arquivo(nome_arquivo_saida, num_blocos, tam_bloco);
    return 0;
}


// Recebe um registro para salvar em um endereço do arquivo
void salva_no_arq_dados(Registro* registro, long endereco, string nome_arquivo_saida, int num_blocos_hash, int tam_bloco) {
    // WRITE_ITEM()
    char* bloco = ler_bloco_do_arquivo(endereco, nome_arquivo_saida, tam_bloco);
    if (!escrever_registro_no_bloco_dados(registro, bloco, tam_bloco)) {
        printf("Não escreveu registro no bloco.\n");
        return;
    }
    if (escrever_bloco_no_arquivo(bloco, endereco, nome_arquivo_saida, tam_bloco)) {
        printf("Não escreveu bloco no arquivo.\n");
        return;
    }
    //printf("Escrito\n");
    
    free(bloco);
    return;
}

int aplica_hashing(Registro* registro, int tam_bloco, int num_blocos, string nome_arquivo_saida) {
    long endereco = funcao_hash(registro, num_blocos, tam_bloco);
    
    //printf("\nEndereço hash para id (%d): %d\n", registro->ID, endereco);

    // Salva o registro no arquivo de dados
    salva_no_arq_dados(registro, endereco, nome_arquivo_saida, num_blocos, tam_bloco);
    return 0;
}

// TO-DO
void salva_no_arq_idx_prim(Registro* registro, int endereco) {
    return;
}


// TO-DO
void salva_no_arq_idx_sec(Registro* registro, int endereco) {
    return;
}


// leitura do arquivo csv
void le_arquivo_csv(string nome_arquivo_entrada, string nome_arquivo_saida){
    
    int cont = 0;
    FILE *arquivo = fopen(nome_arquivo_entrada.c_str(), "r");
    if (!arquivo) {
        cout << "Erro ao abrir arquivo!!!" << endl;
        return;
    }

    string linha;
    int int_length = 2.5 * sizeof(int);
    
    int num_campos = 0;
    long endereco = 0;
    Registro r, r0, r1;

    int num_blocos_hash_temp = 0;
    
    ///*
    printf("Descobrindo tamanho do bloco de dados...\n");
    int tam_bloco = acha_tamanho_dos_blocos();

    printf("Descobrindo tamanhos dos registros...\n");
    int tam_reg_hash = tam_registro_hash();
    // TO-DO: descobrir registros dos índices

    printf("Descobrindo o número de linhas do arquivo...\n");
    int num_linhas = conta_linhas_arquivo(nome_arquivo_entrada);

    printf("Descobrindo o número de blocos para armazenar o arquivo de dados...\n");
    int num_blocos_hash = calcula_num_blocos(tam_bloco, tam_reg_hash, num_linhas);
    // TO-DO - hashing perfeito
    num_blocos_hash_temp = num_linhas * 1.55;

    printf("\ttamanho do bloco: %d\n", tam_bloco);
    printf("\ttamanho do registro (hash): %d\n", tam_reg_hash);
    printf("\tnúmero de linhas: %d\n", num_linhas);
    printf("\tnumero de blocos (hash): %d, %d bytes\n", num_blocos_hash, num_blocos_hash * tam_bloco);
    
    printf("Inicializando metadados...\n");
    int metadados[4];
    metadados[0] = tam_bloco;
    metadados[1] = num_blocos_hash_temp;
    metadados[2] = num_linhas;
    metadados[3] = tam_reg_hash;

    printf("\nAlocando %ld bytes de memória para o arquivo...\n", (long) metadados[1] * metadados[3]);
    cria_arq_dados(metadados[1], metadados[0], nome_arquivo_saida);

    printf("\nEscrevendo metadados do hashing...\n");    
    escreve_metadados_hash_arquivo(metadados[0], metadados[1], metadados[2], metadados[3], nome_arquivo_saida);
    
    printf("Metadados escritos:\n");
    char* bloco_meta = ler_bloco_do_arquivo(0, nome_arquivo_saida, metadados[0]);
    
    char int_str[4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int_str[j] = bloco_meta[(i*sizeof(int)) + j];
        }
        printf("%d ", converter_chars_para_int(int_str));
    }
    printf("\n");
    free(bloco_meta);

    
    //*/

    printf("\nParsing do arquivo de entrada:\n");
    linha = le_linha(arquivo);
    while (linha.size() > 0) {
        cont++;

        // Lê um registro do arquivo
        r = parse(linha, arquivo);
        
        // TO-DO - if temporário, por causa do artinho.csv
        if (r.ID <= metadados[1]) {
            aplica_hashing(&r, metadados[0], metadados[1], nome_arquivo_saida);
        }
        printf("%do id salvo: %d\n", cont, r.ID);
        
        /*
        // Salva o endereço no arquivo de índice primário
        salva_no_arq_idx_prim(&r, endereco);

        // Salva o endereço no arquivo de índice secundário
        salva_no_arq_idx_sec(&r, endereco);
        */
       
        linha = le_linha(arquivo);
    }
    printf("\nNúmero de artigos: %d\n", cont);

    // Teste:
    ///*
    Registro obaa;
    obaa.ID = 14;

    endereco = funcao_hash(&obaa, num_blocos_hash, tam_bloco);
    printf("\nBuscando id: %d. Endereço hash: %ld\n", obaa.ID, endereco);

    char* bloco_teste = ler_bloco_do_arquivo(endereco, nome_arquivo_saida, tam_bloco);

    printf("\nBloco de endereço %ld lido (tamanho: %d bytes):\n", endereco, tam_bloco);
    for (int i = 0; i < tam_bloco; i++) {
        printf("%c", bloco_teste[i]);
    }
    printf("\n\n");
    free(bloco_teste);
    //*/

    fclose(arquivo);
}

int main() {
    string nome_arquivo_entrada = "artino.csv";
    string nome_arquivo_saida = "saida.bin";

    le_arquivo_csv(nome_arquivo_entrada, nome_arquivo_saida);

    return 0;
}