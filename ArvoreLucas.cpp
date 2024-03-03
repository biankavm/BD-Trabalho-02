#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring> // Para as funções de manipulação de strings C
#include <vector>
#include <limits>
#include <unordered_map>
#include <stack>

#define ORDEM 5

using namespace std;

typedef struct Nost {
    int* chaves; // 0 para galho, 1 para folha
    int* enderecos; // Agora armazena objetos No diretamente
}No;

/*
No *buffer; // um vetor de nós salvos em memória principal
int *endBuff; //um vetor que possui os endereços dos nós do buffer 
*/

int endRaiz = 1; //primeiro bloco de dados

No* inicializaNo2(int tamBloco){
    printf("maloc incio\n");
    int numIntVect = tamBloco/(sizeof(int)*2); //calcula o tamanho dos vectores de inteiros dentro dos nós
    No *no = (No*)malloc(sizeof(No)+(sizeof(int)*numIntVect)*2); //inicializa o no
    //no->chaves = (int*)malloc(sizeof(int)*numIntVect); //aloca o tamanho do vetor de chaves que depende do tamanhoo do bloco do sistema
    //printf("malloc 1 funcionou\n");
    //no->enderecos = (int*)malloc(sizeof(int)*numIntVect); //aloca o tamanho do vetor de chaves que depende do tamanhoo do bloco do sistema
    printf("maloc 2 funcionou\n");
    memset(no->chaves, 0, numIntVect*sizeof(int)); //seta todos os valores do vetor como 0
    memset(no->enderecos, 0, numIntVect*sizeof(int)); //seta todos os valores do vetor como 0
    return no; //retorna o no
}

