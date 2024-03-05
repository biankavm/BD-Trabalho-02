#include <stdio.h>
#include <stdlib.h>

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
    NoRAM **filhos;
}NoRAM;

//arvore
typedef struct tipoArvoreBP
{
    NoRAM *raiz;
    int maximo;
}ArvoreBP;


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

int escrever_NoDisco_no_disco(NoRAM *noR, char novo){
//retorna 1 se foi bem sucedido, 0 se não
}
NoRAM *ler_NoDisco_do_disco(long endereco, NoRAM *noVazio){

    return noVazio;
}

int liberar_NoDisco_da_RAM(NoRAM *no){
//retorna 1 se foi bem sucedido, 0 se não
}
