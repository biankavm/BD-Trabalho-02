#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <iostream>
#include <vector>
#include <algorithm>

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring> 
#include <limits>
#include <unordered_map>

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


// Dados que vão para o darquivo de índice
typedef struct tipoNoDisc
{
    int *chaves;
    long *enderecos;
}NoDisco;

// Dados que ficam em memória principal
typedef struct tipoNoRam
{
    int esFolha;
    long meuEndereco;
    int numChaves;
    NoDisco *dados;
    struct tipoNoRam **filhos;
}NoRAM;

// Árvore B+
typedef struct tipoArvoreBP
{
    NoRAM *raiz;
    int maximo;
}ArvoreBP;

int numBlocos = 1;




// ======================================================================
// Funções de criação de structs

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

// Retorna o ponteiro para uma struct que representa a arvore B+
ArvoreBP *criaArvore(int maximo) {

    // Allocate memory of new tree
    ArvoreBP *arvore = (ArvoreBP *) malloc(sizeof(ArvoreBP));

    if (arvore != NULL)
    {
        // Set degree and root value
        arvore->maximo = maximo;
        arvore->raiz = NULL;
    }
    else
    {
        printf("\n Memory Overflow when create new Tree");
    }
    return arvore;
}

// Retorna um novo nó de memória principal vazio
NoRAM *criaNoRAM(int maximo) {
    // Create new tree node
    printf("tentando criar...\n");
    NoRAM *no = (NoRAM *) malloc(sizeof(NoRAM));
    printf("cria no, ");
    NoDisco *dados = (NoDisco*)malloc(sizeof(NoDisco));
    printf("cria dados, ");
    
    if (no != NULL && dados !=NULL) {
        // Create memory of node key
        no->dados = dados;
        no->dados->chaves = (int *) malloc((sizeof(int)) *(maximo+1));
        no->dados->enderecos = (long*) malloc((sizeof(long))*(maximo+1));

        printf("cria vetores, ");
        // Allocate memory of node child
        no->filhos = (NoRAM **) malloc((maximo + 1) *sizeof(NoRAM *));

        printf("cria ponteiros\n");
        // Set initial child
        for (int i = 0; i <= maximo ; ++i) {
            no->filhos[i] = NULL;
        }
        no->esFolha = 0;
        no->numChaves = 0;
    }
    return no;
}




// ======================================================================
// Funções util 

//Suporte Tecnico:
int converter_chars_para_int(char* cs) {
    int retorno = 0;
    int num_char = sizeof(int);

    for (int i = 0; i < num_char; i++) {
        retorno |= ((unsigned char)cs[i] << (8*i));
    }
    
    return retorno;
}

int converter_chars_para_long(char* cs) {
    int retorno = 0;
    int num_char = sizeof(long);

    for (int i = 0; i < num_char; i++) {
        retorno |= ((unsigned char)cs[i] << (8*i));
    }
    
    return retorno;
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

char* converter_long_para_chars(long x) {
    int size_long = sizeof(long);
    int long_bits = (size_long*8);

    // Convert the integer to a sequence of chars
    char* as = (char*) malloc(size_long);
    
    // Use bitwise operations to extract the bytes of the integer
    for (int i = 0; i < size_long; i++) {
        as[i] = (x >> (8*i)) & 0xFF; //(8*(size_int-i-1))
    }
    
    return as;
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

    return reg_char;
}


void ler_campo(char* dest_str, int posicao, int tam, char* bloco) {
    for (int i = 0; i < tam; i++) {
        dest_str[i] = bloco[i+posicao];
    }
    return;
}




// ======================================================================================================
// Funções de acesso à memória secundária

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
        printf("Erro ao ler o arquivo \"%s\". Endereço: %ld\n", nome_arquivo_entrada.c_str(), endereco_bloco);
        fclose(arquivo);
        return NULL;
    }
    
    fclose(arquivo);
    return bloco;
}

// Função para ler um registro de um bloco de arquivo de dados, dado um id 
void ler_registro_do_bloco_dados(Registro* r, char* bloco, int id, int tam_bloco, int tam_reg) {
    //Registro r;
    r->ID = -1;
    vector<int> sizes = {sizeof(r->ID), sizeof(r->titulo), sizeof(r->ano), sizeof(r->autores), sizeof(r->citacoes), sizeof(r->atualizacao), sizeof(r->snippet)};
    char id_str[sizeof(int)];
    int pos = 0;
    int id_atual = -1;
    int pos_char = 0;

    for (int reg = 0; reg < tam_bloco/tam_reg; reg++) {
        memcpy(id_str, bloco + pos, sizeof(int));
        id_atual = converter_chars_para_int(id_str);

        if (id_atual == id) {
            r->ID = id_atual;

            pos_char = sizes[0];
            ler_campo(r->titulo, pos + pos_char, sizes[1], bloco);

            pos_char += sizes[1];
            ler_campo(id_str, pos + pos_char, sizes[2], bloco);
            r->ano = converter_chars_para_int(id_str);
            
            pos_char += sizes[2];
            ler_campo(r->autores, pos + pos_char, sizes[3], bloco);

            pos_char += sizes[3];
            ler_campo(id_str, pos + pos_char, sizes[4], bloco);
            r->citacoes = converter_chars_para_int(id_str);

            pos_char += sizes[4];
            ler_campo(r->atualizacao, pos + pos_char, sizes[5], bloco);

            pos_char += sizes[5];
            ler_campo(r->snippet, pos + pos_char, sizes[6], bloco);
            return;
        }
        pos += tam_reg;
    }

    return;
}

