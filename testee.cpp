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
        as[i] = (x >> (8*(i))) & 0xFF;
    }
    return as;
}


int converter_chars_para_int(char* cs) {
    int retorno = 0;
    int num_char = sizeof(int);

    for (int i = 0; i < num_char; i++) {
        retorno |= ((unsigned char)cs[i] << (8*i));
    }
    
    return retorno;
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
    char *bloco = (char*) malloc(tam_bloco);
    memset(bloco, 0, tam_bloco);

    FILE* pFile;
    pFile = fopen(nome_arquivo_saida.c_str(), "wb");
    if (!pFile) {
        printf("Erro ao abrir arquivo para escrita.\n");
        return 1;
    }

    for (int j = 0; j < num_blocos; ++j){
        printf("\tBloco %d/%d\n", j, num_blocos);
        fwrite(bloco, 1, tam_bloco, pFile);
    }

    fclose(pFile);
    //free(bloco);
    printf("\nMemória alocada em \"%s\", de tamanho %d bytes\n", nome_arquivo_saida.c_str(), tam_bloco * num_blocos);

    return 0;
}


// Função para ler um bloco de um arquivo, dado o seu endereço
char* ler_bloco_do_arquivo(double endereco_bloco, string nome_arquivo_saida, int tam_bloco, int num_blocos) {
    ifstream arquivo(nome_arquivo_saida, ios::binary);
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << nome_arquivo_saida << endl;
        return NULL;
    }

    char *bloco = (char*) malloc(tam_bloco);
    
    // Move o cursor para o endereço do bloco
    arquivo.seekg(endereco_bloco);
    arquivo.read(bloco, tam_bloco);
    if (!arquivo) {
        cerr << "Erro ao ler o arquivo: " << nome_arquivo_saida << endl;
        return NULL;
    }

    arquivo.close();

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
            printf("Achou o registro no bloco\n");
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


// TO-DO
int escrever_bloco_no_arquivo(char* bloco, int endereco, string nome_arquivo_saida, int tam_bloco, int num_blocos) {
    FILE* arquivo = fopen(nome_arquivo_saida.c_str(), "wb");
    if (!arquivo) {
        printf("Erro ao abrir arquivo para escrita.\n");
        return 0;
    }

    // Posicionar no endereço do bloco no arquivo
    if (fseek(arquivo, endereco, SEEK_SET) != 0) {
        printf("Erro ao se posicionar no arquivo para escrita.\n");
        fclose(arquivo);
        return 1;
    }

    // Escrever bloco no arquivo
    int num_escritos = fwrite(bloco, tam_bloco, 1, arquivo);
    //printf("%d vs %ld / %d\n", num_escritos, sizeof(bloco), tam_bloco);
    if (num_escritos != 1) {
        printf("Erro ao escrever no arquivo.\n");
        fclose(arquivo);
        return 1;
    }

    return 0;
}


// Função que escreve um registro em um dado bloco de dados
int escrever_registro_no_bloco_dados(Registro* r, char* bloco, int tam_bloco) {
    int pos = 0;
    int id_atual = -1;
    int reg = 0;
    char id_str[sizeof(int)];
    //char* bloco = (char*) malloc(sizeof(char) * tam_bloco);
    
    // Achar um registro vazio no bloco
    for (reg = 0; reg < tam_bloco/sizeof(Registro); reg++) {
        memcpy(id_str, bloco + pos, sizeof(int));
                
        //id_atual = stoi(id_str);

        if (id_str[0] == 0) {
            break;
        }

        pos += sizeof(Registro);
    }

    // Escreve se achou
    if (id_str[0] == 0) {
        char* reg = registro_to_char(r);
        printf("Achou um bloco livre, vai escrever o registro \n");
        for (int i = 0; i < sizeof(Registro); i++) {
            bloco[pos + i] = reg[i];
        }
        /*
        for (int i = 0; i < sizeof(Registro); i++) {
            printf("%d ", bloco[pos + i]);
        }
        */
        return 1;
    }

    // Não escreve se estiver cheio
    return 0;
}


