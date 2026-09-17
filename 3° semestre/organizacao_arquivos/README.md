# Organização de Arquivos e Indexação por Árvore B

Projeto desenvolvido em **C** para a disciplina de Organização de Arquivos (ICMC-USP), implementando manipulação de dados em arquivos binários e indexação eficiente por meio de uma Árvore B.

## 📋 Sobre o projeto

O projeto é dividido em duas etapas principais:

1. **Conversão e manipulação de dados binários**
   Leitura de um arquivo `.csv` e conversão para um arquivo binário em disco, com funcionalidades de busca, inserção e remoção de registros diretamente sobre o arquivo binário.

2. **Indexação por Árvore B**
   Construção de uma estrutura de Árvore B sobre o arquivo binário gerado na primeira etapa, permitindo busca, inserção, atualização e remoção de registros de forma eficiente através da árvore.

## 🚀 Funcionalidades

- [ ] Conversão de CSV para arquivo binário
- [ ] Busca de registros no arquivo binário
- [ ] Inserção de registros no arquivo binário
- [ ] Remoção de registros no arquivo binário
- [ ] Construção da Árvore B a partir do arquivo binário
- [ ] Busca por chave via Árvore B
- [ ] Inserção via Árvore B
- [ ] Atualização de registros via Árvore B
- [ ] Remoção via Árvore B

## 🛠️ Tecnologias

- **Linguagem:** C
- Manipulação de arquivos binários (`fread`/`fwrite`)
- Estrutura de dados: Árvore B

## ⚙️ Como compilar e executar

```bash
# compilar
gcc -o organizacao_arquivos main.c [demais arquivos .c] -Wall

# executar
./organizacao_arquivos
```

## 📚 Aprendizados

Este projeto aprofundou conceitos de:
- Manipulação de arquivos em baixo nível (registros de tamanho fixo/variável em binário)
- Estruturas de dados para indexação eficiente em disco
- Trade-offs entre acesso sequencial e indexado a grandes volumes de dados

---

Desenvolvido por **Bárbara** — estudante de Ciência da Computação no ICMC-USP.
