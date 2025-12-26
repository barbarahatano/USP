#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct ResultadoTrecho {
    int indice;                    // Índice do trecho (0, 1, 2, ...)
    long long ultrapassagens;      // Total de ultrapassagens neste trecho
};

struct Jogador {
    int s0;  // Posição inicial do jogador
    int sf;  // Posição final do jogador
};

bool compararPosicao(const Jogador& a, const Jogador& b) {
    if (a.s0 != b.s0) {
        return a.s0 < b.s0;
    }
    return a.sf < b.sf;  // Ordena pela posição final (crescente) se s0 for igual
}

long long mergeAndCount(vector<int>& vec, vector<int>& temp, int esq, int meio, int direita) {
    int i = esq;      // Índice inicial da sublista esquerda
    int j = meio + 1;   // Índice inicial da sublista direita
    int k = esq;      // Índice inicial do vetor temporário
    long long inv_count = 0;  // Contador de inversões

    while (i <= meio && j <= direita) {
        if (vec[i] <= vec[j]) {
            temp[k++] = vec[i++];  
        } else {
            temp[k++] = vec[j++];  
            inv_count += (meio - i + 1);  // Conta todas as inversões, pois todos os elementos restantes na sublista esquerda são maiores que vec[j-1]
        }
    }
    // Copia os elementos restantes da sublista esquerda, se houver
    while (i <= meio) {
        temp[k++] = vec[i++];  
    }
    // Copia os elementos restantes da sublista direita, se houver
    while (j <= direita) {
        temp[k++] = vec[j++];  
    }
    // Copia o vetor temporário de volta para o vetor original
    for (int i = esq; i <= direita; i++) {
        vec[i] = temp[i];
    }
    return inv_count;
}

long long mergeSortAndCount(vector<int>& vec, vector<int>& temp, int esq, int direita) {
    long long inv_count = 0;
    if (esq < direita) {
        int meio = esq + (direita - esq) / 2;

        //divide os elementos da esquerda
        inv_count += mergeSortAndCount(vec, temp, esq, meio);
        //divide os elementos da direita
        inv_count += mergeSortAndCount(vec, temp, meio + 1, direita);
        //junta as duas metades e conta as inversoes
        inv_count += mergeAndCount(vec, temp, esq, meio, direita);
    }
    return inv_count;
}

long long calcularUltrapassagens(vector<Jogador>& jogadores) {
    int n = jogadores.size();
    vector<int> posicoes_finais(n);
    for (int i = 0; i < n; ++i) {
        posicoes_finais[i] = jogadores[i].sf;
    }
    vector<int> temp(n);
    return mergeSortAndCount(posicoes_finais, temp, 0, n - 1);
}

int main() {
    int T;  // Número de trechos
    cin >> T;
    vector<ResultadoTrecho> resultados;

    for (int trecho = 0; trecho < T; ++trecho) {
        int J;  // Número de jogadores
        cin >> J;
        vector<Jogador> jogadores(J);

        for (int i = 0; i < J; ++i) {
            cin >> jogadores[i].s0 >> jogadores[i].sf;
        }

        // Ordena os jogadores pela posição inicial (s0) de forma crescente
        sort(jogadores.begin(), jogadores.end(), compararPosicao);

        long long ultrapassagens = calcularUltrapassagens(jogadores);
        resultados.push_back({trecho, ultrapassagens});
    }

    // Imprime os resultados
    for (const auto& resultado : resultados) {
        cout << "Trecho " << resultado.indice << ": " << resultado.ultrapassagens << " ultrapassagens" << endl;
    }

    return 0;
}