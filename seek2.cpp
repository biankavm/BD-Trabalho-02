#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
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

void mostraRegistro(Registro r){
    cout << "ID do registro: " << r.ID << endl;
    cout << "Titulo do registro: " << r.titulo << endl;
    cout << "Ano: " << r.ano << endl;
    cout << "Autores: " << r.autores << endl;
    cout << "Citacoes: " << r.citacoes << endl;
    cout << "Atualizacao: " << r.atualizacao << endl;
    cout << "Snippet: " << r.snippet << endl;
}

void buscaArquivoDeIndiceSec(char titulo[]){
    // a fazer

}

int main(int argc, char** argv){
    char titulo[300];
    strncpy(titulo, argv[1], sizeof(titulo)); // Copia o argumento para a variável titulo
    buscaArquivoDeIndiceSec(titulo);
}