No* inicializaNo(int tamBloco){
    printf("malloc inicio\n");
    int numIntVect = tamBloco / (sizeof(int) * 2); // calcula o tamanho dos vetores de inteiros dentro dos nós
    No *no = (No*)malloc(sizeof(No));

    // Aloca a memória para os vetores chaves e enderecos
    no->chaves = (int*)malloc(sizeof(int) * numIntVect);
    no->enderecos = (int*)malloc(sizeof(int) * numIntVect);

    // Verifica se a alocação foi bem-sucedida
    if (no->chaves == NULL || no->enderecos == NULL) {
        // Trate o erro de alocação de memória aqui, se necessário
        printf("Erro na alocação de memória\n");
        exit(EXIT_FAILURE);
    }

    printf("malloc funcionou\n");
    return no; // retorna o nó
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

// Função para ler um registro de um bloco de arquivo de dados, dado um id 
// Função para ler um registro de um bloco de arquivo de dados, dado um id 
int ler_registro_do_bloco_index(No* no, char* bloco, int tam_bloco) {
    // Calculando tamanhos dos campos dos registros
    int size = sizeof(int);
    int pos = (tam_bloco)/2;
    int id_atual = -1;
    int num_chaves = tam_bloco/(2*sizeof(int));
    char id_str[sizeof(int)];
    
    // Lê os chars binários do vetor de chaves
    for (int reg = 0; reg < num_chaves; reg++) {
        // Pega os chars de um int
        for (int i = 0; i < size; i++) {
            id_str[i] = bloco[(reg * size)+i];
        }
        id_atual = converter_chars_para_int(id_str);
        no->chaves[reg] = id_atual;
    }

    // Lê os chars binários do vetor de endereços
    for (int reg = 0; reg < num_chaves; reg++) {
        // Pega os chars de um int
        for (int i = 0; i < size; i++) {
            id_str[i] = bloco[pos + (reg * size)+i];
        }
        id_atual = converter_chars_para_int(id_str);
        no->enderecos[reg] = id_atual;
    }
    
    return 0;
}

char* ler_bloco_do_arquivo(double endereco_bloco, string nome_arquivo_saida, int tam_bloco) {
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

// Função de arquivo equivalente a uma operação "no->filho[posicao]"" em uma árvore
No* carregaNo(No* no, int endereco, int tamanhoBloco, string arquivo_entrada) {
    // Acessa o bloco
    char* bloco = ler_bloco_do_arquivo(endereco, arquivo_entrada, tamanhoBloco);

    // Lê o nó do bloco
    ler_registro_do_bloco_index(no, bloco, tamanhoBloco);

    return no;
}




// Função para trocar dois elementos de posição em um vetor
void trocar(int &a, int &b) {
    int temp = a;
    a = b;
    b = temp;
}

int verificaZero(int vetor[], int tamanho) {
    for (int i = 0; i < tamanho; ++i) {
        if (vetor[i] == 0) {
            return 1; // Retorna 1 se encontrar um zero
        }
    }
    return 0; // Retorna 0 se nenhum zero for encontrado
}

// Função para particionar o vetor e retornar o índice do pivô
int particionar(int vetor[], int inicio, int fim) {
    int pivot = vetor[fim]; 
    int i = (inicio - 1); 

    for (int j = inicio; j < fim; j++) {
        if (vetor[j] <= pivot) {
            i++; 
            trocar(vetor[i], vetor[j]);
        }
    }
    trocar(vetor[i + 1], vetor[fim]);
    return (i + 1);
}

// Função de ordenação Quick Sort
void quickSort(int vetor[], int inicio, int fim) {
    if (inicio < fim) {
        int pi = particionar(vetor, inicio, fim); 
        quickSort(vetor, inicio, pi - 1);
        quickSort(vetor, pi + 1, fim);
    }
}

void inserirValor2(int vetor[], int tamanho, int valor, int indice, int mantem) {

    int valFim = vetor[tamanho-1];
    // Desloca os valores para abrir espaço para o novo valor
    for (int i = tamanho - 1; i > indice; --i) {
        vetor[i] = vetor[i - 1];
    }

    if(mantem ==1)
    {
        vetor[tamanho-1] = valFim;
    }
    vetor[indice] = valor;
}

int acha_quantidade_chaves(){
    return acha_tamanho_dos_blocos() / (2*sizeof(int));
}

int* criarVetorAleatorio(int tamanho) {
    int* vetor = new int[tamanho]; // Aloca memória para o vetor

    // Preenche o vetor com zeros
    for (int i = 0; i < tamanho; ++i) {
        vetor[i] = 0;
    }

    return vetor;
}

int posicaoInsercao = 0;
void inserirValor(int vetor[], int tamanho, int valor, int mantem) {
    // Procura a posição onde o valor deve ser inserido
    int ultimoVal = vetor[tamanho-1];

    while (posicaoInsercao < tamanho && vetor[posicaoInsercao] < valor && vetor[posicaoInsercao] != 0) {
        posicaoInsercao++;
    }

    // Desloca os valores para abrir espaço para o novo valor
    for (int i = tamanho - 1; i > posicaoInsercao; --i) {
        vetor[i] = vetor[i - 1];
    }

    if(mantem == 1)
    {
        vetor[tamanho-1] = ultimoVal;
    }


    // Insere o novo valor na posição apropriada
    vetor[posicaoInsercao] = valor;
}

int ultimoBloco = 0; //é uma variavel global que diz qual o endereço do proximo bloco a ser inserido

void insercao(int chave, int enderecoRegistro, int tamBloco, string arquivo) {
    stack<No*> S; // guarda a struct dos pais
    stack<int> Send; //guarda os endereços dos pais 


    
    No* n = inicializaNo(tamBloco); //adquire o nó raiz, provavelmente o endereço desse nó é 1
    cout << "inicializei o no" << endl;
    int q = sizeof(n->enderecos)/sizeof(n->enderecos[0]); // isso aqui serve pra calcular a quantidade do vetor de chaves/endereços

    int novoEnd = 0; //serve para ser o endereço do novo nó, caso aja um split
    while (n->chaves[q-1] == 0  && n->enderecos[q-1] == 0) { //enquanto não é uma folha, pois o ultimo valor do vetor de chaves indica se é ou não uma folha
        //cout << "entrei no while de percorre a arvore" << endl;
        S.push(n); // guarda o papai
        Send.push(1); //provavelmente o endereço da raiz vai ser 1
        if (chave <= n->chaves[0]) {
            //cout << "chave é menor ao primeiro cara" << endl;
            n = carregaNo(n, n->enderecos[0], acha_tamanho_dos_blocos(), arquivo); 
            Send.push(n->enderecos[0]);
        }
        else if (chave > n->chaves[q - 2]) {
            //cout << "chave é maior ao ultimo cara" << endl;
            n = carregaNo(n, n->enderecos[q - 2], acha_tamanho_dos_blocos(), arquivo);
            Send.push(n->enderecos[q - 2]);
        }
        else {
            //cout << "chave nao atendeu aos dois ifs anteriores" << endl;
            size_t i = 1;
            while (chave > n->chaves[i]) {
                ++i;
            }
            n = carregaNo(n, n->enderecos[i], acha_tamanho_dos_blocos(), arquivo);
            Send.push(n->enderecos[i]);
        }
    }

    for(int x = 0;x < q-1; x++) //procura valor
    {   
        //cout << "vou procurar um valor" << endl;
        if(n->chaves[x]==chave){ //existe uma chave igual
            break;
        }
    }

    int folhaCheia = 0;

    if(n->chaves[q-1] != 0)
    {
        cout << "folha nao cheia cheia" << endl;

        folhaCheia = 1;
    }
    if(folhaCheia == 0)
    {
        cout << "encheu a folha" << endl;
        inserirValor(n->chaves, q-1, chave,1); //se a folha não está cheia, é só inserir o valor no lugar certo
        inserirValor2(n->chaves,q-1,chave,posicaoInsercao,1);
        posicaoInsercao = 0;
    }
    if(folhaCheia == 1) //se a folha não está cheia, aí complica
    {
        No* temp = inicializaNo(tamBloco); //nó temporário que copia n para fazer as coisas, lembrar de dar free
        temp->chaves = n->chaves;
        temp->enderecos = n->enderecos;
        inserirValor(temp->chaves, q-1, chave,0); 
        inserirValor2(temp->chaves,q-1,chave,posicaoInsercao,0);

        posicaoInsercao = 0; //não ligue pra isso, só é pra inserir nos vetores

        No* novo = inicializaNo(tamBloco); //novo nó para o split

        novo->enderecos[q-1] = n->enderecos[q-1];  // o novo nó aponta para o que o ultimo nó n apontava
        ultimoBloco++; //soma o contador universal de blocos indicando a criação de um novo

        int j = q/2; //isso é um calculo adaptado do livro, tinha haver com oo 2m

        int *chaveTemp = criarVetorAleatorio(q); //
        int *endTemp = criarVetorAleatorio(q); //cria um vetor de 0s

        int a = 0;
        for(a = 0; a < j; a++) //preenche esses valores com a primeira metade dos antigos valores de n
        {
            chaveTemp[a] = temp->chaves[a]; 
            endTemp[a] = temp->enderecos[a]; 
        }
        

        endTemp[q-1] = ultimoBloco; // n recebe ponteiro para o bloco novo
        chaveTemp[q-1] = 1; //diz que é folha
        ultimoBloco++;
        n->chaves = chaveTemp;
        n->enderecos = endTemp;

        chaveTemp = criarVetorAleatorio(q);
        endTemp = criarVetorAleatorio(q);
        int y = 0;
        chave = temp->chaves[j];
        while(a<q){ //preenche a esses vetores com a segunda metade de n
            cout << "preenchendo metade..." << endl;
            //cout << "a: " << a << "q: " << q << endl;
            chaveTemp[y] = temp->chaves[a];
            endTemp[y] = temp->enderecos[a];
            a++;
            y++;
        }
        chaveTemp[q-1] = 1;
        novo->chaves = chaveTemp;
        novo->enderecos = endTemp;  

        // pelo o que eu entendi, se não entrou nos casos anteriores, significa que não é folha, então resto deve tratar os nós internos
        //É MUITO POSSIVEL QUE ISSO DEVERIA SER DENTRO DO IF DA FOLHA CHEIA -- ok, definitivamente deve ser, maldito livro desorganizado
        char finished = 0;
        while(finished == 0)
        {
            if(Send.empty()){ //se a pilha de endereços dos pais
                cout << "pilha dos pais vazia" << endl;
                No* raiz = inicializaNo(tamBloco); //cria um nó raíz
                raiz->chaves[0] = chave; 
                inserirValor(raiz->chaves,q-1,chave,1);
                inserirValor2(raiz->enderecos,q-1,2,posicaoInsercao,1); //assumo que os valores dos blocos filhos da rais sejam 2 e 3
                inserirValor2(raiz->enderecos,q-1,3,posicaoInsercao,1);
                finished = 1;
            }
            else{
                cout << "pilha dos pais nao vazia" << endl;
                n = S.top(); //pega o nó no topo do stack
                S.pop();
                

                if(verificaZero(n->chaves, q-1)) // verifica se o nó papai tá cheio, se tiver uma chave 0, significa que o vetor tem espaço vazio
                {
                    inserirValor(n->chaves, q-1, chave,1); 
                    inserirValor2(n->chaves,q-1,chave,posicaoInsercao,1);
                    

                    posicaoInsercao = 0;
                    finished = 1;
                }
                else
                {
                    
                    No* temp = inicializaNo(tamBloco); //nó temporário que copia n para fazer as coisas, lembrar de dar free
                    
                    temp->chaves = n->chaves;
                    temp->enderecos = n->enderecos;

                    int pontFim = temp->enderecos[q-1];

                    inserirValor(temp->chaves, q-1, chave,0); 
                    inserirValor2(temp->chaves,q-1,chave,posicaoInsercao,0);
                    
                    posicaoInsercao = 0; //não ligue pra isso, só é pra inserir nos vetores

                    No* novo = inicializaNo(tamBloco); //novo nó para o split
                    
                    novo->enderecos[q-1] = n->enderecos[q-1];  // o novo nó aponta para o que o ultimo nó n apontava
                    ultimoBloco++; //soma o contador universal de blocos indicando a criação de um novo

                    int j = q/2; //isso é um calculo adaptado do livro, tinha haver com oo 2m

                    chaveTemp = criarVetorAleatorio(q); //
                    endTemp = criarVetorAleatorio(q); //cria um vetor de 0s
                    
                    int a = 0;
                    for(a = 0; a < j; a++) //preenche esses valores com a primeira metade dos antigos valores de n
                    {
                    chaveTemp[a] = temp->chaves[a]; 
                    endTemp[a] = temp->enderecos[a]; 
                    }
                    endTemp[q-1] = ultimoBloco; // n recebe ponteiro para o bloco novo
                    chaveTemp[q-1] = 0; //diz que é galho
                    ultimoBloco++;
                    n->chaves = chaveTemp;
                    n->enderecos = endTemp;

                    chaveTemp = criarVetorAleatorio(q);
                    endTemp = criarVetorAleatorio(q);
                    int y = 0;
                    chave = temp->chaves[j];
                    while(a<q){ //preenche a esses vetores com a segunda metade de n
                    cout << "WHILEE..." << endl;
                    chaveTemp[y] = temp->chaves[a];
                    endTemp[y] = temp->enderecos[a];
                    cout << "OU" << endl;
                    a++;
                    y++;
                    }
                    chaveTemp[q-1] = 0;
                    novo->chaves = chaveTemp;
                    novo->enderecos = endTemp;
                }
            }
        }

    }
}

int main() {
    // Definindo o tamanho do bloco
    int tamBloco = acha_tamanho_dos_blocos();

    // Criando um arquivo de saída
    string arquivo_saida = "saida.bin";

    // Inicializando a raiz da árvore
    No* raiz = inicializaNo(tamBloco);

    // Inserindo alguns valores de teste na árvore
    insercao(10, 1, tamBloco, arquivo_saida);
    insercao(20, 2, tamBloco, arquivo_saida);
    insercao(5, 3, tamBloco, arquivo_saida);
    insercao(15, 4, tamBloco, arquivo_saida);
    insercao(25, 5, tamBloco, arquivo_saida);

    // Outras operações de teste podem ser adicionadas aqui

    return 0;
}
