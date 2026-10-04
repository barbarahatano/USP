from pyscipopt import Model

def solve():
    model = Model("Atividade_2_4")
    model.hideOutput()

    armazens = ['A', 'B', 'C', 'D']
    clientes = ['a', 'b', 'c', 'd', 'e']

    custo_fixo = {'A': 50, 'B': 32, 'C': 28, 'D': 36}
    capacidade = {'A': 35, 'B': 28, 'C': 22, 'D': 28}
    demanda = {'a': 14, 'b': 12, 'c': 10, 'd': 12, 'e': 8}

    custo_transporte = {
        'A': {'a': 2, 'b': 5, 'c': 1, 'd': 2, 'e': 5},
        'B': {'a': 4, 'b': 4, 'c': 9, 'd': 1, 'e': 4},
        'C': {'a': 1, 'b': 8, 'c': 5, 'd': 6, 'e': 2},
        'D': {'a': 7, 'b': 1, 'c': 2, 'd': 1, 'e': 8}
    }

    # Variáveis binárias de abertura de armazém
    y = {i: model.addVar(vtype="B", name=f"y_{i}") for i in armazens}

    # Variáveis contínuas de fluxo de transporte
    x = {(i, j): model.addVar(vtype="C", lb=0, name=f"x_{i}_{j}") 
         for i in armazens for j in clientes}

    # Função objetivo: minimizar custo total
    obj = sum(custo_fixo[i] * y[i] for i in armazens) + \
          sum(custo_transporte[i][j] * x[(i, j)] for i in armazens for j in clientes)
    model.setObjective(obj, "minimize")

    # Restrição 1: atender toda a procura dos clientes
    for j in clientes:
        model.addCons(sum(x[(i, j)] for i in armazens) == demanda[j])

    # Restrição 2: respeitar capacidade e condição de abertura
    for i in armazens:
        model.addCons(sum(x[(i, j)] for j in clientes) <= capacidade[i] * y[i])

    model.optimize()

    # Saída esperada: apenas o valor da função objetivo (com uma casa decimal)
    print(f"{model.getObjVal():.1f}")

if __name__ == "__main__":
    solve()