#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <limits>
#include <unordered_map>
#include <cstring> // Para as funções de manipulação de strings C
#include <vector>
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


unordered_map<int, Registro> MeuMapa; // declaração do Hash (a chave é o ID do registro)

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


// (obsoleto) parse para tirar as aspas do arquivo de dados
void tira_aspas(char sou_uma_string[]) {
    int len = strlen(sou_uma_string);
    if (len >= 2 && sou_uma_string[0] == '"' && sou_uma_string[len - 1] == '"') {
        sou_uma_string[len - 1] = '\0'; // Substitui a última aspa por terminador nulo
        for (int i = 0; i < len - 1; ++i) {
            sou_uma_string[i] = sou_uma_string[i + 1]; // Move os caracteres para a esquerda
        }
    }
}

// Função que faz a separação de uma linha do arquivo em campos de um Registro
Registro parse(string linha, ifstream& arquivo) {
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
            if (!((linha[i] == '.') && (linha[i+1] == '.') && (linha[i+2] == '"')) && (!((linha[i] == 'U') && (linha[i+1] == 'L') && (linha[i+2] == 'L'))) && (linha[i+2] != '"')) {
                string nova_linha;

                // Lê a próxima linha e a atribui como a nova linha do parsing
                getline(arquivo, nova_linha);
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
        cout << "Erro no artigo." << endl;
        return r;
    }

    // Cria e retorna um Registro com as informações destiladas
    /*int id = stoi(campos_completos[0]);
    string titulo = campos_completos[1];
    int ano = stoi(campos_completos[2]);
    string autores = campos_completos[3];
    int citacoes = stoi(campos_completos[4]);
    string atualizacao = campos_completos[5];
    string snippet = campos_completos[6];*/
    int id = stoi(campos_completos[0]);
    char titulo[300];
    strncpy(titulo, campos_completos[1].c_str(), sizeof(titulo));
    titulo[sizeof(titulo) - 1] = '\0';
    int ano = stoi(campos_completos[2]);
    char autores[150];
    strncpy(autores, campos_completos[3].c_str(), sizeof(autores));
    autores[sizeof(autores) - 1] = '\0';
    int citacoes = stoi(campos_completos[4]);
    char atualizacao[20];
    strncpy(atualizacao, campos_completos[5].c_str(), sizeof(atualizacao));
    atualizacao[sizeof(atualizacao) - 1] = '\0';
    char snippet[1024];
    strncpy(snippet, campos_completos[6].c_str(), sizeof(snippet));
    snippet[sizeof(snippet) - 1] = '\0';

    r = criaRegistro(id, titulo, ano, autores, citacoes, atualizacao, snippet);
    //cout << r.ID << r.titulo << r.ano << r.autores << r.citacoes << r.atualizacao << r.snippet << endl;
    return r;

}

// transformar arquivo de dados em um .dat
void monta_arquivo_de_dados(Registro R, string nome_arquivo_saida){
    ofstream outputFile(nome_arquivo_saida, ios::binary); // arquivo de saída
    outputFile.write(reinterpret_cast<char*>(&R), sizeof(R)); // escrever no arquivo de saída
}

// função para contar as linhas do arquivo para definirmos o tamanho do arquivo de dados
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

// leitura do arquivo csv
void le_arquivo_csv(string nome_arquivo_entrada, string nome_arquivo_saida){
    
    int cont = 0;
    ifstream arquivo;
    arquivo.open(nome_arquivo_entrada);

    if (!arquivo.is_open()) {
        cout << "Erro ao abrir arquivo!!!" << endl;
        return;
    }

    string linha;
    int num_campos = 0;
    Registro r;
    
    while (getline(arquivo, linha)) {
        cont++;
        r = parse(linha, arquivo);
        printf("id: %d\n", r.ID);
    }
    printf("\nNúmero de linhas: %d\n", cont);

    arquivo.close();
}

int main() {
    string nome_arquivo_entrada = "artinho.csv";
    string nome_arquivo_saida = "saida dat";
    cout << "Tamanho dos blocos: " << acha_tamanho_dos_blocos() << endl;

    // acha_tamanho_dos_blocos


    le_arquivo_csv(nome_arquivo_entrada, nome_arquivo_saida);
    return 0;
}