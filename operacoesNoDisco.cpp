#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <iostream>
#include <vector>
#include <algorithm>

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring> // Para as funções de manipulação de strings C
#include <limits>
#include <unordered_map>

using namespace std;

//dado qeu vai para o disco
typedef struct tipoNoDisc
{
    int *chaves;
    long *enderecos;
}NoDisco;

//dado que fica na ram
typedef struct tipoNoRam
{
    int esFolha;
    long meuEndereco;
    int numChaves;
    NoDisco *dados;
    struct tipoNoRam **filhos;
}NoRAM;

//arvore
typedef struct tipoArvoreBP
{
    NoRAM *raiz;
    int maximo;
}ArvoreBP;

int numBlocos = 0;


// FUNCIOOOOOOONAAAAAAA
// Retorna o ponteiro para uma struct que representa a arvore B+
ArvoreBP *criaArvore(int maximo)
{

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
// FUNCIOOOOOOONAAAAAAA
// Returns new B+ tree node
NoRAM *criaNoRAM(int maximo)
{
    // Create new tree node
    NoRAM *no = (NoRAM *) malloc(sizeof(NoRAM));
    NoDisco *dados = (NoDisco*)malloc(sizeof(NoDisco));
    
    if (no != NULL && dados !=NULL)
    {
        // Create memory of node key
        no->dados = dados;
        no->dados->chaves = (int *) malloc((sizeof(int)) *(maximo));
        no->dados->enderecos = (long*) malloc((sizeof(long))*(maximo));

        // Allocate memory of node child
        no->filhos = (NoRAM **) malloc((maximo + 1) *sizeof(NoRAM *));

        // Set initial child
        for (int i = 0; i <= maximo ; ++i)
        {
            no->filhos[i] = NULL;
        }
        no->esFolha = 0;
        no->numChaves = 0;
    }
    return no;
}

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


// FUNCIOOOOOOONAAAAAAA
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

        // Adiciona o vetor no bloco
        for (int i = 0; i < sizeLong; i++) {
            bloco[((reg + num_chaves) * sizeLong)+i] = id_str[i];
        }        
    }
    free(id_str);
    // printf("deu free no bloco escrever_registro_no_bloco\n");

    return 0;
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

// FUNCIOOOOOOONAAAAAAA
// Escreve um bloco char binario em um dado endereço em um arquivo
int escrever_bloco_no_arquivo(char* bloco, int endereco, string nome_arquivo_saida, int tam_bloco) {
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

    if (num_escritos != 1) {
        printf("Erro ao escrever no arquivo.\n");
        fclose(arquivo);
        return 1;
    }
    fclose(arquivo);
    return 0;
}

// Faz a leitura de um bloco de dado endereço no arquivo
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
    printf("enderecoBloco: %d,numBlocos: %d\n",endereco_bloco, numBlocos);
    int num_lidos = fread(bloco, tam_bloco, 1, arquivo);
    printf("numlidos: %d\n",num_lidos);
    
    if (num_lidos != 1) {
        printf("Erro ao ler o arquivo.\n");
        fclose(arquivo);
        return NULL;
    }
    
    fclose(arquivo);
    return bloco;
}

// FUNCIOOOOOOONAAAAAAA
int escrever_NoDisco_no_disco(NoRAM *noR, char novo, int tamBloco, string nomeArqSaida){
// Ele deve receber um ponteiro para um no, não vazio
// retorna 1 se foi bem sucedido, 0 se não
    char* bloco = NULL;


    if(novo == 1 ){
        printf("vai criar novo bloco no arquivo de dados\n");
        // vai criar um bloco de dados no arquivo
        numBlocos++;
        bloco = (char*)malloc(tamBloco);
        if(bloco ==NULL){
            printf("\t-bloco null --erro\n");
            return 0;
        }
        memset(bloco,0,tamBloco);
        printf("\t-criou novo bloco\n");
    }
    else{
        printf("vai atualizar um bloco existente\n");
        // vai atualizar os dados que ja existem no arquivo
        bloco = ler_bloco_do_arquivo(noR->meuEndereco, nomeArqSaida, tamBloco);

    }

    escrever_registro_no_bloco_index(noR->dados,bloco,tamBloco);
    printf("escreveu registro no bloco\n");
    escrever_bloco_no_arquivo(bloco,noR->meuEndereco,nomeArqSaida,tamBloco);
    printf("escreveu bloco no arquivo\n");

    free(bloco);
    //printf("deu free no bloco escrever_NoDisco\n");

    return 1;
}

