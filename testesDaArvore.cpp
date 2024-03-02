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

    // Converte a string para um número inteiro e retorna
    return stoi(result);
}

// =============================================================================================
// Coisas da árvore B+
int MAX_CHAVES;
int MIN_CHAVES;

// Função responsável por calcular o m, que é usado para encontrar o máximo e
// mínimo de chaves na árvore b+
int calcularM(int tamanhoChave, int tamanhoApontador, int tamanhoBloco) {
    int m = 0;
    int bytesPorChave = tamanhoChave;
    int bytesPorApontador = tamanhoApontador;
    int bytesPorBloco = tamanhoBloco;
    // garante que a soma dos bytes de chaves e apontadores não ultrapasse o tamanho do bloco
    while ((2 * m * bytesPorChave + (2 * m + 1) * bytesPorApontador) <= bytesPorBloco) {
        m++;
    }

    return m;
}

struct No {
    vector<int> chaves;
    vector<Registro*> registros;
    vector<No*> filho;
    No* pai;

    No() {
        pai = nullptr;
    }

    bool ehFolha() {
        return filho.empty();
    }
};

class ArvoreBMais {
private:
    No* raiz;

public:
    ArvoreBMais() {
        raiz = new No();
    }

    void insere(int chave, Registro* record) {
        No* no = encontraNoParaInsercao(raiz, chave);
        insereNo(no, chave, record);
    }

    Registro* busca(int chave) {
        No* no = encontraNoParaBusca(raiz, chave);
        if (no != nullptr) {
            int pos = lower_bound(no->chaves.begin(), no->chaves.end(), chave) - no->chaves.begin();
            if (pos < no->chaves.size() && no->chaves[pos] == chave)
                return no->registros[pos];
        }
        return nullptr;
    }

   /* void remove(int chave) {
        // to-do?
    }*/

    void mostra() {
        unsigned int cont = 0;
        printf("Vou mostrar a arvore");
        mostraAux(raiz, 0, cont);
        cout << "Contei " << cont << " vezes." << endl;
    }

private:
    void insereNo(No* no, int chave, Registro* record) {
        auto it = lower_bound(no->chaves.begin(), no->chaves.end(), chave);
        int pos = it - no->chaves.begin();

        if (no->ehFolha()) {
            // Alocar memória dinamicamente para o objeto Registro
            Registro* newRecord = new Registro(*record);

            // Se o nó for uma folha, insira o registro associado à chave
            no->chaves.insert(it, chave);
            no->registros.insert(no->registros.begin() + pos, newRecord);
        } else {
            // Se o nó não for uma folha, apenas insira a chave
            no->chaves.insert(it, chave);
        }

        if (no->chaves.size() > MAX_CHAVES) {
            divideNo(no);
        }
    }


    void divideNo(No* no) {
        No* novoNo = new No();
        int meio = no->chaves.size() / 2;

        novoNo->chaves.assign(no->chaves.begin() + meio, no->chaves.end());

        if (!no->ehFolha()) {
            novoNo->filho.assign(no->filho.begin() + meio, no->filho.end());
            no->filho.resize(meio);
        }

        no->chaves.resize(meio);

        if (no->pai == nullptr) {
            No* novaRaiz = new No();
            novaRaiz->chaves.push_back(novoNo->chaves[0]);
            novaRaiz->filho.push_back(no);
            novaRaiz->filho.push_back(novoNo);

            no->pai = novaRaiz;
            novoNo->pai = novaRaiz;

            raiz = novaRaiz;
        } else {
            No* pai = no->pai;
            insereNo(pai, novoNo->chaves[0], nullptr); // Insere a nova chave no pai (sem registro associado)
            pai->filho.push_back(novoNo);
        }

        // se o nó dividido for uma folha, os registros devem ser reajustados
        if (no->ehFolha()) {
            novoNo->registros.assign(no->registros.begin() + meio, no->registros.end());
            no->registros.resize(meio);
        }
    }
    // com base na chave procura em qual posição da árvore deve inserir
    No* encontraNoParaInsercao(No* no, int chave) {
        if (no->ehFolha()) {
            return no;
        } else {
            auto it = lower_bound(no->chaves.begin(), no->chaves.end(), chave); // busca binaria
            int pos = it - no->chaves.begin();
            return encontraNoParaInsercao(no->filho[pos], chave);
        }
    }

    // procura o nó com base na chave passada
    No* encontraNoParaBusca(No* no, int chave) {
        if (no == nullptr)
            return nullptr;

        auto it = lower_bound(no->chaves.begin(), no->chaves.end(), chave);
        int pos = it - no->chaves.begin();
        
        if (pos < no->chaves.size() && no->chaves[pos] == chave)
            return no;
        
        if (no->ehFolha())
            return nullptr;
        
        return encontraNoParaBusca(no->filho[pos], chave);
    }
    // mostra os níveis da árvore b+
    void mostraAux(No* no, int linha, unsigned int &cont) {
    if (no != nullptr) {
        cout << "Nível " << linha << ": ";
        // Adiciona o número de registros no nó ao contador
        cont += no->registros.size();
        for (int i = 0; i < no->chaves.size(); ++i)
            cout << no->chaves[i] << " ";
        cout << " (Total Keys: " << no->chaves.size() << ")" << endl;

        cout << endl;
        for (int i = 0; i < no->filho.size(); ++i)
            mostraAux(no->filho[i], linha + 1, cont);
    }
}};
    


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
        id_atual = stoi(id_str);

        if (id_atual == id) {
            printf("Achou\n");
            r.ID = id_atual;

            memcpy(r.titulo, bloco + sizes[0], sizes[1]);

            memcpy(id_str, bloco + sizes[1], sizes[2]);
            r.ano = stoi(id_str);
            
            memcpy(r.autores, bloco + sizes[2], sizes[3]);

            memcpy(id_str, bloco + sizes[3], sizes[4]);
            r.citacoes = stoi(id_str);

            memcpy(r.atualizacao, bloco + sizes[4], sizes[5]);
            memcpy(r.snippet, bloco + sizes[5], sizes[6]);

            
            return r;
        }
        pos += tam_reg;
    }

    return r;
}



