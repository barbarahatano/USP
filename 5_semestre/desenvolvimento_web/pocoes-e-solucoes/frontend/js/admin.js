// URL base do Web Service
const API_URL = "http://localhost:3000/api/potions";

const form = document.getElementById("form-cadastro");
const feedbackCadastro = document.getElementById("feedback-cadastro");
const listaAdmin = document.getElementById("lista-admin");
const statusLista = document.getElementById("status-lista");

function mostrarFeedback(elemento, mensagem, tipo) {
  elemento.textContent = mensagem;
  elemento.className = `feedback ${tipo}`;
  setTimeout(() => {
    elemento.textContent = "";
    elemento.className = "feedback";
  }, 4000);
}

function criarItemAdmin(pocao) {
  const item = document.createElement("div");
  item.className = "item-admin";

  item.innerHTML = `
    <img src="${pocao.imagem}" alt="${pocao.nome}" />
    <div class="info">
      <h4>${pocao.nome}</h4>
      <p>${pocao.descricao}</p>
      <p><strong>${pocao.preco} moedas</strong></p>
    </div>
    <button class="botao-remover" type="button">Remover</button>
  `;

  item.querySelector(".botao-remover").addEventListener("click", () => {
    removerPocao(pocao.id);
  });

  return item;
}

function carregarPocoes() {
  fetch(API_URL)
    .then((resposta) => {
      if (!resposta.ok) throw new Error("Erro ao buscar poções.");
      return resposta.json();
    })
    .then((pocoes) => {
      statusLista.style.display = "none";
      listaAdmin.innerHTML = "";

      if (pocoes.length === 0) {
        listaAdmin.innerHTML = `<p class="mensagem-status">Nenhuma poção cadastrada ainda.</p>`;
        return;
      }

      pocoes.forEach((pocao) => {
        listaAdmin.appendChild(criarItemAdmin(pocao));
      });
    })
    .catch((erro) => {
      console.error(erro);
      statusLista.style.display = "block";
      statusLista.textContent =
        "Não foi possível carregar as poções. Verifique se o Web Service está rodando.";
    });
}

function cadastrarPocao(dados) {
  fetch(API_URL, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(dados),
  })
    .then((resposta) => {
      if (!resposta.ok) throw new Error("Erro ao cadastrar poção.");
      return resposta.json();
    })
    .then(() => {
      mostrarFeedback(feedbackCadastro, "Poção cadastrada com sucesso!", "sucesso");
      form.reset();
      carregarPocoes();
    })
    .catch((erro) => {
      console.error(erro);
      mostrarFeedback(feedbackCadastro, "Erro ao cadastrar poção.", "erro");
    });
}

function removerPocao(id) {
  fetch(`${API_URL}/${id}`, { method: "DELETE" })
    .then((resposta) => {
      if (!resposta.ok) throw new Error("Erro ao remover poção.");
      return resposta.json();
    })
    .then(() => {
      carregarPocoes();
    })
    .catch((erro) => {
      console.error(erro);
      alert("Erro ao remover poção.");
    });
}

form.addEventListener("submit", (evento) => {
  evento.preventDefault();

  const dados = {
    nome: document.getElementById("nome").value.trim(),
    descricao: document.getElementById("descricao").value.trim(),
    imagem: document.getElementById("imagem").value.trim(),
    preco: document.getElementById("preco").value,
  };

  cadastrarPocao(dados);
});

document.addEventListener("DOMContentLoaded", carregarPocoes);
