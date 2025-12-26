#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <sstream>

using namespace std;

struct Solicitacao {
    int id_cliente;
    int horario_retirada; 
    int horario_devolucao; 
    
    Solicitacao(int id, int retirada, int devolucao)
        : id_cliente(id), horario_retirada(retirada), horario_devolucao(devolucao) {}
};
//o criterio de comparacao primeiro pelo horario de devolucao e depois pelo horario de retirada, pois assim garantimos que em casos de empate no horario de devolucao, a solicitacao que comeca mais cedo seja priorizada
bool compararSolicitacoes(const Solicitacao& a, const Solicitacao& b) {
    if (a.horario_devolucao != b.horario_devolucao) {
        return a.horario_devolucao < b.horario_devolucao;
    }
    return a.horario_retirada < b.horario_retirada;
}

//converte horario no formato HH:MM para minutos desde meia-noite
int horarioParaMinutos(const string &horario_str) {
    //o .substr(i,j) retorna uma substring que comeca na posicao i e tem tamanho j
    return stoi(horario_str.substr(0, 2)) * 60 + stoi(horario_str.substr(3, 2));
}

void resolver() {
    int num_modelos, num_solicitacoes;
    cin >> num_modelos >> num_solicitacoes; 

    map<int, vector<Solicitacao>> solicitacoes_por_modelo;

    for (int i = 0; i < num_solicitacoes; ++i) {
        int id_cliente, id_modelo;
        string retirada_str, devolucao_str;
        cin >> id_cliente >> retirada_str >> devolucao_str >> id_modelo;

        solicitacoes_por_modelo[id_modelo].emplace_back(id_cliente,horarioParaMinutos(retirada_str),horarioParaMinutos(devolucao_str));
    }

    stringstream saida_ss;
    bool primeiro_modelo = true;

    
    for (int id_modelo = 1; id_modelo <= num_modelos; ++id_modelo) {
        if (!primeiro_modelo) {
            saida_ss << " | ";
        }
        primeiro_modelo = false;

        saida_ss << id_modelo << ": ";

        
        if (solicitacoes_por_modelo.find(id_modelo) == solicitacoes_por_modelo.end() || solicitacoes_por_modelo[id_modelo].empty()) {
            saida_ss << "0";
            continue;
        }

        vector<Solicitacao>& solicitacoes = solicitacoes_por_modelo[id_modelo];

        
        stable_sort(solicitacoes.begin(), solicitacoes.end(), compararSolicitacoes);

        vector<int> clientes_aceitos;
        int ultimo_horario_devolucao = -1;

        
        for (const auto& sol : solicitacoes) {
            if (sol.horario_retirada >= ultimo_horario_devolucao) {
                clientes_aceitos.push_back(sol.id_cliente);
                ultimo_horario_devolucao = sol.horario_devolucao;
            }
        }

        saida_ss << clientes_aceitos.size() << " = ";
        for (size_t i = 0; i < clientes_aceitos.size(); ++i) {
            saida_ss << clientes_aceitos[i] << (i == clientes_aceitos.size() - 1 ? "" : ", ");
        }
    }
    cout << saida_ss.str() << endl;
}

int main() {
    
    int num_casos_teste;
    cin >> num_casos_teste;
    for (int caso = 0; caso < num_casos_teste; ++caso) {
        resolver();
    }

    return 0;
}