from pyscipopt import Model

def solve():
    model = Model("Atividade_2_2")
    model.hideOutput()

    # Variáveis binárias
    x1 = model.addVar(vtype="B", name="x1")
    x2 = model.addVar(vtype="B", name="x2")

    # Função Objetivo
    model.setObjective(2 * x1 + 3 * x2, "maximize")

    # Restrições
    model.addCons(6 * x1 + 8 * x2 <= 10)

    model.optimize()

    # Saída esperada: valor de z, seguido por x1 e x2 em cada linha
    print(f"{model.getObjVal():.1f}")
    print(f"{model.getVal(x1):.1f}")
    print(f"{model.getVal(x2):.1f}")

if __name__ == "__main__":
    solve()