void teste_leitura_dados(Registro* registro, int endereco, string nome_arquivo_saida, int num_blocos_hash, int tam_bloco) {
    char* bloco = (char*) malloc(sizeof(char) * tam_bloco);
    // READ_ITEM()
    bloco = ler_bloco_do_arquivo(endereco, nome_arquivo_saida, tam_bloco, num_blocos_hash);
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

    fscanf(arquivo, "%[^\r\n]%*[\r\n]", linha);
    string line = linha;
    
    return line;
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


// TO-DO
int funcao_hash(Registro* registro, int num_blocos_hash, int tam_bloco) {
    return 0;
} 


// TO-DO
void salva_no_arq_dados(Registro* registro, int endereco, string nome_arquivo_saida, int num_blocos_hash, int tam_bloco) {
    
    
    char* bloco = (char*) malloc(sizeof(char) * tam_bloco);

    // WRITE_ITEM()
    bloco = ler_bloco_do_arquivo(endereco, nome_arquivo_saida, tam_bloco, num_blocos_hash);
    escrever_registro_no_bloco_dados(registro, bloco, tam_bloco);
    escrever_bloco_no_arquivo(bloco, endereco, nome_arquivo_saida, tam_bloco, num_blocos_hash);
    printf("Escrito\n");

    
    
    return;
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
    printf("il: %d\n", int_length);
    
    int num_campos = 0;
    int endereco = 0;
    Registro r, s;
    
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

    printf("tamanho do bloco: %d\n", tam_bloco);
    printf("tamanho do registro (hash): %d\n", tam_reg_hash);
    printf("número de linhas: %d\n", num_linhas);
    printf("numero de blocos (hash): %d\n\n", num_blocos_hash);

    printf("Alocando %d bytes de memória para o arquivo...\n", num_blocos_hash * tam_bloco);
    aloca_memoria_arquivo(nome_arquivo_saida, num_blocos_hash, tam_bloco);

    // TO-DO: escrever um registro
    
    

    // TO-DO: ler um registro
    //*/

    printf("Parsing do arquivo de entrada:\n");
    linha = le_linha(arquivo);
    while (linha.size() > 0) {
        cont++;
        s = r;
        // Lê um registro do arquivo
        r = parse(linha, arquivo);
        printf("id: %d\t", r.ID);

        /*

        // Gera o endereço do registro
        endereco = funcao_hash(&r, num_blocos_hash, tam_bloco);

        // Salva o registro no arquivo de dados
        salva_no_arq_dados(&r, endereco, nome_arquivo_saida, num_blocos_hash, tam_bloco);

        // Salva o endereço no arquivo de índice primário
        salva_no_arq_idx_prim(&r, endereco);

        // Salva o endereço no arquivo de índice secundário
        salva_no_arq_idx_sec(&r, endereco);
        */

        linha = le_linha(arquivo);
    }
    printf("\nNúmero de artigos: %d\n\n", cont);

    // Teste:
    ///*
    endereco = funcao_hash(&r, num_blocos_hash, tam_bloco);

    // Salva o registro no arquivo de dados
    // escrevendo penúltimo registro
    salva_no_arq_dados(&s, endereco, nome_arquivo_saida, num_blocos_hash, tam_bloco);
    // escrevendo último registro
    salva_no_arq_dados(&r, endereco, nome_arquivo_saida, num_blocos_hash, tam_bloco);

    char* bloco_teste = ler_bloco_do_arquivo(tam_bloco * 0, nome_arquivo_saida, tam_bloco, num_blocos_hash);

    printf("\nBloco de endereço 0 * %d lido (tamanho: %d bytes):\n", tam_bloco, tam_bloco);
    for (int i = 0; i < tam_bloco; i++) {
        printf("%c", bloco_teste[i]);
    }
    printf("\n\n");
    //*/

    fclose(arquivo);
}

int main() {
    string nome_arquivo_entrada = "artino.csv";
    string nome_arquivo_saida = "saida.bin";

    le_arquivo_csv(nome_arquivo_entrada, nome_arquivo_saida);

    return 0;
}