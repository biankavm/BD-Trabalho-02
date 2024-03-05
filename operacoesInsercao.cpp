#include <stdio.h>
#include <stdlib.h>

using namespace std;

// Dados que vão para o darquivo de índice
typedef struct tipoNoDisc {
    int *chaves;
    long *enderecos;
} NoDisco;

// Dados que ficam em memória principal
typedef struct tipoNoRam {
    int esFolha;
    long meuEndereco;
    int numChaves;
    NoDisco *dados;
    struct tipoNoRam **filhos;
} NoRAM;

// Árvore B+
typedef struct tipoArvoreBP {
    NoRAM *raiz;
    int maximo;
} ArvoreBP;


// ======================================================================
// Funções de criação de structs

// Retorna o ponteiro para uma struct que representa a arvore B+
ArvoreBP *criaArvore(int maximo) {
    return NULL;
}

// Retorna um novo nó de memória principal vazio
NoRAM *criaNoRAM(int maximo) {
    return NULL;
}


// ======================================================================
// Operações básicas envolvendo o disco

// 
int escrever_NoDisco_no_disco(NoRAM *noR, char novo){
    //retorna 1 se foi bem sucedido, 0 se não
    return 0;
}

//
NoRAM *ler_NoDisco_do_disco(long endereco, NoRAM *noVazio){
    return noVazio;
}

// 
int liberar_NoDisco_da_RAM(NoRAM *no){
    //retorna 1 se foi bem sucedido, 0 se não
    return 0;
}


// ======================================================================
// Funções da árvore

long inserir_NoRAM_interno_arvore(ArvoreBP* abp, int chave, long endereco, NoRAM* atual, NoRAM* filho);

// Faz a busca pela chave, caminhando pelo galho da árvore, 
// e retorna o endereço do nó no disco.
long* busca_endereco_NoRAM(ArvoreBP* abp, int chave) {
    return 0;
}


// Seleciona apenas o endereço do pai ou do filho do conjunto de 
// endereços retornados pela função de busca
long busca_endereco_pai_filho(ArvoreBP* abp, int chave_filho, int retorna_pai) {
    long endereco = 0;
    long* enderecos = (long*) malloc(sizeof(long) * 2);
    enderecos = busca_endereco_NoRAM(abp, chave_filho);

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
NoRAM* achar_NoRAM_pai(ArvoreBP* abp, int chave_filho) {
    if (!abp->raiz) {
        return NULL;
    }

    // Recebe um vetor de endereços
    long endereco = 0;
    endereco = busca_endereco_pai_filho(abp, chave_filho, 1);

    // Se o endereço existir
    if (!endereco) {
        return NULL;
    }

    // Inicializa o nó para poder procurá-lo no disco
    NoRAM* pai = criaNoRAM(abp->maximo);
    pai->meuEndereco = endereco;

    ler_NoDisco_do_disco(endereco, pai);
    if (!pai->meuEndereco) {
        return NULL;
    }
    
    return pai;
}



// Para inserções em nós que têm espaço sobrando, só são feitas atualizações nos nós já existentes
NoRAM* tenta_inserir_No_vago(ArvoreBP* abp, int chave, long endereco, NoRAM* atual, NoRAM* filho) {
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
        printf("Atualizando do nó atual (endereço: %ld)...\n", atual->meuEndereco);
        escrever_NoDisco_no_disco(atual, 0);
        printf("Escrevendo nó filho (endereço: %ld)...\n", filho->meuEndereco);
        escrever_NoDisco_no_disco(filho, 1);

        // Liberando dados extras desnecessários
        printf("Liberando NoDisco do nó atual (endereço: %ld)...\n", atual->meuEndereco);
        liberar_NoDisco_da_RAM(atual);
        printf("Liberando NoDisco do nó filho (endereço: %ld)...\n", filho->meuEndereco);
        liberar_NoDisco_da_RAM(filho);

        return filho;
    }
    else {
        return NULL;
    }
}


