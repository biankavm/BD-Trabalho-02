#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <limits>
//#include "C:/Program Files/Boost/boost_1_84_0/boost/filesystem.hpp"

#include <boost/filesystem.hpp> // requer instalação da bib
using namespace std;

typedef struct Registro {
    int ID;
    string titulo;
    int ano;
    string autores;
    int citacoes;
    string atualizacao;
    string snippet;
} Registro;

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

void tira_aspas(string& sou_uma_string){
    if (sou_uma_string.front() == '"' && sou_uma_string.back() == '"') {
        sou_uma_string.erase(0, 1); // Remove a primeira aspa
        sou_uma_string.pop_back();  // Remove a última aspa
    }
}

void monta_arquivo_de_dados(Registro R, string nome_arquivo_saida){
    ofstream outputFile(nome_arquivo_saida, ios::binary); // arquivo de saída
    outputFile.write(reinterpret_cast<char *>(&R), sizeof(R)); // escrever no arquivo de saída
}

void le_arquivo_csv(string nome_arquivo_entrada, string nome_arquivo_saida) {
    ifstream arquivo(nome_arquivo_entrada);
    if (!arquivo.is_open()) {
        cout << "Erro ao abrir arquivo!!!" << endl;
        return;
    }

    string linha;
    while (getline(arquivo, linha)) {
        istringstream iss(linha);

        string ID_str, titulo, autores, atualizacao, snippet, ano_str, citacoes_str;
        int id, ano, citacoes;

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

        // convertendo os necessários para int
        id = stoi(ID_str);
        ano = stoi(ano_str);
        citacoes = stoi(citacoes_str);

        // imprimindo (aqui teremos que salvar em registro)
        Registro r = criaRegistro(id, titulo, ano, autores, citacoes, atualizacao, snippet);
        
        cout << "\nID: " << id << endl;
        cout << "\nTítulo: " << titulo << endl;
        cout << "\nAno: " << ano << endl;
        cout << "\nAutores: " << autores << endl;
        cout << "\nCitacoes: " << citacoes << endl;
        cout << "\nAtualizacao: " << atualizacao << endl;
        cout << "\nSnippet: " << snippet << endl;

        monta_arquivo_de_dados(r, nome_arquivo_saida);
}

    arquivo.close();
}

int main() {
    string nome_arquivo_entrada = "artinho.csv";
    string nome_arquivo_saida = "saida dat";
    le_arquivo_csv(nome_arquivo_entrada, nome_arquivo_saida);
    return 0;
}
