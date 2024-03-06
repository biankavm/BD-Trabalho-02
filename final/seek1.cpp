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

NoRAM *criaNoRAM(int maximo)
{
    // Create new tree node
    NoRAM *no = (NoRAM *) malloc(sizeof(NoRAM));
    NoDisco *dados = (NoDisco*)malloc(sizeof(NoDisco));
    
    if (no != NULL && dados !=NULL)
    {
        // Create memory of node key
        no->dados = dados;
        no->dados->chaves = (int *) malloc((sizeof(int)) *(maximo+1));
        no->dados->enderecos = (long*) malloc((sizeof(long))*(maximo+1));

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

typedef struct Registro {
    int ID;
    char titulo[300];
    int ano;
    char autores[150];
    int citacoes;
    char atualizacao[20];
    char snippet[1024];
} Registro;


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

void mostraRegistro(Registro* r){
    printf("ID:\t\t%d\n", r->ID);
    cout << "Titulo:\t\t" << r->titulo << endl;
    printf("Ano:\t\t%d\n", r->ano);
    cout << "Autores:\t" << r->autores << endl;
    printf("Citações:\t%d\n", r->citacoes);
    cout << "Atualização:\t" << r->atualizacao << endl;
    cout << "Snippet:\n\t\t" << r->snippet << endl;
}

int converter_chars_para_int(char* cs) {
    int retorno = 0;
    int num_char = sizeof(int);

    for (int i = 0; i < num_char; i++) {
        retorno |= ((unsigned char)cs[i] << (8*i));
    }
    
    return retorno;
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

void ler_campo(char* dest_str, int posicao, int tam, char* bloco) {
    for (int i = 0; i < tam; i++) {
        dest_str[i] = bloco[i+posicao];
    }
    return;
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

int converter_chars_para_long(char* cs) {
    int retorno = 0;
    int num_char = sizeof(long);

    for (int i = 0; i < num_char; i++) {
        retorno |= ((unsigned char)cs[i] << (8*i));
    }
    
    return retorno;
}
int numBlocos = 0;
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
    //printf("enderecoBloco: %d,numBlocos: %d\n",endereco_bloco, numBlocos);
    int num_lidos = fread(bloco, tam_bloco, 1, arquivo);
    //printf("numlidos: %d\n",num_lidos);
    
    if (num_lidos != 1) {
        printf("Erro ao ler o arquivo.\n");
        fclose(arquivo);
        return NULL;
    }
    
    fclose(arquivo);
    return bloco;
}
NoRAM *ler_NoDisco_do_disco(NoRAM *noVazio, string arqSaida, int tamBloco, ArvoreBP *abp){
    // ler bloco com aquele endereço(ele está em char)
    char* bloco = NULL;
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
        //printf("No não vazio\n");
        dis = noVazio->dados;
        noVazio->dados->chaves = (int *) malloc((sizeof(int)) *(abp->maximo));
        noVazio->dados->enderecos = (long*) malloc((sizeof(long))*(abp->maximo));
    }
    ler_registro_do_bloco_index(noVazio->dados, bloco,tamBloco);
    free(bloco);
    
    
    return noVazio;
}
int liberar_NoDisco_da_RAM(NoRAM *no){
//retorna 1 se foi bem sucedido, 0 se não
    free(no->dados->chaves);
    free(no->dados->enderecos);
    free(no->dados);
    no->dados = NULL;
    return 1;
}
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

void transformaRamDisco(NoRAM* nor, NoRAM* aux, ArvoreBP* abp, int i){
    nor->esFolha = aux->dados->chaves[abp->maximo-2];
    nor->meuEndereco = nor->dados->enderecos[i];
    nor->dados->chaves = aux->dados->chaves;
    nor->dados->enderecos = aux->dados->enderecos; 
}

int contar_valores(int vetor[], int tamanho) {
    int contador = 0;
    for (int i = 0; i < tamanho; i++) {
        if (vetor[i] == 0) {
            break; // Sai do loop se encontrar um valor igual a 0
        }
        contador++;
    }
    return contador;
}

long busca_endereco_NoDisco(ArvoreBP* abp, int chave, string testArq, int tamBloco) {
    NoRAM *galho = abp->raiz;
    NoRAM *pai = NULL;
    int i = 0;
    int j = 0;
    int endPai = 0;
    NoRAM *aux = (NoRAM *) malloc(sizeof(NoRAM));
    NoDisco *dados = (NoDisco*)malloc(sizeof(NoDisco));
    aux->dados = dados;
    aux->dados->chaves = (int *) malloc((sizeof(int)) *(abp->maximo+1));
    aux->dados->enderecos = (long*) malloc((sizeof(long))*(abp->maximo+1));


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

                galho->meuEndereco = galho->dados->enderecos[i];
                liberar_NoDisco_da_RAM(galho);
                ler_NoDisco_do_disco(galho,testArq,tamBloco,abp);
                galho->esFolha = galho->dados->chaves[abp->maximo-2];
                galho->numChaves = contar_valores(galho->dados->chaves,abp->maximo-2);
                break;
            }
            if (i == galho->numChaves - 1)
            {
                galho->meuEndereco = galho->dados->enderecos[i+1];
                liberar_NoDisco_da_RAM(galho);
                ler_NoDisco_do_disco(galho,testArq,tamBloco,abp);
                galho->esFolha = galho->dados->chaves[abp->maximo-2];
                galho->numChaves = contar_valores(galho->dados->chaves,abp->maximo-2);
                break;
            }
        }
    }
    for(int e = 0; e < galho->numChaves; e++){
        if(chave == galho->dados->chaves[e]){
            return galho->dados->enderecos[e];
        }
    }
    return 0;
}