// Para inserções em nós cheios, faz o split
void insere_em_split(ArvoreBP* abp, int chave, long endereco, NoRAM* atual, NoRAM* filho) {
    int i = 0;
    int j = 0;

    NoRAM* novo_interno = criaNoRAM(abp->maximo);

    // A cópia das chaves tem o mesmo tamanho dos vetores de endereços 
    // e ponteiros porque sua última posição é o booleano de ser ou não folha
    int* copia_chaves = (int*) malloc(sizeof(int) * (abp->maximo + 2));
    long* copia_enderecos = (long*) malloc(sizeof(long) * (abp->maximo + 2));
    NoRAM** copia_filhos = (NoRAM**) malloc(sizeof(NoRAM*) * (abp->maximo + 2));
    
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
        printf("Escrevendo novo nó raiz (endereço: %ld)...\n", abp->raiz->meuEndereco);
        escrever_NoDisco_no_disco(abp->raiz, 1);
        printf("Liberando NoDisco do nó raiz (endereço: %ld)...\n", abp->raiz->meuEndereco);
        liberar_NoDisco_da_RAM(abp->raiz);
    }
    else {
        inserir_NoRAM_interno_arvore(abp, atual->dados->chaves[atual->numChaves],  atual->dados->enderecos[atual->numChaves], achar_NoRAM_pai(abp, atual->meuEndereco), novo_interno);
    }

    // Operações de atualização do nó atual e escrita do novo nó filho
    printf("Atualizando do nó atual (endereço: %ld)...\n", atual->meuEndereco);
    escrever_NoDisco_no_disco(atual, 0);
    printf("Escrevendo novo nó interno (endereço: %ld)...\n", novo_interno->meuEndereco);
    escrever_NoDisco_no_disco(novo_interno, 1);
    printf("Escrevendo nó filho (endereço: %ld)...\n", filho->meuEndereco);
    escrever_NoDisco_no_disco(filho, 1);

    // Liberando dados extras desnecessários
    printf("Liberando NoDisco do nó atual (endereço: %ld)...\n", atual->meuEndereco);
    liberar_NoDisco_da_RAM(atual);
    printf("Liberando NoDisco do novo nó interno (endereço: %ld)...\n", novo_interno->meuEndereco);
    liberar_NoDisco_da_RAM(novo_interno);
    printf("Liberando NoDisco do nó filho (endereço: %ld)...\n", filho->meuEndereco);
    liberar_NoDisco_da_RAM(filho);

    free(copia_chaves);
    free(copia_enderecos);
    free(copia_filhos);
}



// Insere um nó interno na árvore
// Vai receber a chave e o endereço a serem inseridos, além dos 
// mesmos nós.  Vai retornar o endereço atrelado à chave inserida 
// (importante para o índice secundário, retorna 0 para o índice 
// primário).
long inserir_NoRAM_interno_arvore(ArvoreBP* abp, int chave, long endereco, NoRAM* atual, NoRAM* filho) {
    NoRAM* novo_no = NULL; 

    // Primeiro caso de inserção: em um nó vago, se não funcionar, vai para o próximo caso
    if (!(novo_no = tenta_inserir_No_vago(abp, chave, endereco, atual, filho))) {
        printf("Vai dar split para o nó filho (%ld) e nó atual (%ld)\n", filho->meuEndereco, atual->meuEndereco);
        insere_em_split(abp, chave, endereco, atual, filho);
    }
    return filho->meuEndereco;
}