// Lê um bloco carregado em char binário.
int ler_registro_do_bloco_index(NoDisco* no, char* bloco, int tam_bloco) {
    // Calculando tamanhos dos campos dos registros
    int sizeInt = sizeof(int);
    int sizeLong =  sizeof(long);
    int pos = (tam_bloco)/2;
    int id_atual = -1;
    int num_chaves = tam_bloco/(sizeof(int)+sizeof(long));
    char id_str[sizeof(long)];
    
    // Lê os chars binários do vetor de chaves
    for (int reg = 0; reg < num_chaves; reg++) {
        // Pega os chars de um int
        for (int i = 0; i < sizeInt; i++) {
            id_str[i] = bloco[(reg * sizeInt)+i];
        }
        id_atual = converter_chars_para_int(id_str);
        no->chaves[reg] = id_atual;
    }

    // Lê os chars binários do vetor de endereços
    for (int reg = 0; reg < num_chaves; reg++) {
        // Pega os chars de um int
        for (int i = 0; i < sizeLong; i++) {
            id_str[i] = bloco[pos + (reg * sizeLong)+i];
        }
        id_atual = converter_chars_para_long(id_str);
        no->enderecos[reg] = id_atual;
    }
    
    return 0;
}

//vai utilizar as informações de um nó da ram para carregar dados do disco
NoRAM *ler_NoDisco_do_disco(NoRAM *noVazio, string arqSaida, int tamBloco, ArvoreBP *abp){
    // ler bloco com aquele endereço(ele está em char)
    char* bloco = NULL;
    printf("ller_NoDisco: endereço %ld\n", noVazio->meuEndereco);
    bloco = ler_bloco_do_arquivo(noVazio->meuEndereco, arqSaida, tamBloco);
    NoDisco *dis;
    if(noVazio->dados == NULL){
        //printf("no vazio\n");
        dis = (NoDisco*) malloc(sizeof(NoDisco));
        noVazio->dados = dis;
        noVazio->dados->chaves = (int *) malloc((sizeof(int)) *(abp->maximo));
        noVazio->dados->enderecos = (long*) malloc((sizeof(long))*(abp->maximo));

    }
    else{
        printf("No não vazio\n");
        dis = noVazio->dados;
        noVazio->dados->chaves = (int *) malloc((sizeof(int)) *(abp->maximo));
        noVazio->dados->enderecos = (long*) malloc((sizeof(long))*(abp->maximo));
    }
    ler_registro_do_bloco_index(noVazio->dados, bloco,tamBloco);
    free(bloco);
    
    
    return noVazio;
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

// Ou seja, converte o que ta em em um bloco, em um cloco de char, para usar no arquivo binario
int escrever_registro_no_bloco_index(NoDisco* no, char* bloco, int tam_bloco) {
    //recebe um ponteiro para um no do disco, e um bloco, e o tamanho do bloco
    // Calculando tamanhos dos campos dos registros
    
    int num_chaves = tam_bloco / (sizeof(int) + sizeof(long));
    char *id_str = NULL;//(char*)malloc(sizeof(long));
    int sizeInt = sizeof(int);
    int sizeLong = sizeof(long);
    
    for (int reg = 0; reg < num_chaves; reg++) {
        // Converte o int em um vetor de chars de valor ascii
        id_str = converter_int_para_chars(no->chaves[reg]);
        
        // Adiciona o vetor no bloco
        for (int i = 0; i < sizeInt; i++) {
            bloco[(reg * sizeInt)+i] = id_str[i];
        }
    }
    free(id_str);
    for (int reg = 0; reg < num_chaves; reg++) {
        // Converte o int em um vetor de chars de valor ascii
        id_str = converter_long_para_chars(no->enderecos[reg]);
        //printf("%ld\n", converter_chars_para_long(id_str));

        // Adiciona o vetor no bloco
        for (int i = 0; i < sizeLong; i++) {
            //printf("%d ", id_str[i]);
            bloco[((reg + (num_chaves*sizeInt)) * sizeLong)+i] = id_str[i];
        }
        //printf("\n");
    }
    free(id_str);
    // printf("deu free no bloco escrever_registro_no_bloco\n");
    //id_str = converter_long_para_chars(12288);
    //printf("12288 =  %ld\n", converter_chars_para_long(id_str));
    return 0;
}


int escrever_NoDisco_no_disco(NoRAM *noR, char novo, int tamBloco, string nomeArqSaida){
// Ele deve receber um ponteiro para um no, não vazio
// retorna 1 se foi bem sucedido, 0 se não
    char* bloco = NULL;


    if(novo == 1 ){
        //printf("vai criar novo bloco no arquivo de dados\n");
        // vai criar um bloco de dados no arquivo
        numBlocos++;
        bloco = (char*)malloc(tamBloco);
        if(bloco ==NULL){
            printf("\t-bloco null --erro\n");
            return 0;
        }
        memset(bloco,0,tamBloco);
        //printf("\t-criou novo bloco\n");
    }
    else{
        //printf("vai atualizar um bloco existente\n");
        // vai atualizar os dados que ja existem no arquivo
        bloco = ler_bloco_do_arquivo(noR->meuEndereco, nomeArqSaida, tamBloco);

    }

    //printf("escrever antes NoDisco %d: %ld %ld\n", 2, noR->dados->enderecos[2]);
    escrever_registro_no_bloco_index(noR->dados,bloco,tamBloco);
    //printf("escrever NoDisco %d: %ld %ld\n", 2, noR->dados->enderecos[2]);

    //printf("escreveu registro no bloco\n");
    escrever_bloco_no_arquivo(bloco,noR->meuEndereco,nomeArqSaida,tamBloco);
    //printf("escreveu bloco no arquivo\n");

    free(bloco);
    //printf("deu free no bloco escrever_NoDisco\n");

    return 1;
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


// Vai reunir as informações necessárias para o cálculo dos endereços de árvore do seek1 
void escreve_metadados_idx_arquivo(int tam_bloco, int num_blocos, long raiz, int tam_reg, string nome_arquivo_saida) {
    int endereco_metadados = 0;
    char* bloco = ler_bloco_do_arquivo(endereco_metadados, nome_arquivo_saida, tam_bloco);
    int size_int = sizeof(int);

    // Escreve cada int em uma posição consecutiva do bloco
    char* str = converter_int_para_chars(tam_bloco);
    escreve_int_str(str, bloco, 0);

    str = converter_int_para_chars(num_blocos);
    escreve_int_str(str, bloco, 1*size_int);

    str = converter_long_para_chars(raiz);
    escreve_int_str(str, bloco, 2*sizeof(long));

    str = converter_int_para_chars(tam_reg);
    escreve_int_str(str, bloco, 3*size_int);

    escrever_bloco_no_arquivo(bloco, endereco_metadados, nome_arquivo_saida, tam_bloco);

    free(bloco);
    free(str);

    return;
}

int liberar_NoDisco_da_RAM(NoRAM *no){
    //retorna 1 se foi bem sucedido, 0 se não
    if (no) {
        //printf("%d %d %d\n", no->dados->chaves, no->dados->enderecos, no->dados);
        free(no->dados->chaves);
        //printf("liberou chaves\n");
        if (no->dados->enderecos) {
            free(no->dados->enderecos);
        }
        //printf("liberou enderecos\n");
        free(no->dados);
        //printf("liberou dados\n");
        no->dados = NULL;
    }
    //printf("dados: %ld\n", no->dados);
    return 1;
}

// Função para alocar a quantidade de bytes total para um arquivo
// (necessária porque é possível que um bloco vazio seja lido, e não queremos acessar memória que não é nossa)
int aloca_memoria_arquivo(string nome_arquivo_saida, int num_blocos, int tam_bloco) {
    // Preenche o arquivo com 0s (NULL)
    int batch_size = 16;
    int bytes = tam_bloco * batch_size;
    int iteracoes = 1 + num_blocos/batch_size;
    char *bloco = (char*) malloc(bytes);
    memset(bloco, 0, bytes);

    FILE* pFile;
    pFile = fopen(nome_arquivo_saida.c_str(), "wb");
    if (!pFile) {
        printf("Erro ao abrir arquivo para escrita.\n");
        return 1;
    }

    for (int j = 0; j < iteracoes; j++){
        //printf("\tIteração %d/%d\n", j, iteracoes);
        fwrite(bloco, 1, bytes, pFile);
    }

    fclose(pFile);
    free(bloco);
    printf("\nMemória alocada em \"%s\", de tamanho %d bytes\n", nome_arquivo_saida.c_str(), tam_bloco * num_blocos);

    return 0;
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




// ======================================================================================================
// Funções de hashing

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

int cria_arq_dados(int num_blocos, int tam_bloco, string nome_arquivo_saida){
    aloca_memoria_arquivo(nome_arquivo_saida, num_blocos, tam_bloco);
    return 0;
}

// Recebe um registro para salvar em um endereço do arquivo
void salva_no_arq_dados(Registro* registro, long endereco, string nome_arquivo_saida, int num_blocos_hash, int tam_bloco) {
    // WRITE_ITEM()
    // printf("salva arq_dados: endereço %ld\n", endereco);
    char* bloco = ler_bloco_do_arquivo(endereco, nome_arquivo_saida, tam_bloco);
    if (!escrever_registro_no_bloco_dados(registro, bloco, tam_bloco)) {
        printf("Não escreveu registro no bloco.\n");
        free(bloco);
        return;
    }
    if (escrever_bloco_no_arquivo(bloco, endereco, nome_arquivo_saida, tam_bloco)) {
        printf("Não escreveu bloco no arquivo.\n");
        free(bloco);
        return;
    }
    //printf("Escrito\n");
    free(bloco);
    
    return;
}

long aplica_hashing(Registro* registro, int tam_bloco, int num_blocos, string nome_arquivo_saida) {
    long endereco = funcao_hash(registro, num_blocos, tam_bloco);
    //printf("\nEndereço hash para id (%d): %d\n", registro->ID, endereco);
    
    // Salva o registro no arquivo de dados
    salva_no_arq_dados(registro, endereco, nome_arquivo_saida, num_blocos, tam_bloco);
    return endereco;
}



// ======================================================================================================
// Funções de árvore B+

long inserir_NoRAM_interno_arvore(ArvoreBP* abp, int chave, long endereco, NoRAM* atual, NoRAM* filho, int tambloco, string arquivo);

// Faz a busca pela chave, caminhando pelo galho da árvore, 
// e retorna o endereço do nó no disco.
long* busca_endereco_NoRAM(ArvoreBP* abp, int chave, string testArq, int tamBloco) {
    NoRAM *galho = abp->raiz;
    int i = 0;
    int j = 0;
    NoRAM *pai = NULL;

    // Executes the loop until when cursor node is not leaf node
    while (galho->esFolha == 0)
    {
        // Get the current node
        pai = galho;

        for (i = 0; i < galho->numChaves; i++)
        {
            if(!galho->dados)
            {
                ler_NoDisco_do_disco(galho,testArq,tamBloco,abp);
            }
            if (chave < galho->dados->chaves[i])
            {
                liberar_NoDisco_da_RAM(galho);
                galho = galho->filhos[i];
                break;
            }
            if (i == galho->numChaves - 1)
            {
                liberar_NoDisco_da_RAM(galho);
                galho = galho->filhos[i + 1];
                break;
            }
        }
    }

    long* enderecos = (long*) malloc(sizeof(long) * 2);
    enderecos[0] = galho->meuEndereco;
    enderecos[1] = pai ->meuEndereco;
    return enderecos;
}

// Seleciona apenas o endereço do pai ou do filho do conjunto de 
// endereços retornados pela função de busca
long busca_endereco_pai_filho(ArvoreBP* abp, int chave_filho, int retorna_pai, string testArq, int tamBloco) {
    long endereco = 0;
    long* enderecos = (long*) malloc(sizeof(long) * 2);
    enderecos = busca_endereco_NoRAM(abp, chave_filho,testArq,tamBloco);

    if (retorna_pai) {
        endereco = enderecos[1];
    }
    else {
        endereco = enderecos[0];
    }

    free(enderecos);

    return endereco;
}

// Retorna um NoRAM contendo as informações do pai do nó buscado. 
// Usa busca por chave, e usa a ler_NoDisco_do_disco()
NoRAM* achar_NoRAM_pai(ArvoreBP* abp, int chave_filho, string arsaida, int tamBloco) {
    if (!abp->raiz) {
        return NULL;
    }

    // Recebe um vetor de endereços
    long endereco = 0;
    endereco = busca_endereco_pai_filho(abp, chave_filho, 1, arsaida, tamBloco);

    // Se o endereço existir
    if (!endereco) {
        return NULL;
    }

    // Inicializa o nó para poder procurá-lo no disco
    NoRAM* pai = criaNoRAM(abp->maximo);
    pai->meuEndereco = endereco;

    ler_NoDisco_do_disco(pai,arsaida,tamBloco,abp );
    if (!pai->meuEndereco) {
        return NULL;
    }
    
    return pai;
}

// Para inserções em nós que têm espaço sobrando, só são feitas atualizações nos nós já existentes
NoRAM* tenta_inserir_No_vago(ArvoreBP* abp, int chave, long endereco, NoRAM* atual, NoRAM* filho, int tambloco, string arquivo) {
    int i = 0;
    int j = 0;

    if (atual->numChaves < abp->maximo) {
        printf("Inserindo chave %d em um nó VAGO de endereço %ld\n", chave, atual->meuEndereco);
        // Acha a posição certa para a chave
        while (chave > atual->dados->chaves[i] && i < atual->numChaves) {
            i++;
        }
     
        
        for (j = atual->numChaves; j > i; j--) {
            atual->dados->chaves[j] = atual->dados->chaves[j - 1];
        }
        

        // Desloca chaves maiores para a direita
        // Desloca ponteiros e endereços das chaves para a direita
        // ??? possível segmentation fault
        for (j = atual->numChaves + 1; j > i + 1; j--) {
            //atual->dados->chaves[j] = atual->dados->chaves[j - 1];
            atual->dados->enderecos[j] = atual->dados->enderecos[j - 1];
            atual->filhos[j] = atual->filhos[j - 1];
        }

        // Adiciona nova chave nos vetores
        atual->dados->chaves[i] = chave;
        atual->numChaves++;
        atual->filhos[i + 1] = filho;
        atual->dados->enderecos[i + 1] = filho->meuEndereco;

        // Operações de atualização do nó atual e escrita do novo nó filho
        //printf("Atualizando do nó atual (endereço: %ld)...\n", atual->meuEndereco);
        escrever_NoDisco_no_disco(atual, 0, tambloco, arquivo);
        //printf("Escrevendo nó filho (endereço: %ld)...\n", filho->meuEndereco);
        escrever_NoDisco_no_disco(filho, 1, tambloco, arquivo);

        // Liberando dados extras desnecessários
        //printf("Liberando NoDisco do nó atual (endereço: %ld)...\n", atual->meuEndereco);
        liberar_NoDisco_da_RAM(atual);
        //printf("Liberando NoDisco do nó filho (endereço: %ld)...\n", filho->meuEndereco);
        liberar_NoDisco_da_RAM(filho);

        return filho;
    }
    else {
        return NULL;
    }
}

// Para inserções em nós cheios, faz o split
void insere_em_split(ArvoreBP* abp, int chave, long endereco, NoRAM* atual, NoRAM* filho, int tambloco, string arquivo) {
    int i = 0;
    int j = 0;

    NoRAM* novo_interno = criaNoRAM(abp->maximo);

    // A cópia das chaves tem o mesmo tamanho dos vetores de endereços 
    // e ponteiros porque sua última posição é o booleano de ser ou não folha
    int* copia_chaves = (int*) malloc(sizeof(int) * (abp->maximo + 2));
    long* copia_enderecos = (long*) malloc(sizeof(long) * (abp->maximo + 2));
    NoRAM** copia_filhos = (NoRAM**) malloc(sizeof(NoRAM*) * (abp->maximo + 2));
    
    ler_NoDisco_do_disco(atual, arquivo, tambloco, abp);

    // Copia todas as chaves, endereços e filhos do nó atual para um vetor temporário
    // ??? possível segmentation fault
    for (i = 0; i < abp->maximo + 1; i++) {
        copia_chaves[i] = atual->dados->chaves[i];
        copia_enderecos[i] = atual->dados->enderecos[i];
        copia_filhos[i] = atual->filhos[i];
    }
    printf("Copia chaves, enderecos e filhos da atual (endereco %ld)\n", atual->meuEndereco);
    /*
    for (i = 0; i < abp->maximo + 1; i++)
    {
        copia_filhos[i] = atual->child[i];
    }
    */

    i = 0;

    while (chave > copia_chaves[i] && i < abp->maximo) {
        i++;
    }
    
    
    for (j = abp->maximo + 1; j > i; j--) {
        copia_chaves[j] = copia_chaves[j - 1];
    }

    // ??? possível segmentation fault
    for ( j = abp->maximo + 2; j > i + 1; j--) {
        //copia_chaves[j] = copia_chaves[j - 1];
        copia_enderecos[j] = copia_enderecos[j - 1];
        copia_filhos[j] = copia_filhos[j - 1];
    }

    copia_chaves[i] = chave;
    copia_enderecos[i] = endereco;
    printf("Nova chave e endereços (%d, %ld) inseridos na posição %d\n", chave, endereco, i);

    copia_filhos[i + 1] = filho;
    
    novo_interno->esFolha = 0;
    novo_interno->dados->chaves[abp->maximo] = 0;
    // atual->dados->chaves[abp->maximo] = 

    atual->numChaves = (abp->maximo + 1) / 2;
    novo_interno->numChaves = abp->maximo - (abp->maximo + 1) / 2;

    // ???
    for (i = 0, j = atual->numChaves; i < novo_interno->numChaves; i++, j++) {
        novo_interno->dados->chaves[i] = copia_chaves[j];
    }
    for (i = 0, j = atual->numChaves + 1; i < novo_interno->numChaves + 1; i++, j++) {
        novo_interno->dados->enderecos[i] = copia_enderecos[j];
        novo_interno->filhos[i] = copia_filhos[j];
    }

    if (atual == abp->raiz) {
        abp->raiz = criaNoRAM(abp->maximo);

        abp->raiz->dados->chaves[0] = atual->dados->chaves[atual->numChaves];
        abp->raiz->filhos[0] = atual;
        abp->raiz->dados->enderecos[0] = atual->meuEndereco;
        abp->raiz->filhos[1] = novo_interno;
        abp->raiz->dados->enderecos[1] = novo_interno->meuEndereco;

        abp->raiz->esFolha = 0;
        abp->raiz->dados->chaves[abp->maximo] = 0;
        abp->raiz->numChaves = 1;

        // Escrevendo novo nó e liberando memória extra
        //printf("Escrevendo novo nó raiz (endereço: %ld)...\n", abp->raiz->meuEndereco);
        escrever_NoDisco_no_disco(abp->raiz, 1, tambloco, arquivo);
        //printf("Liberando NoDisco do nó raiz (endereço: %ld)...\n", abp->raiz->meuEndereco);
        liberar_NoDisco_da_RAM(abp->raiz);
    }
    else {
        inserir_NoRAM_interno_arvore(abp, atual->dados->chaves[atual->numChaves],  atual->dados->enderecos[atual->numChaves], achar_NoRAM_pai(abp, atual->meuEndereco, arquivo, tambloco), novo_interno, tambloco, arquivo);
    }

    // Operações de atualização do nó atual e escrita do novo nó filho
    //printf("Atualizando do nó atual (endereço: %ld)...\n", atual->meuEndereco);
    escrever_NoDisco_no_disco(atual, 0, tambloco, arquivo);
    //printf("Escrevendo novo nó interno (endereço: %ld)...\n", novo_interno->meuEndereco);
    escrever_NoDisco_no_disco(novo_interno, 1, tambloco, arquivo);
    //printf("Escrevendo nó filho (endereço: %ld)...\n", filho->meuEndereco);
    escrever_NoDisco_no_disco(filho, 1, tambloco, arquivo);

    // Liberando dados extras desnecessários
    //printf("Liberando NoDisco do nó atual (endereço: %ld)...\n", atual->meuEndereco);
    liberar_NoDisco_da_RAM(atual);
    //printf("Liberando NoDisco do novo nó interno (endereço: %ld)...\n", novo_interno->meuEndereco);
    liberar_NoDisco_da_RAM(novo_interno);
    //printf("Liberando NoDisco do nó filho (endereço: %ld)...\n", filho->meuEndereco);
    liberar_NoDisco_da_RAM(filho);

    free(copia_chaves);
    free(copia_enderecos);
    free(copia_filhos);
}

// Insere um nó interno na árvore
// Vai receber a chave e o endereço a serem inseridos, além dos 
// mesmos nós.  Vai retornar o endereço atrelado à chave inserida 
long inserir_NoRAM_interno_arvore(ArvoreBP* abp, int chave, long endereco, NoRAM* atual, NoRAM* filho, int tambloco, string arquivo) {
    NoRAM* novo_no = NULL; 

    // Primeiro caso de inserção: em um nó vago, se não funcionar, vai para o próximo caso
    if (!(novo_no = tenta_inserir_No_vago(abp, chave, endereco, atual, filho, tambloco, arquivo))) {
        printf("Vai dar split para o nó filho (%ld) e nó atual (%ld)\n", filho->meuEndereco, atual->meuEndereco);
        insere_em_split(abp, chave, endereco, atual, filho, tambloco, arquivo);
    }
    return filho->meuEndereco;
}

// Insere uma nova chave na árvore
// Vai retornar o endereço do nó atrelado à chave inserida em disco.
long inserir_nova_chave(ArvoreBP* abp, int chave, long endereco, int tambloco, string arquivo) {
    if (abp->raiz == NULL) {
        // Add first node of tree
        abp->raiz = criaNoRAM(abp->maximo);
        abp->raiz->numChaves   = 1;
        abp->raiz->esFolha  = 1;
        abp->raiz->dados->chaves[abp->maximo]  = 1;
        abp->raiz->dados->chaves[0] = chave;
        abp->raiz->dados->enderecos[0] = endereco;
        //abp->raiz->meuEndereco = endereco;

        //printf("Primeiro valor: Escrevendo nó raiz (endereço: %ld)...\n", abp->raiz->meuEndereco);
        abp->raiz->meuEndereco = escrever_NoDisco_no_disco(abp->raiz, 1, tambloco, arquivo);
        //printf("Liberando NoDisco do nó raiz (endereço: %ld)...\n", abp->raiz->meuEndereco);
        //liberar_NoDisco_da_RAM(abp->raiz);
        //printf("retornando..\n");
        return abp->raiz->meuEndereco;
    }
    else {
        // Loop controlling variables
        int i = 0;
        int j = 0;

        NoRAM *atual = abp->raiz;
        NoRAM *pai = NULL;
        if (!atual->dados) {
            ler_NoDisco_do_disco(atual, arquivo, tambloco, abp);
        }
        // Caminha na árvore até chegar à folha onde a chave deve ser inserida
        while (atual->esFolha == 0) {
            pai = atual;
            for (i = 0; i < atual->numChaves; i++) {
                if (!atual->dados) {
                    ler_NoDisco_do_disco(atual, arquivo, tambloco, abp);
                }
                if (chave < atual->dados->chaves[i]) {
                    liberar_NoDisco_da_RAM(atual);
                    atual = atual->filhos[i];
                    break;
                }
                if (i == atual->numChaves - 1) {
                    liberar_NoDisco_da_RAM(atual);
                    atual = atual->filhos[i + 1];
                    break;
                }
            }
        }

        // Caso o nó esteja vago
        if (atual->numChaves < abp->maximo) {
            //printf("nó vago pra folha\n");
            //atual->meuEndereco = endereco;
            i = 0;
            if (!atual->dados) {
                printf("recuperando dados do discos...\n");
                ler_NoDisco_do_disco(atual, arquivo, tambloco, abp);
            }
            
            // Procura o espaço ideal para a nova chave
            while (chave > atual->dados->chaves[i] && i < atual->numChaves) {
                i++;
            }
            //printf("espaço achado\n");
            // Desloca as chaves para a direita
            for (j = atual->numChaves; j > i; j--) {
                atual->dados->chaves[j] = atual->dados->chaves[j - 1];
            }

            for (j = atual->numChaves + 1; j > i + 1; j--) {
                atual->dados->enderecos[j] = atual->dados->enderecos[j - 1];
            }
            //printf("deslocou\n");

            // Insere chave e endereço
            atual->dados->chaves[i] = chave;
            atual->dados->enderecos[i] = endereco;
            //printf("inserido %ld %ld\n", endereco, atual->dados->enderecos[i]);
            atual->numChaves++;

            //printf("adiciona os valores novos\n");


            // Aponta para a próxima folha
            atual->filhos[atual->numChaves] = atual->filhos[atual->numChaves - 1];
            atual->dados->enderecos[atual->numChaves] = atual->dados->enderecos[atual->numChaves - 1];
            atual->filhos[atual->numChaves - 1] = NULL;
            //atual->dados->enderecos[atual->numChaves - 1] = 0;
            //printf("apontado %ld %ld\n", endereco, atual->dados->enderecos[i]);
            
            //printf("trocando ponteiros\n");

            // Atualiza nó folha
            //printf("Atualizando do nó atual (endereço: %ld)...\n", atual->meuEndereco);
            //printf("antes endereço %d: %ld %ld\n", i, atual->dados->enderecos[i], endereco);
            atual->meuEndereco = escrever_NoDisco_no_disco(atual, 0, tambloco, arquivo);
            //printf("Liberando NoDisco do nó atual (endereço: %ld)...\n", atual->meuEndereco);
            //printf("endereço %d: %ld %ld\n", i, atual->dados->enderecos[i], endereco);
            liberar_NoDisco_da_RAM(atual);
            //printf("retornando\n");
            return atual->meuEndereco;
        }

        // Caso o nó esteja cheio, faz o split
        else {
            printf("vai dar split para a folha\n");

            NoRAM *nova_folha = criaNoRAM(abp->maximo);
            printf("criou nó\n");
            int* copia_chaves = (int*) malloc(sizeof(int) * (abp->maximo + 2));
            printf("criou chaves\n");
            long* copia_enderecos = (long*) malloc(sizeof(long) * (abp->maximo + 2));
            printf("criou enderecos\n");
 
            for (i = 0; i < abp->maximo; i++) {
                copia_chaves[i] = atual->dados->chaves[i];
            }

            for (i = 0; i < abp->maximo + 1; i++) {
                copia_enderecos[i] = atual->dados->enderecos[i];
            }

            // Procura espaço ideal para separar os nós
            i = 0;
            while (chave > copia_chaves[i] && i < abp->maximo) {
                i++;
            }

            // Desloca chaves e endereços para a direita
            for (j = abp->maximo + 1; j > i; j--) {
                copia_chaves[j] = copia_chaves[j - 1];
            }
            for ( j = abp->maximo + 2; j > i + 1; j--) {
                copia_enderecos[j] = copia_enderecos[j - 1];
            }

            copia_chaves[i] = chave;
            copia_enderecos[i] = endereco;

            nova_folha->esFolha = 1;
            nova_folha->dados->chaves[abp->maximo] = 1;

            atual->numChaves = (abp->maximo + 1) / 2;
            nova_folha->numChaves = abp->maximo + 1 - (abp->maximo + 1) / 2;

            // Aponta para a próxima folha
            atual->filhos[atual->numChaves] = nova_folha;
            atual->dados->enderecos[atual->numChaves] = nova_folha->meuEndereco;

            nova_folha->filhos[nova_folha->numChaves] = atual->filhos[abp->maximo];
            nova_folha->dados->enderecos[nova_folha->numChaves] = atual->dados->enderecos[abp->maximo];

            atual->filhos[abp->maximo] = NULL;
            atual->dados->enderecos[abp->maximo] = 0;


            for (i = 0; i < atual->numChaves; i++) {
                atual->dados->chaves[i] = copia_chaves[i];
            }
            for (i = 0; i < atual->numChaves+1; i++) {
                atual->dados->enderecos[i] = copia_enderecos[i];
            }

            for (i = 0, j = atual->numChaves; i < nova_folha->numChaves; i++, j++) {
                nova_folha->dados->chaves[i] = copia_chaves[j];
            }
            for (i = 0, j = atual->numChaves+1; i < nova_folha->numChaves; i++, j++) {
                nova_folha->dados->enderecos[i] = copia_enderecos[j];
            }

            /*
            if (chave > i) {
                nova_folha->meuEndereco = endereco;
            }
            else {

            }*/

            // Cria nova raiz
            if (atual == abp->raiz) {
                printf("split na raiz\n");
                NoRAM *nova_raiz = criaNoRAM(abp->maximo);
                printf("criou nova raiz\n");
                nova_raiz->dados->chaves[0] = nova_folha->dados->chaves[0];
                nova_raiz->filhos[0] = atual;
                nova_raiz->dados->enderecos[0] = atual->meuEndereco;

                nova_raiz->filhos[1] = nova_folha;
                nova_raiz->dados->enderecos[1] = nova_folha->meuEndereco;

                nova_raiz->esFolha = 0;
                nova_raiz->dados->chaves[abp->maximo] = 0;
                nova_raiz->numChaves = 1;
                abp->raiz = nova_raiz;

                // Escrevendo novo nó e liberando memória extra
                printf("fim\n");
                abp->raiz->meuEndereco = escrever_NoDisco_no_disco(abp->raiz, 1, tambloco, arquivo);
                printf("(inserir_nova_chave()) Escrevendo novo nó raiz (endereço: %ld)...\n", abp->raiz->meuEndereco);
                liberar_NoDisco_da_RAM(abp->raiz);
                printf("Liberando NoDisco do nó raiz (endereço: %ld)...\n", abp->raiz->meuEndereco);
            }
            else
            {
                inserir_NoRAM_interno_arvore(abp, nova_folha->dados->chaves[0],nova_folha->dados->enderecos[0], pai, nova_folha, tambloco, arquivo);
            }

            // Operações de atualização do nó atual e escrita do novo nó filho
            //printf("Atualizando do nó atual (endereço: %ld)...\n", atual->meuEndereco);
            atual->meuEndereco = escrever_NoDisco_no_disco(atual, 0, tambloco, arquivo);
            //printf("Escrevendo nó folha (endereço: %ld)...\n", nova_folha->meuEndereco);
            nova_folha->meuEndereco = escrever_NoDisco_no_disco(nova_folha, 1, tambloco, arquivo);

            // Liberando dados extras desnecessários
            //printf("Liberando NoDisco do nó atual (endereço: %ld)...\n", atual->meuEndereco);
            liberar_NoDisco_da_RAM(atual);
            //printf("Liberando NoDisco do nó folha (endereço: %ld)...\n", nova_folha->meuEndereco);
            liberar_NoDisco_da_RAM(nova_folha);

            free(copia_chaves);
            free(copia_enderecos);

            return nova_folha->meuEndereco;
        }
    }
}





// ======================================================================================================
// Funções principais

// Função que lê um arquivo csv e gera os arquivos de dados, índice primário e secundário
void le_arquivo_csv(string nome_arquivo_entrada, string nome_arquivo_dados, string nome_arquivo_idx_prim, string nome_arquivo_idx_sec){
    
    int cont = 0;
    FILE *arquivo = fopen(nome_arquivo_entrada.c_str(), "r");
    if (!arquivo) {
        cout << "Erro ao abrir arquivo!!!" << endl;
        return;
    }

    string linha;
    int int_length = 2.5 * sizeof(int);
    
    int num_campos = 0;
    long endereco_hash = 0;
    long endereco_idx = 0;
    Registro r, r0, r1;

    int num_blocos_hash_temp = 0;
    
    ///*
    printf("Descobrindo tamanho do bloco de dados...\n");
    int tam_bloco = acha_tamanho_dos_blocos();
    //int tam_bloco = 8192;

    printf("Descobrindo tamanhos dos registros...\n");
    int tam_reg_hash = tam_registro_hash();

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

    printf("\nAlocando %ld bytes de memória para o arquivo (Isso deve demorar alguns minutos)...\n", (long) metadados[1] * metadados[3]);
    cria_arq_dados(metadados[1], metadados[0], nome_arquivo_dados);
    // cria_arq_dados(1, metadados[0], nome_arquivo_idx_prim);

    printf("\nEscrevendo metadados do hashing...\n");    
    escreve_metadados_hash_arquivo(metadados[0], metadados[1], metadados[2], metadados[3], nome_arquivo_dados);
    
    printf("\tMetadados escritos:\n\t");
    char* bloco_meta = ler_bloco_do_arquivo(0, nome_arquivo_dados, metadados[0]);
    
    char int_str[4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int_str[j] = bloco_meta[(i*sizeof(int)) + j];
        }
        printf("%d ", converter_chars_para_int(int_str));
    }
    printf("\n");
    free(bloco_meta);

    ArvoreBP* indice_prim = criaArvore(tam_bloco/(sizeof(int) + sizeof(long)));

    printf("\nSalvando dados (Isso deve demorar alguns minutos)...\n");
    linha = le_linha(arquivo);
    while (linha.size() > 0) {
        cont++;

        // Lê um registro do arquivo
        r = parse(linha, arquivo);
        endereco_hash = aplica_hashing(&r, metadados[0], metadados[1], nome_arquivo_dados);
        //endereco_idx = inserir_nova_chave(indice_prim, r.ID, endereco_hash, tam_bloco, nome_arquivo_idx_prim);
        //printf("%do id salvo: %d (h.%ld, i.%ld)\n", cont, r.ID, endereco_hash, endereco_idx);
        
        linha = le_linha(arquivo);
    }
    printf("\nInserção concluída.\n");
    printf("Número de artigos: %d\n", cont);

    // printf("\nEscrevendo metadados do índice primário...\n");    
    // escreve_metadados_idx_arquivo(metadados[0], numBlocos, indice_prim->raiz->meuEndereco, metadados[0], nome_arquivo_dados);
    // printf("Metadados salvos.\n");

    fclose(arquivo);
}

int main(int argc, char** argv) {
    string nome_arquivo_entrada = argv[1];//"artigo.csv";
    string nome_arquivo_dados = "arquivo_dados.bin";
    string nome_arquivo_idx_prim = "arquivo_indice_primario.bin";
    string nome_arquivo_idx_sec = "arquivo_indice_secundario.bin";

    le_arquivo_csv(nome_arquivo_entrada, nome_arquivo_dados, nome_arquivo_idx_prim, nome_arquivo_idx_sec);

    return 0;
}
