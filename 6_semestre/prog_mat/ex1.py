from pyscipopt import Model

# Inicializa o modelo
model = Model("Atividade_1_2")

# Cria as variáveis de decisão (não-negativas por padrão, tipo contínuo)
x1 = model.addVar(vtype="C", lb=0.0, name="x1")
x2 = model.addVar(vtype="C", lb=0.0, name="x2")
x3 = model.addVar(vtype="C", lb=0.0, name="x3")

# Define a função objetivo (minimização)
model.setObjective(0.562 * x1 + 0.81 * x2 + 0.46 * x3, sense="minimize")

# Adiciona as restrições
model.addCons(0.2 * x1 + 0.5 * x2 + 0.4 * x3 >= 0.3, name="R1")
model.addCons(0.6 * x1 + 0.4 * x2 + 0.4 * x3 >= 0.5, name="R2")
model.addCons(x1 + x2 + x3 == 1.0, name="R3")

# Oculta logs do solver na saída padrão
model.hideOutput(True)

# Executa a otimização
model.optimize()

# Imprime o valor da função objetivo formatado conforme o exemplo de saída
print(f"{model.getObjVal():.2f}")