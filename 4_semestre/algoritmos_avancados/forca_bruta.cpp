#include <iostream>
#include <vector>
#include <string>
#include <map>

using namespace std;

struct Pessoa {
    bool tem_diabetes;
    int id_pai;
    int id_mae;
};

void resolver(){
    int n;
    cin >> n;

    // Mapa para armazenar a árvore genealógica
    map<int, Pessoa> arvore_genealogica;

    for (int i = 0; i < n; ++i) {
        int id, id_pai, id_mae;
        string diagnostico_str;
        cin >> id >> diagnostico_str >> id_pai >> id_mae;

        bool tem_diabetes = (diagnostico_str == "sim");
        arvore_genealogica[id] = {tem_diabetes, id_pai, id_mae};
    }

    int contador_hereditario = 0;

    // Verifica cada pessoa na árvore genealógica
    for (auto const& [id, pessoa] : arvore_genealogica) {
        if (pessoa.tem_diabetes) {
            bool pai_tem_diabetes = false;
            // Verifica se o pai está na árvore e se tem diabetes
            if (pessoa.id_pai != -1) {
                if (arvore_genealogica.count(pessoa.id_pai)) {
                    pai_tem_diabetes = arvore_genealogica.at(pessoa.id_pai).tem_diabetes;
                }
            }

            bool mae_tem_diabetes = false;
            if (pessoa.id_mae != -1) {
                if (arvore_genealogica.count(pessoa.id_mae)) {
                    mae_tem_diabetes = arvore_genealogica.at(pessoa.id_mae).tem_diabetes;
                }
            }

            // Incrementa o contador se pelo menos um dos pais tem diabetes
            if (pai_tem_diabetes || mae_tem_diabetes) {
                contador_hereditario++;
            }
        }
    }

    cout << contador_hereditario << endl;
}