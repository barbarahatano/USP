from pyscipopt import Model

def solve():
    model = Model("Atividade_2_3")
    model.hideOutput()

    # x1 é contínua e x2 é inteira
    x1 = model.addVar(vtype="C", lb=0, name="x1")
    x2 = model.addVar(vtype="I", lb=0, name="x2")

    # Função Objetivo
    model.setObjective(10 * x1 + 6 * x2, "maximize")

    # Restrições
    model.addCons(9 * x1 + 5 * x2 <= 45)
    model.addCons(-4 * x1 + 5 * x2 <= 5)

    model.optimize()

    # Saída com round(valor, 2)
    obj = round(model.getObjVal(), 2)
    val_x1 = round(model.getVal(x1), 2)
    val_x2 = round(model.getVal(x2), 2)

    print(f"{obj}")
    print(f"{val_x1}")
    print(f"{val_x2}")

if __name__ == "__main__":
    solve()