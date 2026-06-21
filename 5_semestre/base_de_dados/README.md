# PRESERVE5.0 – Parte 3: Guia de Instalação e Uso

## Estrutura do projeto

```
bd/
├── sql/
│   ├── esquema.sql      ← DDL: criação das tabelas
│   ├── dados.sql        ← DML: dados iniciais
│   └── consultas.sql    ← 6 consultas SQL complexas
└── app/
    ├── app.py           ← Protótipo CLI principal
    ├── db.py            ← Módulo de conexão Oracle
    ├── .env.example     ← Modelo de configuração
    └── requirements.txt ← Dependências Python
```

---

## 1. Configurar credenciais do banco

1. Copie `.env.example` para `.env` (na pasta `app/`):
   ```
   copy app\.env.example app\.env
   ```
2. Edite `app\.env` e preencha com seus dados do Oracle SQL Developer:
   ```
   DB_USER=seu_usuario
   DB_PASSWORD=sua_senha
   DB_DSN=host:porta/service_name
   ```
   > **Dica**: No SQL Developer, ao criar/editar uma conexão, você verá o hostname, porta e service name.

---

## 2. Criar o banco de dados (no SQL Developer)

1. Abra o **Oracle SQL Developer**
2. Conecte-se ao seu banco
3. Abra `sql/esquema.sql` → pressione **F5** para executar
4. Abra `sql/dados.sql` → pressione **F5** para executar

> Você verá as tabelas criadas no painel Objects do SQL Developer.

---

## 3. Instalar dependências Python

Abra o terminal (PowerShell) na pasta `app/` e execute:

```powershell
py -m pip install -r requirements.txt
```

---

## 4. Rodar o protótipo

```powershell
py app\app.py
```

O sistema abrirá o menu principal no terminal:

```
════════════════════════════════════════════════════════════
  PRESERVE5.0  │  Menu Principal
════════════════════════════════════════════════════════════

  Bem-vindo ao sistema PRESERVE5.0
  Plataforma de Gestão de Ativos Industriais

  1. Cadastros
  2. Consultas
  0. Sair
```

---

## 5. Rodar as consultas no SQL Developer

Abra `sql/consultas.sql` e execute cada consulta separadamente com **Ctrl+Enter**.

> As consultas com `:v_serie` e `:v_data_ini` são **parametrizadas** — o SQL Developer abrirá uma janela pedindo o valor automaticamente.

---

## 6. Funcionalidades do protótipo

### Cadastros
| Opção | Descrição |
|-------|-----------|
| 1     | Cadastrar ativo industrial (motor, compressor, robô…) |
| 2     | Cadastrar técnico de manutenção |
| 3     | Registrar manutenção (Corretiva / Preventiva / Preditiva / Prescritiva) |

### Consultas
| Opção | Descrição | Técnica SQL |
|-------|-----------|-------------|
| 1 | Histórico de manutenções de um ativo | JOIN múltiplo + LISTAGG |
| 2 | Técnicos em **todas** as manutenções de um ativo | **Divisão Relacional** (NOT EXISTS duplo) |
| 3 | Ranking de peças mais utilizadas | GROUP BY + subconsulta correlacionada |
| 4 | Ativos sem nenhuma manutenção | LEFT JOIN + IS NULL |
| 5 | Custo por fornecedor em período | GROUP BY + RATIO_TO_REPORT |
| 6 | Rastreabilidade de falhas críticas | JOIN múltiplo + subconsultas correlacionadas |
