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
#define ORDEM 5

using namespace std;


// Funções de structs

typedef struct Nost {
    int* chaves; // 0 para galho, 1 para folha
    int* enderecos; // Agora armazena objetos No diretamente
}No;

/*
No *buffer; // um vetor de nós salvos em memória principal
int *endBuff; //um vetor que possui os endereços dos nós do buffer 
*/

No inicializaNo(int tamBloco){
    int numIntVect = tamBloco / (sizeof(int) * 2); // calcula o tamanho dos vetores de inteiros dentro dos nós
    No no;

    // Aloca a memória para os vetores chaves e enderecos
    no.chaves = (int*)malloc(sizeof(int) * numIntVect);
    no.enderecos = (int*)malloc(sizeof(int) * numIntVect);

    // Verifica se a alocação foi bem-sucedida
    if (no.chaves == NULL || no.enderecos == NULL) {
        // Trate o erro de alocação de memória aqui, se necessário
        printf("Erro na alocação de memória\n");
        exit(EXIT_FAILURE);
    }

    return no; // retorna o nó
}


// Funções util

int converter_chars_para_int(char* cs) {
    int retorno = 0;
    int num_char = sizeof(int);

    for (int i = 0; i < num_char; i++) {
        retorno |= ((unsigned char)cs[i] << (8*i));
    }
    
    return retorno;
}


void converter_int_para_chars(int x, char* as) {
    int size_int = sizeof(int);
    int int_bits = (size_int*8);

    // Convert the integer to a sequence of chars
    //char* as = (char*) malloc(sizeof(int));
    //char as[sizeof(int)];
    
    // Use bitwise operations to extract the bytes of the integer
    for (int i = 0; i < size_int; i++) {
        as[i] = (x >> (8*i)) & 0xFF; //(8*(size_int-i-1))
    }
    
}



// Funções de arquivo

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


// Calcula a quantidade de chaves que cabem em um bloco
int acha_quantidade_chaves(int tam_bloco){
    return tam_bloco / (2*sizeof(int));
}


// Faz a leitura de um bloco de dado endereço no arquivo
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


// Escreve um bloco em um dado endereço em um arquivo
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

    return 0;
}


// Escreve um nó completo em um dado bloco
int escrever_registro_no_bloco_index(No* no, char* bloco, int tam_bloco) {
    // Calculando tamanhos dos campos dos registros
    int size = sizeof(int);
    int num_chaves = tam_bloco/(2*size);
    char* id_str;
    
    for (int reg = 0; reg < num_chaves; reg++) {
        // Converte o int em um vetor de chars de valor ascii
        converter_int_para_chars(no->chaves[reg], id_str);
        
        // Adiciona o vetor no bloco
        for (int i = 0; i < size; i++) {
            bloco[(reg * size)+i] = id_str[i];
        }
    }
    
    for (int reg = 0; reg < num_chaves; reg++) {
        // Converte o int em um vetor de chars de valor ascii
        converter_int_para_chars(no->enderecos[reg], id_str);

        // Adiciona o vetor no bloco
        for (int i = 0; i < size; i++) {
            bloco[((reg + num_chaves) * size)+i] = id_str[i];
        }        
    }

    return 0;
}


// Função de arquivo equivalente a uma operação "no->filho[posicao]"" em uma árvore
No* carregaNo(No* no, int endereco, int tamanhoBloco, string arquivo_entrada) {
    // Acessa o bloco
    char* bloco = ler_bloco_do_arquivo(endereco, arquivo_entrada, tamanhoBloco);

    // Lê o nó do bloco
    ler_registro_do_bloco_index(no, bloco, tamanhoBloco);

    return no;
}


int escreve_teste_input(int* bloco, int endereco, string nome_arquivo_saida, int tam_bloco) {
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
    int num_escritos = fwrite(bloco, sizeof(int), tam_bloco, arquivo);
    
    if (num_escritos != tam_bloco) {
        printf("Erro ao escrever no arquivo.\n");
        fclose(arquivo);
        return 1;
    }

    return 0;
}

void gera_entrada_teste(int tam_bloco) {
    string filename = "in_arvore_teste.bin";
    int num_chaves = tam_bloco / (sizeof(int) * 2);
    int* bloco = (int*) malloc(sizeof(int) * num_chaves * 2);

    for (int i = 0; i < num_chaves-1; i++) { 
        bloco[i] = i;
    }

    // setando como nó interno
    bloco[num_chaves-1] = 0;

    for (int i = 0; i < num_chaves; i++) { 
        bloco[i+num_chaves] = i*3;
    }

    escreve_teste_input(bloco, 0, filename, num_chaves*2);
}


void teste_acesso_aos_dados(int endereco) {
    string arquivo_entrada = "in_arvore_teste.bin";
    string arquivo_saida = "out_arvore_teste.bin";
    int tamanhoBloco = acha_tamanho_dos_blocos();
    int num_chaves = acha_quantidade_chaves(tamanhoBloco);
    printf("Tamanho do bloco: %d\nNúmero de chaves em um bloco: %d\n", tamanhoBloco, num_chaves);

    gera_entrada_teste(tamanhoBloco);
    printf("Arquivo de teste gerado.\n");

    char* bloco = ler_bloco_do_arquivo(endereco, arquivo_entrada, tamanhoBloco);
    printf("O bloco no endereço %d foi lido.\n", endereco);

    
    No no = inicializaNo(tamanhoBloco);
    ler_registro_do_bloco_index(&no, bloco, tamanhoBloco);
    printf("O bloco foi convertido em um nó.\n");
    printf("Primeira chave/endereço: (%d, %d)\n", no.chaves[0], no.enderecos[0]);
    
    no.chaves[0] += 42;
    no.enderecos[0] += 42 * 3;
    printf("Primeira chave/endereço modificada: (%d, %d)\n", no.chaves[0], no.enderecos[0]);
    
    escrever_registro_no_bloco_index(&no, bloco, tamanhoBloco);
    printf("Nó reescrito no bloco.\n");
    
    
    escrever_bloco_no_arquivo(bloco, endereco, arquivo_entrada, tamanhoBloco);
    printf("Bloco reescrito no arquivo.\n");

    printf("Checando se os dados foram modificados:\n");
    bloco = ler_bloco_do_arquivo(endereco, arquivo_entrada, tamanhoBloco);

    
    No no2 = inicializaNo(tamanhoBloco);
    ler_registro_do_bloco_index(&no2, bloco, tamanhoBloco);
    printf("O bloco foi convertido em um nó.\n");
    printf("Primeira chave/endereço: (%d, %d)\n", no2.chaves[0], no2.enderecos[0]);
    
}

int main() {
    
    teste_acesso_aos_dados(0);

    return 0;
}
