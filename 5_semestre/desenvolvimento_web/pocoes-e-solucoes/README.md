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


### Endpoints da API

| Método | Rota                  | Descrição                                  |
|--------|-----------------------|---------------------------------------------|
| GET    | `/api/potions`        | Lista todas as poções cadastradas           |
| POST   | `/api/potions`        | Cadastra uma nova poção (JSON no corpo)     |
| DELETE | `/api/potions/:id`    | Remove a poção com o `id` informado         |

Corpo esperado no `POST /api/potions`:

```json
{
  "nome": "Poção Exemplo",
  "descricao": "Descrição da poção.",
  "imagem": "https://exemplo.com/imagem.png",
  "preco": 250
}
```

## 2. Executando o frontend

O frontend é composto apenas por arquivos estáticos (HTML/CSS/JS), sem necessidade de
build. Com o backend já rodando em `http://localhost:3000`, basta servir a pasta
`frontend`. Recomenda-se usar um servidor estático simples para evitar restrições do
navegador ao abrir arquivos diretamente via `file://`:

```bash
cd frontend
npx serve -p 5500
```

Depois, acesse:

- **Loja (comprador):** http://localhost:5500/index.html
- **Administração:** http://localhost:5500/admin.html

> Alternativamente, qualquer outro servidor estático funciona (ex.: extensão "Live
> Server" do VS Code, `python3 -m http.server`, etc.). O importante é que o backend
> esteja rodando em `http://localhost:3000`, pois é esse endereço que está fixado em
> `frontend/js/main.js` e `frontend/js/admin.js` (constante `API_URL`).

## Funcionalidades

- **Página da loja (`index.html`):** descrição da loja, seção de histórico (fundada em
  1867) com fotos, listagem de poções (nome, imagem, descrição, preço e botão
  "Comprar") carregada via AJAX (`fetch`) a partir do Web Service, e rodapé com
  informações de contato.
- **Página de administração (`admin.html`):** formulário para cadastrar novas poções,
  listagem das poções existentes e botão para remover cada uma — todas as ações via
  AJAX, consumindo a mesma API.

## Observações de design

- Paleta de cores escuras (tons de roxo, vinho e dourado).
- Tipografia: `Gill Sans` (com fallback para `Gill Sans MT`, `Calibri` e
  `sans-serif`, já que a fonte é proprietária e não é incorporada via web font).
- Imagens das poções de exemplo usam placeholders (`placehold.co`); basta trocar a URL
  no cadastro/seed para usar imagens reais.
