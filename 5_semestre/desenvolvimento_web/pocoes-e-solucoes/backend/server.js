import express from "express";
import cors from "cors";
import { Sequelize, DataTypes } from "sequelize";

const app = express();
const PORT = process.env.PORT || 3000;

app.use(cors());
app.use(express.json());

// ---------------------------------------------------------------------------
// Banco de dados SQLite em memória (conforme exigido na atividade)
// ---------------------------------------------------------------------------
const sequelize = new Sequelize({
  dialect: "sqlite",
  storage: ":memory:",
  logging: false,
});

const Potion = sequelize.define(
  "Potion",
  {
    nome: { type: DataTypes.STRING, allowNull: false },
    descricao: { type: DataTypes.TEXT, allowNull: false },
    imagem: { type: DataTypes.STRING, allowNull: false }, // URL ou caminho da imagem
    preco: { type: DataTypes.FLOAT, allowNull: false },
  },
  { tableName: "potions" }
);

// ---------------------------------------------------------------------------
// Dados iniciais (poções de exemplo fornecidas na atividade)
// ---------------------------------------------------------------------------
const seedPotions = [
  {
    nome: "Poção Blue Sky",
    descricao:
      "Essa poção provê um surto de inspiração por 24 horas. Foi utilizada por John Lennon quando escreveu Lucy in the Sky with Diamonds.",
    imagem: "https://i.ibb.co/ZzS7xb2/rsz-sky.png",
    preco: 300,
  },
  {
    nome: "Poção do Perfume Misterioso",
    descricao:
      "Essa poção faz com que você fique cheirando lilás e groselha por 24 dias. Essência muito admirada pelos bruxos.",
    imagem: "https://i.ibb.co/pyhZJXf/rsz-lilas.png",
    preco: 200,
  },
  {
    nome: "Poção de Pinus",
    descricao:
      "Essa poção faz com que você fique 10 cm mais alto! Observação: efeitos colaterais desconhecidos.",
    imagem: "https://i.ibb.co/DkzdL1q/rsz-pinus.png",
    preco: 3000,
  },
  {
    nome: "Poção da Beleza Eterna",
    descricao: "Veneno que mata rápido.",
    imagem: "https://i.ibb.co/9p872NK/rsz-1beleza.png",
    preco: 100,
  },
  {
    nome: "Poção do Arco Íris",
    descricao: "Traz felicidade momentânea. Pode durar de 10 minutos a 2 dias.",
    imagem: "https://i.ibb.co/PrC09MP/rsz-2unicornio.png",
    preco: 120,
  },
  {
    nome: "Caldeirão das Verdades Secretas",
    descricao: "As pessoas lhe dirão apenas verdades por 1 hora. É necessário beber os 5L.",
    imagem: "https://i.ibb.co/s9Lyvj8/rsz-verdades.png",
    preco: 150,
  },
];

// ---------------------------------------------------------------------------
// Inicialização
// ---------------------------------------------------------------------------
async function start() {
  await sequelize.sync();
  await Potion.bulkCreate(seedPotions);
  console.log(`Banco em memória populado com ${seedPotions.length} poções.`);

  // GET /api/potions -> lista todas as poções
  app.get("/api/potions", async (req, res) => {
    try {
      const potions = await Potion.findAll({ order: [["id", "ASC"]] });
      res.json(potions);
    } catch (err) {
      console.error(err);
      res.status(500).json({ erro: "Erro ao listar poções." });
    }
  });

  // POST /api/potions -> cadastra uma nova poção
  app.post("/api/potions", async (req, res) => {
    try {
      const { nome, descricao, imagem, preco } = req.body;
      if (!nome || !descricao || !imagem || preco === undefined || preco === "") {
        return res
          .status(400)
          .json({ erro: "Os campos nome, descrição, imagem e preço são obrigatórios." });
      }
      const potion = await Potion.create({
        nome,
        descricao,
        imagem,
        preco: parseFloat(preco),
      });
      res.status(201).json(potion);
    } catch (err) {
      console.error(err);
      res.status(500).json({ erro: "Erro ao cadastrar poção." });
    }
  });

  // DELETE /api/potions/:id -> remove uma poção
  app.delete("/api/potions/:id", async (req, res) => {
    try {
      const { id } = req.params;
      const deletados = await Potion.destroy({ where: { id } });
      if (!deletados) {
        return res.status(404).json({ erro: "Poção não encontrada." });
      }
      res.json({ sucesso: true });
    } catch (err) {
      console.error(err);
      res.status(500).json({ erro: "Erro ao remover poção." });
    }
  });

  app.listen(PORT, () => {
    console.log(`Servidor rodando em http://localhost:${PORT}`);
  });
}

start();
