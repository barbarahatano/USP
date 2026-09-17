from pyscipopt import Model

# Inicializa o modelo
model = Model("Atividade_1_1")

# Cria as variáveis de decisão:
# Nota: x1 não tem limite inferior explícito no enunciado (lb=None),
# enquanto x2 tem x2 >= 0 (lb=0.0). Ambas são contínuas (vtype="C").
x1 = model.addVar(vtype="C", lb=None, name="x1")
x2 = model.addVar(vtype="C", lb=0.0, name="x2")

# Define a função objetivo (maximização)
model.setObjective(x1 + 2 * x2, sense="maximize")

# Adiciona as restrições
model.addCons(x1 + x2 <= 4, name="R1")
model.addCons(x1 <= 2, name="R2")
model.addCons(x2 <= 3, name="R3")

# Oculta logs do solver na saída padrão
model.hideOutput(True)

# Executa a otimização
model.optimize()

# Imprime o valor da função objetivo com 1 casa decimal (conforme o exemplo: 7.0)
print(f"{model.getObjVal():.1f}")