//vai utilizar as informações de um nó da ram para carregar dados do disco
NoRAM *ler_NoDisco_do_disco(NoRAM *noVazio, string arqSaida, int tamBloco, ArvoreBP *abp){
    // ler bloco com aquele endereço(ele está em char)
    char* bloco = NULL;
    bloco = ler_bloco_do_arquivo(noVazio->meuEndereco, arqSaida, tamBloco);
    NoDisco *dis;
    if(noVazio->dados == NULL){
        printf("no vazio\n");
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

// FUNCIOOOOOOONAAAAAAA
int liberar_NoDisco_da_RAM(NoRAM *no){
//retorna 1 se foi bem sucedido, 0 se não
    free(no->dados->chaves);
    free(no->dados->enderecos);
    free(no->dados);
    no->dados = NULL;
    return 1;
}

int main(){

    printf("Testes:\n");
    int tamBloco = 40;
    ArvoreBP* abp = criaArvore(3);
    printf("[%ld], maximo: %d\n",abp->raiz, abp->maximo);

    printf("Arvore criada com sucesso!!\n");

    NoRAM *no1 = criaNoRAM(abp->maximo);
    printf("esfolha: %d, endereco:%d numChaves:%d, dados[chave]: %d, dados[enderecos]: %d, Ponteiro para dados: %d\n", no1->esFolha, no1->meuEndereco, no1->numChaves, no1->dados->chaves[0], no1->dados->enderecos[0], no1->dados);
    printf("No criado com sucesso!\n");

    printf("\tcriando novo registro\n");
    no1->dados->chaves[0] = 4;
    no1->dados->chaves[1] = 6;
    no1->dados->enderecos[0] = 1;
    no1->dados->enderecos[1] = 2;

    no1->numChaves = 2;

    for(int i = 0;  i<no1->numChaves; i++){
        printf("\t\t(%d,%d)\n",no1->dados->chaves[i],no1->dados->enderecos[i]);
    }
    printf("esfolha: %d, endereco:%d numChaves:%d, dados[chave]: %d, dados[enderecos]: %d, Ponteiro para dados: %d\n", no1->esFolha, no1->meuEndereco, no1->numChaves, no1->dados->chaves[0], no1->dados->enderecos[0], no1->dados);

    printf("\tsalvando registro no disco\n");

    string testArq = "testeArvoreteste.bin";
    escrever_NoDisco_no_disco(no1,1,tamBloco,testArq);

    printf("escreveu no arquivo binario...\n");

    liberar_NoDisco_da_RAM(no1);

    printf("dados removidos do no: %d da ram...\n", no1->meuEndereco);
    printf("esfolha: %d, endereco:%d numChaves:%d, Ponteiro para dados: %d\n", no1->esFolha, no1->meuEndereco, no1->numChaves, no1->dados);

    printf("carregando dados do disco\n");

    ler_NoDisco_do_disco(no1,testArq,tamBloco,abp);

    printf("esfolha: %d, endereco:%d numChaves:%d, dados[chave]: %d, dados[enderecos]: %d, Ponteiro para dados: %d\n", no1->esFolha, no1->meuEndereco, no1->numChaves, no1->dados->chaves[0], no1->dados->enderecos[0], no1->dados);

    
}