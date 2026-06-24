// URL base do Web Service
const API_URL = "http://localhost:3000/api/potions";

const listaPocoes = document.getElementById("lista-pocoes");
const statusCarregamento = document.getElementById("status-carregamento");

function formatarPreco(preco) {
  return `${preco} moedas`;
}

function criarCardPocao(pocao) {
  const card = document.createElement("div");
  card.className = "card-pocao";

  card.innerHTML = `
    <img src="${pocao.imagem}" alt="${pocao.nome}" />
    <div class="conteudo">
      <h3>${pocao.nome}</h3>
      <p class="descricao">${pocao.descricao}</p>
      <p class="preco">${formatarPreco(pocao.preco)}</p>
      <button class="botao-comprar" type="button">Comprar</button>
    </div>
  `;

  // Funcionalidade de compra não é exigida nesta entrega
  card.querySelector(".botao-comprar").addEventListener("click", () => {
    alert(`Funcionalidade de compra para "${pocao.nome}" em breve!`);
  });

  return card;
}

function carregarPocoes() {
  fetch(API_URL)
    .then((resposta) => {
      if (!resposta.ok) throw new Error("Erro ao buscar poções.");
      return resposta.json();
    })
    .then((pocoes) => {
      statusCarregamento.style.display = "none";
      listaPocoes.innerHTML = "";

      if (pocoes.length === 0) {
        listaPocoes.innerHTML = `<p class="mensagem-status">Nenhuma poção cadastrada ainda.</p>`;
        return;
      }

      pocoes.forEach((pocao) => {
        listaPocoes.appendChild(criarCardPocao(pocao));
      });
    })
    .catch((erro) => {
      console.error(erro);
      statusCarregamento.textContent =
        "Não foi possível carregar as poções. Verifique se o Web Service está rodando.";
    });
}

document.addEventListener("DOMContentLoaded", carregarPocoes);
