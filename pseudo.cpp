#include <iostream>
#include <vector>
#include <algorithm>
#define ORDEM 5

using namespace std;

struct No {
    vector<int> chaves;
    vector<No> enderecos; // Agora armazena objetos No diretamente
    int tipo;  // 0 para galho, 1 para folha
};

void inserirChave(No& no, int chave) {
    auto it = std::upper_bound(no.chaves.begin(), no.chaves.end(), chave);
    no.chaves.insert(it, chave);
}

void insercao(No& raiz, int chave, int enderecoRegistro) {
    vector<No> S; // guarda pais
    No n = raiz;

    while (n.tipo == 0) {
        S.push_back(n); // guarda o papai
        int q = n.enderecos.size(); // q recebe o tamanho maximo 2m

        if (chave <= n.chaves[0]) {
            n = n.enderecos[0];
        } else if (chave > n.chaves[q - 1]) {
            n = n.enderecos[q - 1];
        } else {
            size_t i = 1;
            while (chave > n.chaves[i]) {
                ++i;
            }
            n = n.enderecos[i];
        }
    }

    auto it = find(n.chaves.begin(), n.chaves.end(), chave); // procura valor

    if (it != n.chaves.end()) {
        cout << "Registro já existe no arquivo, inserção não realizada\n";
    } else {
        pair<int, int> novaEntrada = {chave, enderecoRegistro}; // é a tupla (K, Pk)

        if (n.chaves.size() < ORDEM - 1) {
            auto pos = upper_bound(n.chaves.begin(), n.chaves.end(), chave); // achar a posição certa!
            size_t indice = pos - n.chaves.begin(); // subtrair do inicio, vai dar exatamente a posição que queremos
            inserirChave(n, chave);
            n.enderecos[indice].chaves.push_back(enderecoRegistro);
        } else {
            // sem espaço, hora do split
            No temp = n;
            auto pos = upper_bound(temp.chaves.begin(), temp.chaves.end(), chave);
            size_t indice = pos - temp.chaves.begin();
            temp.chaves.insert(pos, chave);
            temp.enderecos.insert(temp.enderecos.begin() + indice + 1, No());
            temp.enderecos[indice + 1].chaves.push_back(enderecoRegistro);

            No novoNoFolha;
            novoNoFolha.tipo = 1;

            n.chaves.assign(temp.chaves.begin(), temp.chaves.begin() + (ORDEM - 1) / 2);
            n.enderecos.assign(temp.enderecos.begin(), temp.enderecos.begin() + (ORDEM + 1) / 2);

            novoNoFolha.chaves.assign(temp.chaves.begin() + (ORDEM - 1) / 2, temp.chaves.end());
            novoNoFolha.enderecos.assign(temp.enderecos.begin() + (ORDEM + 1) / 2, temp.enderecos.end());

            novoNoFolha.enderecos.back() = temp.enderecos[(ORDEM + 1) / 2];

            n.enderecos.back() = novoNoFolha;

            bool terminado = false;
            while (!S.empty() && !terminado) { 
                No pai = S.back();
                S.pop_back();

                if (pai.chaves.size() < ORDEM - 1) {
                    auto posPai = upper_bound(pai.chaves.begin(), pai.chaves.end(), chave);
                    size_t indicePai = posPai - pai.chaves.begin();
                    inserirChave(pai, chave);
                    terminado = true;
                } else {
                    // não tem espaço!!! precisa dar o split
                    No tempPai = pai;
                    auto posPai = upper_bound(tempPai.chaves.begin(), tempPai.chaves.end(), chave);
                    size_t indicePai = posPai - tempPai.chaves.begin();
                    inserirChave(tempPai, chave);

                    No novoNoInterno;
                    novoNoInterno.tipo = 0; // não é folha

                    pai.chaves.assign(tempPai.chaves.begin(), tempPai.chaves.begin() + (ORDEM - 1) / 2);
                    pai.enderecos.assign(tempPai.enderecos.begin(), tempPai.enderecos.begin() + (ORDEM + 1) / 2);

                    novoNoInterno.chaves.assign(tempPai.chaves.begin() + (ORDEM - 1) / 2, tempPai.chaves.end());
                    novoNoInterno.enderecos.assign(tempPai.enderecos.begin() + (ORDEM + 1) / 2, tempPai.enderecos.end());

                    novoNoInterno.enderecos.back() = tempPai.enderecos[(ORDEM + 1) / 2];

                    pai.enderecos.back() = novoNoInterno;

                    novoNoFolha.enderecos[0] = tempPai.enderecos[(ORDEM + 1) / 2];

                    novaEntrada.first = tempPai.chaves[(ORDEM - 1) / 2];
                    novoNoFolha = n;
                }
            }

            if (S.empty()) { // se a pilha está vazia (não tem pai)
                No novaRaiz;
                novaRaiz.tipo = 0;
                inserirChave(novaRaiz, novaEntrada.first);
                raiz = novaRaiz;
            }
        }
    }
}

void imprimirArvore(const No& no) { // mostra árvore
    if (!no.chaves.empty()) {
        for (int i = 0; i < no.chaves.size(); ++i) {
            if (no.tipo == 1) { // é folha
                cout << no.chaves[i] << " ";
            } else {
                imprimirArvore(no.enderecos[i]);
                cout << no.chaves[i] << " ";
            }
        }
        imprimirArvore(no.enderecos.back());
    }
}

int main() {
    No raiz;
    raiz.tipo = 1;

    insercao(raiz, 10, 101);
    insercao(raiz, 5, 102);
    insercao(raiz, 15, 103);
    insercao(raiz, 3, 104);

    imprimirArvore(raiz);

    return 0;
}
