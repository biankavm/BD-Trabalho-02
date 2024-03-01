#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

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

// Função que encontra o tamanho dos blocos usando um comando do linux
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

struct Registro {
    int ID;
    char titulo[300];
    int ano;
    char autores[150];
    int citacoes;
    char atualizacao[20];
    char snippet[1024];
};

struct No {
    vector<int> chaves; // chaves ids
    vector<Registro*> registros; // vetor de registros das folhas
    vector<No*> filho; // ponteiro para os filhos (folhas só tem 1)
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
    // inserir na arvore b+
    void insere(int chave, Registro* record) {
        No* no = encontraNoParaInsercao(raiz, chave);
        insereNo(no, chave, record);
    }
    // busca na arvore b+ pra quando for procurar na função 
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
        // to-do? não precisa
    }*/

    void mostra() {
        mostraAux(raiz, 0);
    }

private:
    void insereNo(No* no, int chave, Registro* record) {
        auto it = lower_bound(no->chaves.begin(), no->chaves.end(), chave);
        int pos = it - no->chaves.begin();

        if (no->ehFolha()) {
            // se o no for uma folha, insere o registro associado a chave
            no->chaves.insert(it, chave);
            no->registros.insert(no->registros.begin() + pos, record);
        } else {
            // se o no não for uma folha, insere só a chave
            no->chaves.insert(it, chave);
        }

        if (no->chaves.size() > MAX_CHAVES) {
            divideNo(no);
        }
    }
    // dividir o no na hora de fazer o split
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

        // Se o nó dividido for uma folha, os registros devem ser reajustados
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
    void mostraAux(No* no, int linha) {
        if (no != nullptr) {
            cout << "Nível " << linha << ": ";
            for (int i = 0; i < no->chaves.size(); ++i)
                cout << no->chaves[i] << " ";
            cout << endl;
            for (int i = 0; i < no->filho.size(); ++i)
                mostraAux(no->filho[i], linha + 1);
        }
    }
};

int main() {
    ArvoreBMais tree;
    int tamanhoChave = sizeof(int);       // Tamanho da chave em bytes
    int tamanhoApontador = sizeof(int);   // Tamanho do apontador em bytes
    int tamanhoBloco = acha_tamanho_dos_blocos();    // Tamanho do bloco em bytes
    int m = calcularM(tamanhoChave, tamanhoApontador, tamanhoBloco); // calculo dos slides

    MIN_CHAVES = m;
    MAX_CHAVES = m*2;

    // registros exemplos
    Registro *r1 = new Registro{
    1, 
    "Poster: 3D sketching and flexible input for surface design: A case study.", 
    2013, 
    "Anamary Leal|Doug A. Bowman", 
    0, "2016-07-28 09:36:29", 
    "Poster: 3D sketching and flexible input for surface design: A case study. A Leal, DA Bowman -  Interfaces (3DUI), 2013 IEEE Symposium , 2013 - ieeexplore.ieee.org. ABSTRACT Designing three-dimensional (3D) surfaces is difficult in both the physical world  and in 3D modeling software, requiring background knowledge and skill. The goal of this  work is to make 3D surface design easier and more accessible through natural and  .."};

    Registro *r2 = new Registro{
        2,
        "Poster: Portable integral photography input/ output system using tablet PC and fly's eye lenses.",
        2013,
        "Yusuke Kawano|Kazuhisa Yanaka",
        0,
        "2016-07-28 09:36:49",
        "Poster: Portable integral photography input/output system using tablet PC and fly's eye lenses. Y Kawano, K Yanaka - 3D User Interfaces (3DUI), 2013 IEEE  , 2013 - ieeexplore.ieee.org. ABSTRACT We present a new system that can input and output integral photography  images. The system consists of a commerciallyavailable tablet PC to which fly's eye lenses,  one of which is for input and the other for output IP images, are attached. Light rays  .."
    };

    Registro *r3 = new Registro{
        3,
        "Poster: Real-time markerless kinect based finger tracking and hand gesture recognition for HCI.",
        2013,
        "Arun Kulshreshth|Christopher Zorn|Joseph J. LaViola Jr.",
        22,
        "2016-10-03 21:16:22",
        "Poster: Real-time markerless kinect based finger tracking and hand gesture recognition for HCI. A Kulshreshth, C Zorn, JJ LaViola - 3D User Interfaces (3DUI),  , 2013 - ieeexplore.ieee.org. ABSTRACT Hand gestures are intuitive ways to interact with a variety of user interfaces. We  developed a real-time finger tracking technique using the Microsoft Kinect as an input device  and compared its results with an existing technique that uses the K-curvature algorithm.  .."
    };

    Registro *r4 = new Registro{
        4,
        "Poster: Real time hand pose recognition with depth sensors for mixed reality interfaces.",
        2013,
        "Byungkyu Kang|Mathieu Rodrigue|Tobias H&ouml;llerer|Hwasup Lim",
        1,
        "2016-10-03 21:38:17",
        "Poster: Real time hand pose recognition with depth sensors for mixed reality interfaces. B Kang, M Rodrigue, T HÃ¶llerer - 3D User Interfaces (3DUI) , 2013 - ieeexplore.ieee.org. ABSTRACT We present a method for predicting articulated hand poses in realtime with a  single depth camera, such as the Kinect or Xtion Pro, for the purpose of interaction in a  Mixed Reality environment and for studying the effects of realistic and non-realistic  .."
    };

    Registro *r5 = new Registro{
        5,
        "Design and implementation of an immersive virtual reality system based on a smartphone platform.",
        2013,
        "Anthony Steed|Simon Julier",
        12,
        "2016-10-03 21:10:56",
        "Design and implementation of an immersive virtual reality system based on a smartphone platform. A Steed, S Julier - 3D User Interfaces (3DUI), 2013 IEEE  , 2013 - ieeexplore.ieee.org. ABSTRACT With the increasing power of mobile CPUs and GPUs, it is becoming tractable to  integrate all the components of an interactive, immersive virtual reality system onto a small  mobile device. We present a demonstration of a head-mounted display system integrated  .."
    };

    Registro *r6 = new Registro{
        6,
        "Poster: Lifted road map view on windshield display.",
        2013,
        "Takaya Kawamata|Itaru Kitahara|Yoshinari Kameda|Yuichi Ohta",
        1,
        "2016-10-03 21:34:50",
        "Poster: Lifted road map view on windshield display. T Kawamata, I Kitahara, Y Kameda - 3D User Interfaces ( , 2013 - ieeexplore.ieee.org. ABSTRACT A new road map visualization method at window shield display for vehicle driver  is proposed. Road structure ahead of a vehicle is shown by CG line segments at the upper  area of driver view in augmented reality fashion. Road line segments are virtually placed  .."
    };


    // Insira chaves com registros na árvore B+
    tree.insere(r1->ID, r1);
    tree.insere(r2->ID, r2);
    tree.insere(r3->ID, r3);
    tree.insere(r4->ID, r4);
    tree.insere(r5->ID, r5);
    tree.insere(r6->ID, r6);


    Registro *r = tree.busca(4);
    cout << "Registro encontrado: " << r->ID << endl << "Seu título: " << r->titulo << endl;
    // Mostra a árvore
    tree.mostra();

    return 0;
}