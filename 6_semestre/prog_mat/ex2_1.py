from pyscipopt import Model

def solve():
    model = Model("Atividade_2_1")
    model.hideOutput()

    # Variáveis inteiras não negativas
    x1 = model.addVar(vtype="I", lb=0, name="x1")
    x2 = model.addVar(vtype="I", lb=0, name="x2")

    # Função Objetivo: Maximização
    model.setObjective(10 * x1 + 6 * x2, "maximize")

    # Restrições
    model.addCons(9 * x1 + 5 * x2 <= 45)
    model.addCons(-4 * x1 + 5 * x2 <= 5)

    model.optimize()

    # Saída esperada: apenas o valor da função objetivo
    obj_val = model.getObjVal()
    print(f"{obj_val:.1f}")

if __name__ == "__main__":
    solve()