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

using namespace std;


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
        retorno |= ((unsigned char)cs[i] << (8*i)); //(num_char - i - 1)
    }
    
    return retorno;
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


void buscaArquivoDeDados(int id){
    printf("Buscando artigo de id = %d...\n\n", id);
    int tam_bloco = acha_tamanho_dos_blocos();
    string nome_arquivo_dados = "arquivo_dados.bin";
    int* metadados = ler_metadados(tam_bloco, nome_arquivo_dados);
    
    int num_blocos = metadados[1];
    int num_linhas = metadados[2];
    int tam_reg_hash = metadados[3];
    free(metadados);

    Registro buscado;
    buscado.ID = id;
    
    long endereco = funcao_hash(&buscado, num_blocos, tam_bloco);
    if ((endereco > (long) (num_blocos-1) * tam_bloco) || (id < 1)) {
        printf("Artigo não encontrado.\n");
        return;
    }

    char* bloco = ler_bloco_do_arquivo(endereco, nome_arquivo_dados, tam_bloco);
    Registro* r = (Registro*) malloc(sizeof(Registro));
    
    ler_registro_do_bloco_dados(r, bloco, id, tam_bloco, tam_reg_hash);
    if (r->ID == -1) {
        printf("Artigo não encontrado.\n");
        return;
    }

    printf("Artigo encontrado:\n");
    mostraRegistro(r);

    // NOTA: Em virtude da natureza do hash temporário, só é necessário ler um bloco para encontrar o registro.
    printf("\nNúmero de blocos lidos para encontrá-lo: %d\n", 1);
    printf("Número total de blocos no arquivo de dados: %d\n", num_blocos);
    free(bloco);
    free(r);
}

int main(int argc, char** argv){
    int id = stoi(argv[1]);
    buscaArquivoDeDados(id);
}