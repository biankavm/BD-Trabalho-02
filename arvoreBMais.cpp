#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstring> // Para usar strncpy
using namespace std;

// Definição do Registro
typedef struct Registro {
    int ID;
    char titulo[300];
    int ano;
    char autores[150];
    int citacoes;
    char atualizacao[20];
    char snippet[1024];
} Registro;

// No class
class No {
public:
    int t; // Ordem da árvore B+
    vector<int> chaves; // Chaves no nó
    vector<Registro> registros; // Registros nas folhas
    vector<No*> filhos; // Ponteiros para nós filhos
    bool folhas; // Booleano para verificar se o nó é uma folha
    int qtd_chaves_corrente; // Número atual de chaves
    No* proximo; // Ponteiro para o próximo nó

    // Construtor de No
    No(int _t, No* _proximo = NULL) {
        t = _t;
        proximo = _proximo;
        folhas = true;
        chaves.resize(2 * t - 1);
        registros.resize(2 * t - 1);
        filhos.resize(2 * t);
        qtd_chaves_corrente = 0;
    }

    // Função para inserir uma chave em um nó não cheio
    void insereEmNoNaoCheio(int k, Registro reg) {
        int i = qtd_chaves_corrente - 1;
        if (folhas) {
            while (i >= 0 && chaves[i] > k) {
                chaves[i + 1] = chaves[i];
                registros[i + 1] = registros[i];
                i--;
            }
            chaves[i + 1] = k;
            registros[i + 1] = reg;
            qtd_chaves_corrente++;
        } else {
            while (i >= 0 && chaves[i] > k)
                i--;
            i++;
            if (filhos[i]->qtd_chaves_corrente == 2 * t - 1) {
                divideFilho(i);
                if (chaves[i] < k)
                    i++;
            }
            filhos[i]->insereEmNoNaoCheio(k, reg);
        }
    }

    // Função para dividir o nó filho
    void divideFilho(int i) {
        No* y = filhos[i];
        No* z = new No(y->t, y->proximo);
        filhos.insert(filhos.begin() + i + 1, z);
        chaves.insert(chaves.begin() + i, y->chaves[t - 1]);
        registros.insert(registros.begin() + i, y->registros[t - 1]);
        y->proximo = z;
        z->folhas = y->folhas;
        z->qtd_chaves_corrente = t - 1;
        y->qtd_chaves_corrente = t - 1;
        for (int j = 0; j < t - 1; j++) {
            z->chaves[j] = y->chaves[j + t];
            z->registros[j] = y->registros[j + t];
        }
        if (!y->folhas) {
            for (int j = 0; j < t; j++)
                z->filhos[j] = y->filhos[j + t];
        }
        qtd_chaves_corrente++;
    }

    // Função para imprimir a árvore
    void mostraArvore() {
        for (int i = 0; i < qtd_chaves_corrente; i++) {
            if (!folhas)
                filhos[i]->mostraArvore();
            cout << chaves[i] << " - " << registros[i].ID << endl;
        }
        if (!folhas)
            filhos[qtd_chaves_corrente]->mostraArvore();
    }

    // Função para pesquisar uma chave na árvore
    No* busca(int k) {
        int i = 0;
        while (i < qtd_chaves_corrente && k > chaves[i])
            i++;
        if (chaves[i] == k)
            return this;
        if (folhas)
            return NULL;
        return filhos[i]->busca(k);
    }
};

// Classe ArvoreBMais
class ArvoreBMais {
public:
    No* raiz; // Raiz da árvore B+
    int m; // Ordem da árvore B+

    // Construtor da ArvoreBMais
    ArvoreBMais(int m) {
        raiz = new No(m);
        raiz->folhas = true;
        m = m;
    }

    // Função para inserir uma chave na árvore
    void insere(int k, Registro reg) {
        No* r = raiz;
        if (r->qtd_chaves_corrente == 2 * m - 1) {
            No* s = new No(m);
            raiz = s;
            s->filhos[0] = r;
            s->divideFilho(0);
            s->insereEmNoNaoCheio(k, reg);
        } else {
            r->insereEmNoNaoCheio(k, reg);
        }
    }

    // Função para imprimir a árvore
    void mostraArvore() {
        raiz->mostraArvore();
    }

    // Função para pesquisar uma chave na árvore
    No* busca(int k) {
        return (raiz == NULL) ? NULL : raiz->busca(k);
    }
};

// Função para imprimir a árvore
void mostraArvoreTree(ArvoreBMais* tree) {
    tree->mostraArvore();
}

// ===============================================================================
// espaço da árvore
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

int main() {
    int record_len = 3;
    ArvoreBMais* bplustree = new ArvoreBMais(record_len);
    Registro r1 = {1, "Titulo1", 2020, "Autor1", 10, "01/01/2020", "Snippet1"};
    Registro r2 = {2, "Titulo2", 2021, "Autor2", 20, "02/01/2021", "Snippet2"};
    Registro r3 = {3, "Titulo3", 2022, "Autor3", 30, "03/01/2022", "Snippet3"};
    Registro r4 = {4, "Titulo4", 2023, "Autor4", 40, "04/01/2023", "Snippet4"};
    Registro r5 = {5, "Titulo5", 2024, "Autor5", 50, "05/01/2024", "Snippet5"};

    bplustree->insere(5, r1);
    bplustree->insere(15, r2);
    bplustree->insere(25, r3);
    bplustree->insere(35, r4);
    bplustree->insere(45, r5);

    mostraArvoreTree(bplustree);

    if (bplustree->busca(5) != NULL) {
        cout << "Found" << endl;
    } else {
        cout << "Not found" << endl;
    }

    return 0;
}