// Insere uma nova chave na árvore
// Vai retornar o endereço do nó atrelado à chave inserida em disco.
long inserir_nova_chave(ArvoreBP* abp, int chave, long endereco) {
    if (abp->raiz == NULL) {
        // Add first node of tree
        abp->raiz = criaNoRAM(abp->maximo);
        abp->raiz->numChaves   = 1;
        abp->raiz->esFolha  = 1;
        abp->raiz->dados->chaves[abp->maximo]  = 1;
        abp->raiz->dados->chaves[0] = chave;
        abp->raiz->dados->enderecos[0] = endereco;

        printf("Primeiro valor: Escrevendo nó raiz (endereço: %ld)...\n", abp->raiz->meuEndereco);
        escrever_NoDisco_no_disco(abp->raiz, 1);
        printf("Liberando NoDisco do nó raiz (endereço: %ld)...\n", abp->raiz->meuEndereco);
        liberar_NoDisco_da_RAM(abp->raiz);

        return abp->raiz->meuEndereco;
    }
    else {
        // Loop controlling variables
        int i = 0;
        int j = 0;

        NoRAM *atual = abp->raiz;
        NoRAM *pai = NULL;

        // Caminha na árvore até chegar à folha onde a chave deve ser inserida
        while (atual->esFolha == 0) {
            pai = atual;
            for (i = 0; i < atual->numChaves; i++) {
                if (chave < atual->dados->chaves[i]) {
                    atual = atual->filhos[i];
                    break;
                }
                if (i == atual->numChaves - 1) {
                    atual = atual->filhos[i + 1];
                    break;
                }
            }
        }

        // Caso o nó esteja vago
        if (atual->numChaves < abp->maximo) {
            i = 0;

            // Procura o espaço ideal para a nova chave
            while (chave > atual->dados->chaves[i] && i < atual->numChaves) {
                i++;
            }

            // Desloca as chaves para a direita
            for (j = atual->numChaves; j > i; j--) {
                atual->dados->chaves[j] = atual->dados->chaves[j - 1];
            }

            for (j = atual->numChaves + 1; j > i + 1; j--) {
                atual->dados->enderecos[j] = atual->dados->enderecos[j - 1];
            }

            // Insere chave e endereço
            atual->dados->chaves[i] = chave;
            atual->dados->enderecos[i] = endereco;
            atual->numChaves++;

            // Aponta para a próxima folha
            atual->filhos[atual->numChaves] = atual->filhos[atual->numChaves - 1];
            atual->dados->enderecos[atual->numChaves] = atual->dados->enderecos[atual->numChaves - 1];
            atual->filhos[atual->numChaves - 1] = NULL;
            atual->dados->enderecos[atual->numChaves - 1] = 0;

            // Atualiza nó folha
            printf("Atualizando do nó atual (endereço: %ld)...\n", atual->meuEndereco);
            escrever_NoDisco_no_disco(atual, 0);
            printf("Liberando NoDisco do nó atual (endereço: %ld)...\n", atual->meuEndereco);
            liberar_NoDisco_da_RAM(atual);

            return atual->meuEndereco;
        }

        // Caso o nó esteja cheio, faz o split
        else {
            NoRAM *nova_folha = criaNoRAM(abp->maximo);

            int* copia_chaves = (int*) malloc(sizeof(int) * (abp->maximo + 2));
            long* copia_enderecos = (long*) malloc(sizeof(long) * (abp->maximo + 2));
 
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

            // Cria nova raiz
            if (atual == abp->raiz) {
                NoRAM *nova_raiz = criaNoRAM(abp->maximo);
             
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
                printf("(inserir_nova_chave()) Escrevendo novo nó raiz (endereço: %ld)...\n", abp->raiz->meuEndereco);
                escrever_NoDisco_no_disco(abp->raiz, 1);
                printf("Liberando NoDisco do nó raiz (endereço: %ld)...\n", abp->raiz->meuEndereco);
                liberar_NoDisco_da_RAM(abp->raiz);
            }
            else
            {
                inserir_NoRAM_interno_arvore(abp, nova_folha->dados->chaves[0],nova_folha->dados->enderecos[0], pai, nova_folha);
            }

            // Operações de atualização do nó atual e escrita do novo nó filho
            printf("Atualizando do nó atual (endereço: %ld)...\n", atual->meuEndereco);
            escrever_NoDisco_no_disco(atual, 0);
            printf("Escrevendo nó folha (endereço: %ld)...\n", nova_folha->meuEndereco);
            escrever_NoDisco_no_disco(nova_folha, 1);

            // Liberando dados extras desnecessários
            printf("Liberando NoDisco do nó atual (endereço: %ld)...\n", atual->meuEndereco);
            liberar_NoDisco_da_RAM(atual);
            printf("Liberando NoDisco do nó folha (endereço: %ld)...\n", nova_folha->meuEndereco);
            liberar_NoDisco_da_RAM(nova_folha);

            free(copia_chaves);
            free(copia_enderecos);

            return nova_folha->meuEndereco;
        }
    }
}


// ======================================================================
// Funções de teste



int main() {

}