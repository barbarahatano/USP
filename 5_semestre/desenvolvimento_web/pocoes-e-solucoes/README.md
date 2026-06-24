# Poções e Soluções — Atividade Prática 2 (SCC0219)

Web Service + páginas (loja e administração) para a loja fictícia **Poções e Soluções**,
de Annabelle Merigold.

## Estrutura do projeto

```
pocoes-e-solucoes/
├── backend/              # Web Service (Node.js + Express + Sequelize + SQLite em memória)
│   ├── server.js
│   ├── package.json
│   └── .gitignore
├── frontend/              # Páginas estáticas (HTML + CSS + JS puro, com AJAX/fetch)
│   ├── index.html         # Página da loja (comprador)
│   ├── admin.html         # Página de administração (cadastrar/listar/remover)
│   ├── css/style.css
│   └── js/
│       ├── main.js        # Lógica da página da loja
│       └── admin.js       # Lógica da página de administração
└── README.md
```

## Requisitos

- [Node.js](https://nodejs.org/) versão 18 ou superior (inclui o `npm`)

## 1. Configurando e executando o Web Service (backend)

```bash
cd backend
npm install
npm start
```

O servidor sobe em **http://localhost:3000**. O banco de dados é **SQLite em modo
memória** (`:memory:`, via Sequelize) e é automaticamente populado, a cada
inicialização, com 6 poções de exemplo (Blue Sky, Perfume Misterioso, Pinus, Beleza
Eterna, Arco Íris e Caldeirão das Verdades Secretas). Como o banco é em memória, os
dados são reiniciados sempre que o servidor é reiniciado.

Durante o desenvolvimento, você pode usar `npm run dev` para reiniciar o servidor
automaticamente a cada alteração no código (usa `node --watch`).