// TO-DO
int escrever_bloco_no_arquivo() {
    return 0;
}


// TO-DO
int escrever_registro_no_bloco_dados() {
    return 0;
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
int num_blocos(int tam_bloco, int tam_reg, int num_artigos) {
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
    string line(linha);

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
            //printf("%ld: %d %d %d - %c %c %c\n", linha.size(), nao_eh_fim_do_snippet, nao_eh_snippet_null, nao_eh_snippet_estranho, linha[i], linha[i+1], linha[i+2]);

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

// Temporária, para facilitar testes com e sem alocação de dados
void parte_de_dados(string nome_arquivo_entrada, string nome_arquivo_saida) {
    printf("Descobrindo tamanho do bloco de dados...\n");
    int tam_bloco = acha_tamanho_dos_blocos();

    printf("Descobrindo tamanhos dos registros...\n");
    int tam_reg_hash = tam_registro_hash();
    // TO-DO: descobrir registros dos índices

    printf("Descobrindo o número de linhas do arquivo...\n");
    int num_linhas = conta_linhas_arquivo(nome_arquivo_entrada);

    printf("Descobrindo o número de blocos para armazenar o arquivo de dados...\n");
    int num_blocos_hash = num_blocos(tam_bloco, tam_reg_hash, num_linhas);

    printf("tamanho do bloco: %d\n", tam_bloco);
    printf("tamanho do registro (hash): %d\n", tam_reg_hash);
    printf("número de linhas: %d\n", num_linhas);
    printf("numero de blocos (hash): %d\n\n", num_blocos_hash);

    printf("Alocando %d bytes de memória para o arquivo...\n", num_blocos_hash * tam_bloco);
    //aloca_memoria_arquivo(nome_arquivo_saida, num_blocos_hash, tam_bloco);
    aloca_memoria_arquivo(nome_arquivo_saida, num_blocos_hash, tam_bloco);

    // TO-DO: escrever um registro
    
    char* bloco_teste = ler_bloco_do_arquivo(tam_bloco * 3, nome_arquivo_saida, tam_bloco, num_blocos_hash);

    printf("Bloco de endereço 3 * %d lido (tamanho: %d bytes):\n", tam_bloco, tam_bloco);
    for (int i = 0; i < tam_bloco; i++) {
        printf("%d", bloco_teste[i]);
    }
    printf("\n\n");

    // TO-DO: ler um registro

}



// leitura do arquivo csv
void le_arquivo_csv(string nome_arquivo_entrada, string nome_arquivo_saida, ArvoreBMais *arvore){
    
    int cont = 0;
    FILE *arquivo = fopen(nome_arquivo_entrada.c_str(), "r");
    if (!arquivo) {
        cout << "Erro ao abrir arquivo!!!" << endl;
        return;
    }

    string linha;
    int num_campos = 0;
    Registro r;
    
    parte_de_dados(nome_arquivo_entrada, nome_arquivo_saida);

    printf("Parsing do arquivo de entrada:\n");
    linha = le_linha(arquivo);
    while (linha.size() > 0) {
        cont++;
        r = parse(linha, arquivo);
        printf("id: %d\t", r.ID);
        arvore->insere(r.ID, &r); // Insere o ID na árvore B+
        linha = le_linha(arquivo);
    }
    printf("\nNúmero de artigos: %d\n", cont);
    
    fclose(arquivo);
}

int main() {
    string nome_arquivo_entrada = "artino.csv";
    string nome_arquivo_saida = "saida dat";
    ArvoreBMais arvore = ArvoreBMais();
    /*int tamanhoChave = sizeof(int);       // Tamanho da chave em bytes
    int tamanhoApontador = sizeof(int);   // Tamanho do apontador em bytes
    int tamanhoBloco = acha_tamanho_dos_blocos();    // Tamanho do bloco em bytes
    int m = calcularM(tamanhoChave, tamanhoApontador, tamanhoBloco); // calculo dos slides*/

    MIN_CHAVES = MAX_CHAVES/2;
    MAX_CHAVES = 25000;

    le_arquivo_csv(nome_arquivo_entrada, nome_arquivo_saida, &arvore);
    
    arvore.mostra();
    Registro *r = arvore.busca(35278);
    cout << "Achei o registro de id: " << r->ID << endl;
    cout << "Seu titulo eh: " << r->titulo << endl;
    cout << "Seus autores sao: " << r->autores << endl;

    return 0;
}