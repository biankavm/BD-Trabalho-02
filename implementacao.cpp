#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <limits>
using namespace std;

typedef struct Registro {
    char ID[1024];
    char titulo[300];
    int ano;
    char autores[150];
    int citacoes;
    char atualizacao[20];
    char snippet[1024];
} Registro;

void tira_aspas(string& sou_uma_string){
    if (sou_uma_string.front() == '"' && sou_uma_string.back() == '"') {
        sou_uma_string.erase(0, 1); // Remove a primeira aspa
        sou_uma_string.pop_back();  // Remove a última aspa
    }
}


void le_arquivo(string nome_arquivo) {
    ifstream arquivo(nome_arquivo);
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
        cout << "\nID: " << id << endl;
        cout << "\nTítulo: " << titulo << endl;
        cout << "\nAno: " << ano << endl;
        cout << "\nAutores: " << autores << endl;
        cout << "\nCitacoes: " << citacoes << endl;
        cout << "\nAtualizacao: " << atualizacao << endl;
        cout << "\nSnippet: " << snippet << endl;
}

    arquivo.close();
}

int main() {
    string nome_arquivo = "artinho.csv";
    le_arquivo(nome_arquivo);
    return 0;
}
