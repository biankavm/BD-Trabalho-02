#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>

using namespace std;

// Declaration of the structure
typedef struct Registro {
    int ID;
    string titulo;
    int ano;
    string autores;
    int citacoes;
    string atualizacao;
    string snippet;
} Registro;

unordered_map<int, Registro> MeuMapa; // Declaration of the Hash Map (the key is the ID of the record)

// Function to create a record (constructor)
Registro criaRegistro(int id, string titulo, int ano, string autores, int citacoes,
                       string atualizacao, string snippet) {
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

// Function to remove quotes from a string
void tira_aspas(string& sou_uma_string) {
    if (sou_uma_string.front() == '"' && sou_uma_string.back() == '"') {
        sou_uma_string.erase(0, 1); // Remove the first quote
        sou_uma_string.pop_back();  // Remove the last quote
    }
}

// Function to write data to a binary file
void monta_arquivo_de_dados(const Registro& R, const string& nome_arquivo_saida) {
    ofstream outputFile(nome_arquivo_saida, ios::binary | ios::app); // Truncate the file
    if (!outputFile.is_open()) {
        cerr << "Error opening output file: " << nome_arquivo_saida << endl;
        return;
    }

    // Write each field of the record to the binary file
    outputFile.write(reinterpret_cast<const char*>(&R.ID), sizeof(R.ID));
    int tituloSize = R.titulo.size();
    outputFile.write(reinterpret_cast<const char*>(&tituloSize), sizeof(tituloSize));
    outputFile.write(R.titulo.c_str(), tituloSize);
    outputFile.write(reinterpret_cast<const char*>(&R.ano), sizeof(R.ano));
    int autoresSize = R.autores.size();
    outputFile.write(reinterpret_cast<const char*>(&autoresSize), sizeof(autoresSize));
    outputFile.write(R.autores.c_str(), autoresSize);
    outputFile.write(reinterpret_cast<const char*>(&R.citacoes), sizeof(R.citacoes));
    int atualizacaoSize = R.atualizacao.size();
    outputFile.write(reinterpret_cast<const char*>(&atualizacaoSize), sizeof(atualizacaoSize));
    outputFile.write(R.atualizacao.c_str(), atualizacaoSize);
    int snippetSize = R.snippet.size();
    outputFile.write(reinterpret_cast<const char*>(&snippetSize), sizeof(snippetSize));
    outputFile.write(R.snippet.c_str(), snippetSize);

    outputFile.close();
}

// Function to count lines in a file
int conta_linhas_arquivo(string nome_arquivo) {
    ifstream arquivo(nome_arquivo);
    if (!arquivo.is_open()) {
        cerr << "Error opening file: " << nome_arquivo << endl;
        return -1;
    }

    int cont = 0;
    string linha;
    while (getline(arquivo, linha)) {
        cont++;
    }
    arquivo.close();
    return cont;
}

// Function to read a CSV file
void le_arquivo_csv(string nome_arquivo_entrada, string nome_arquivo_saida) {
    int cont = 0;
    ifstream arquivo(nome_arquivo_entrada);
    if (!arquivo.is_open()) {
        cout << "Error opening file!!!" << endl;
        return;
    }

    string linha;
    while (getline(arquivo, linha)) {
        cont++;
        istringstream iss(linha);
        string ID_str, titulo, autores, atualizacao, snippet, ano_str, citacoes_str;
        int id, ano, citacoes;

        // Read the lines
        getline(iss, ID_str, ';');
        getline(iss, titulo, ';');
        getline(iss, ano_str, ';');
        getline(iss, autores, ';');
        getline(iss, citacoes_str, ';');
        getline(iss, atualizacao, ';');
        getline(iss, snippet, ';');

        // Remove quotes
        tira_aspas(ID_str);
        tira_aspas(titulo);
        tira_aspas(ano_str);
        tira_aspas(autores);
        tira_aspas(citacoes_str);
        tira_aspas(atualizacao);
        tira_aspas(snippet);

        // Convert the necessary ones to int
        id = stoi(ID_str);
        ano = stoi(ano_str);
        citacoes = stoi(citacoes_str);

        // Create a record
        Registro r = criaRegistro(id, titulo, ano, autores, citacoes, atualizacao, snippet);

        // Add the record to the map
        MeuMapa[id] = r;

        // Write to the binary file
        monta_arquivo_de_dados(r, nome_arquivo_saida);
    }

    arquivo.close();
}

// Function to read a binary data file
void le_arquivo_de_dados(const string& nome_arquivo_de_dados) {
    ifstream arquivo(nome_arquivo_de_dados, ios::binary); // Open the file in binary mode
    if (!arquivo.is_open()) {
        cerr << "Error opening input file: " << nome_arquivo_de_dados << endl;
        return;
    }

    Registro r;

    // While there are records to read
    while (arquivo.read(reinterpret_cast<char*>(&r.ID), sizeof(r.ID))) {
        // Process the read record
        int tituloSize;
        arquivo.read(reinterpret_cast<char*>(&tituloSize), sizeof(tituloSize));
        r.titulo.resize(tituloSize);
        arquivo.read(&r.titulo[0], tituloSize);
        arquivo.read(reinterpret_cast<char*>(&r.ano), sizeof(r.ano));
        int autoresSize;
        arquivo.read(reinterpret_cast<char*>(&autoresSize), sizeof(autoresSize));
        r.autores.resize(autoresSize);
        arquivo.read(&r.autores[0], autoresSize);
        arquivo.read(reinterpret_cast<char*>(&r.citacoes), sizeof(r.citacoes));
        int atualizacaoSize;
        arquivo.read(reinterpret_cast<char*>(&atualizacaoSize), sizeof(atualizacaoSize));
        r.atualizacao.resize(atualizacaoSize);
        arquivo.read(&r.atualizacao[0], atualizacaoSize);
        int snippetSize;
        arquivo.read(reinterpret_cast<char*>(&snippetSize), sizeof(snippetSize));
        r.snippet.resize(snippetSize);
        arquivo.read(&r.snippet[0], snippetSize);

        // Process the record read
        cout << "ID: " << r.ID << endl;
        // cout << "Título: " << r.titulo << endl;
        // cout << "Ano: " << r.ano << endl;
        // cout << "Autores: " << r.autores << endl;
        // cout << "Citacoes: " << r.citacoes << endl;
        // cout << "Atualizacao: " << r.atualizacao << endl;
        // cout << "Snippet: " << r.snippet << endl;
    }

    arquivo.close();
}

int main() {
    string nome_arquivo_entrada = "artinho.csv";
    string nome_arquivo_saida = "saida.dat";
    
    // Read the CSV file and generate the binary data file
    le_arquivo_csv(nome_arquivo_entrada, nome_arquivo_saida);

    // Display the records from the binary data file
    le_arquivo_de_dados(nome_arquivo_saida);

    return 0;
}
