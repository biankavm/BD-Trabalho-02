#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <limits>
#include <unordered_map>
//#include "C:/Program Files/Boost/boost_1_84_0/boost/filesystem.hpp"

#include <boost/filesystem.hpp>
using namespace std;

// declaração do registro
typedef struct Registro {
    int ID;
    string titulo;
    int ano;
    string autores;
    int citacoes;
    string atualizacao;
    string snippet;
} Registro;


unordered_map<int, Registro> MeuMapa; // declaração do Hash (a chave é o ID do registro)

// função que cria um registro (construtor)
Registro criaRegistro(int id, string titulo, int ano, string autores, int citacoes,
                        string atualizacao, string snippet){
    Registro R;
    R.ID = id;
    R.titulo = titulo;
    R.ano = ano;
    R.autores = autores;
    R.citacoes = citacoes;
    R.atualizacao = atualizacao;
    R.snippet = snippet;

    return R;
}

// parse para tirar as aspas do arquivo de dados
void tira_aspas(string& sou_uma_string){
    if (sou_uma_string.front() == '"' && sou_uma_string.back() == '"') {
        sou_uma_string.erase(0, 1); // Remove a primeira aspa
        sou_uma_string.pop_back();  // Remove a última aspa
    }
}

void parse(string linha){
    // verifica primeiro campo da linha e define a regra do id
    char atual = 'a';

    for (int i = 0; i < linha.size(); i++){
        atual = linha[i];
        //cout << atual;
        /*if(i == 0 and linha[i] != '"'){

        }*/
    }
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

// leitura do arquivo csv
void le_arquivo_csv(string nome_arquivo_entrada, string nome_arquivo_saida){
    int cont = 0;
    ifstream arquivo(nome_arquivo_entrada);
    if (!arquivo.is_open()) {
        cout << "Erro ao abrir arquivo!!!" << endl;
        return;
    }

    string linha;
    while (getline(arquivo, linha)) {
        cont++;
        istringstream iss(linha);
        parse(linha);
        /*string ID_str, titulo, autores, atualizacao, snippet, ano_str, citacoes_str;
        int id, ano, citacoes;

        // lê as linhas
        cout << cont << endl; 
        getline(iss, ID_str, ';');
        getline(iss, titulo, ';');
        getline(iss, ano_str, ';');
        getline(iss, autores, ';');
        getline(iss, citacoes_str, ';');
        getline(iss, atualizacao, ';');
        getline(iss, snippet, ';');     
        // tirar as aspas
        tira_aspas(ID_str);
        tira_aspas(titulo);
        tira_aspas(ano_str);
        tira_aspas(autores);
        tira_aspas(citacoes_str);
        tira_aspas(atualizacao);
        tira_aspas(snippet);
        printf("%s %s %s\n", ID_str.c_str(), ano_str.c_str(), citacoes_str.c_str());

        // convertendo os necessários para int
        id = stoi(ID_str);
        ano = stoi(ano_str);
        citacoes = stoi(citacoes_str);

        // imprimindo (aqui teremos que salvar em registro)
        Registro r = criaRegistro(id, titulo, ano, autores, citacoes, atualizacao, snippet);
        MeuMapa[id] = r;
        cout << "\nID: " << id << endl;
        cout << "\nTítulo: " << titulo << endl;
        cout << "\nAno: " << ano << endl;
        cout << "\nAutores: " << autores << endl;
        cout << "\nCitacoes: " << citacoes << endl;
        cout << "\nAtualizacao: " << atualizacao << endl;
        cout << "\nSnippet: " << snippet << endl;

        monta_arquivo_de_dados(r, nome_arquivo_saida);*/
}

    arquivo.close();
}

int main() {
    string nome_arquivo_entrada = "artinho.csv";
    string nome_arquivo_saida = "saida dat";
    le_arquivo_csv(nome_arquivo_entrada, nome_arquivo_saida);
    //int conta = conta_linhas_arquivo(nome_arquivo_entrada);
    //cout << conta;
    return 0;
}