/*
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
}*/

int* ler_metadados(int tam_bloco, string nome_arquivo_dados) {
    char* bloco = ler_bloco_do_arquivo(0, nome_arquivo_dados, tam_bloco);
    char int_str[4];
    int* metadados = (int*) malloc(sizeof(int) * 4);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int_str[j] = bloco[(i*sizeof(int)) + j];
        }
        metadados[i] = converter_chars_para_int(int_str);
    }
    free(bloco);

    return metadados;
}

int main(int argc, char** argv){
    string arqIndices = "arquivo_indice_primario.bin";
    string arqDados = "arquivo_dados.bin";
    int tamBloco = acha_tamanho_dos_blocos();
    int maximo = 10;
    ArvoreBP* abp = criaArvore(maximo);
    NoRAM *raiz = criaNoRAM(abp->maximo);
    abp->raiz = raiz;
    raiz->meuEndereco = 0;
    ler_NoDisco_do_disco(raiz,arqIndices,tamBloco,abp);
    raiz->esFolha = raiz->dados->chaves[abp->maximo-2];
    raiz->numChaves = contar_valores(raiz->dados->chaves,abp->maximo-2);

    int chave = 0;


    int* metadados = ler_metadados(tamBloco, arqDados);
    
    int num_blocos = metadados[1];
    int num_linhas = metadados[2];
    int tam_reg_hash = metadados[3];
    free(metadados);

    //cout << "Digite um id para a bsuca por indice: ";
    //cin >> chave;
    chave = stoi(argv[1]);
    int endDado = busca_endereco_NoDisco(abp,chave, arqIndices,tamBloco);


    char* bloco = ler_bloco_do_arquivo(endDado, arqDados, tamBloco);
    Registro* r = (Registro*) malloc(sizeof(Registro));
    
    ler_registro_do_bloco_dados(r, bloco, chave, tamBloco, tam_reg_hash);

    if (r->ID == -1) {
        cout << "Artigo não encontrado.\n";
        return 1;
    }

    printf("Artigo encontrado:\n");
    mostraRegistro(r);
    return 0;
}