-- =============================================================================
-- PRESERVE5.0 -- Sistema de Manutencao Prescritiva e Gestao Sustentavel
--              de Ativos Industriais 5.0
-- =============================================================================
-- Script:  esquema.sql
-- Descricao: Criacao completa do esquema relacional no Oracle.
--            Baseado no Projeto Logico (Parte 2) com todas as tabelas,
--            chaves primarias, chaves estrangeiras e restricoes de integridade.
-- SGBD:    Oracle (testado no Oracle 19c+)
-- Autores: Barbara Naomi Morimoto Hatano, Cauã Honorato de Paula,
--          Daniel Afonso Borges dos Santos, Gustavo de Araujo Poffo,
--          Livia Rosito Reis dos Santos
-- =============================================================================

-- Remocao de tabelas existentes (ordem inversa das dependencias FK)
BEGIN
  EXECUTE IMMEDIATE 'DROP TABLE PreditivaMedicao CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN
  EXECUTE IMMEDIATE 'DROP TABLE UtilizaPeca CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN
  EXECUTE IMMEDIATE 'DROP TABLE Realiza CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN
  EXECUTE IMMEDIATE 'DROP TABLE Prescritiva CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN
  EXECUTE IMMEDIATE 'DROP TABLE Preditiva CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN
  EXECUTE IMMEDIATE 'DROP TABLE Preventiva CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN
  EXECUTE IMMEDIATE 'DROP TABLE Corretiva CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN
  EXECUTE IMMEDIATE 'DROP TABLE Manutencao CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN
  EXECUTE IMMEDIATE 'DROP TABLE Ocorrencia CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN
  EXECUTE IMMEDIATE 'DROP TABLE TipoFalha CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN
  EXECUTE IMMEDIATE 'DROP TABLE Medicao CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN
  EXECUTE IMMEDIATE 'DROP TABLE Sensor CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN
  EXECUTE IMMEDIATE 'DROP TABLE Descarte CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN
  EXECUTE IMMEDIATE 'DROP TABLE Compra CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN
  EXECUTE IMMEDIATE 'DROP TABLE Peca CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN
  EXECUTE IMMEDIATE 'DROP TABLE Fornecedor CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN
  EXECUTE IMMEDIATE 'DROP TABLE Tecnico CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN
  EXECUTE IMMEDIATE 'DROP TABLE Ativo CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN
  EXECUTE IMMEDIATE 'DROP SEQUENCE seq_manutencao_id';
EXCEPTION WHEN OTHERS THEN NULL; END;
/

-- =============================================================================
-- SEQUENCIA para chave sintetica de Manutencao
-- (Justificativa 2: uso de chave sintetica em Manutencao para simplificar
--  chaves estrangeiras nas subtabelas e relacoes associativas)
-- =============================================================================
CREATE SEQUENCE seq_manutencao_id
    START WITH 1
    INCREMENT BY 1
    NOCACHE
    NOCYCLE;

-- =============================================================================
-- TABELA: Ativo
-- Representa equipamentos/maquinas industriais cadastrados na plataforma.
-- Chave primaria: numeroSerie (identificador fisico unico do equipamento).
-- =============================================================================
CREATE TABLE Ativo (
    numeroSerie     VARCHAR2(50)  NOT NULL,
    nome            VARCHAR2(100) NOT NULL,
    tipo            VARCHAR2(50)  NOT NULL,
    fabricante      VARCHAR2(100) NOT NULL,
    modelo          VARCHAR2(100) NOT NULL,
    dataInstalacao  DATE          NOT NULL,
    localizacao     VARCHAR2(200) NOT NULL,
    -- Status operacional: Ativo, Em manutencao, Desativado
    statusOperacional VARCHAR2(30) NOT NULL
        CONSTRAINT ck_ativo_status
        CHECK (statusOperacional IN ('Ativo', 'Em manutencao', 'Desativado')),
    CONSTRAINT pk_ativo PRIMARY KEY (numeroSerie)
);

-- =============================================================================
-- TABELA: Tecnico
-- Profissionais responsaveis pelas atividades de manutencao.
-- Chave primaria: CPF.
-- Atributo composto "contato" foi separado em telefone e endereco
-- (Justificativa 10: evitar join desnecessario para dados de contato)
-- =============================================================================
CREATE TABLE Tecnico (
    CPF         CHAR(11)      NOT NULL,
    nome        VARCHAR2(100) NOT NULL,
    especialidade VARCHAR2(100) NOT NULL,
    telefone    VARCHAR2(20)  NOT NULL,
    endereco    VARCHAR2(200) NOT NULL,
    CONSTRAINT pk_tecnico PRIMARY KEY (CPF),
    CONSTRAINT ck_tecnico_cpf CHECK (REGEXP_LIKE(CPF, '^\d{11}$'))
);

-- =============================================================================
-- TABELA: Fornecedor
-- Empresas fornecedoras de pecas de reposicao.
-- Chave primaria: CNPJ.
-- Atributo composto "contato" separado em telefone e endereco (Justif. 10).
-- =============================================================================
CREATE TABLE Fornecedor (
    CNPJ        CHAR(14)      NOT NULL,
    nome        VARCHAR2(100) NOT NULL,
    telefone    VARCHAR2(20)  NOT NULL,
    endereco    VARCHAR2(200) NOT NULL,
    CONSTRAINT pk_fornecedor PRIMARY KEY (CNPJ),
    CONSTRAINT ck_fornecedor_cnpj CHECK (REGEXP_LIKE(CNPJ, '^\d{14}$'))
);

-- =============================================================================
-- TABELA: Peca
-- Componentes utilizados nas manutencoes.
-- Chave primaria: nome (identificador do componente no catalogo).
-- Todo componente deve ter um fornecedor (participacao total de Peca
-- no relacionamento Fornece - Justificativa 6).
-- =============================================================================
CREATE TABLE Peca (
    nome        VARCHAR2(100) NOT NULL,
    dimensoes   VARCHAR2(100),
    fabricante  VARCHAR2(100) NOT NULL,
    CNPJ_forn   CHAR(14)      NOT NULL,
    CONSTRAINT pk_peca PRIMARY KEY (nome),
    CONSTRAINT fk_peca_forn FOREIGN KEY (CNPJ_forn)
        REFERENCES Fornecedor(CNPJ)
        -- Oracle nao suporta ON DELETE RESTRICT; o comportamento padrao ja e restritivo
);

-- =============================================================================
-- TABELA: Compra (Agregacao Compra)
-- Representa o historico de fornecimento de pecas por fornecedores.
-- Chave primaria: (CNPJ_forn, nome_peca, dataCompra).
-- Armazena atributos historicos Quantidade e Custo por transacao.
-- (Justificativa 7: agregacao Compra como tabela propria para auditoria)
-- =============================================================================
CREATE TABLE Compra (
    CNPJ_forn   CHAR(14)       NOT NULL,
    nome_peca   VARCHAR2(100)  NOT NULL,
    dataCompra  DATE           NOT NULL,
    quantidade  INTEGER        NOT NULL,
    custo       NUMBER(12,2)   NOT NULL,
    CONSTRAINT pk_compra PRIMARY KEY (CNPJ_forn, nome_peca, dataCompra),
    CONSTRAINT fk_compra_forn FOREIGN KEY (CNPJ_forn)
        REFERENCES Fornecedor(CNPJ),
    CONSTRAINT fk_compra_peca FOREIGN KEY (nome_peca)
        REFERENCES Peca(nome),
    -- Note 9: Quantidade e Custo devem ser maiores que zero
    CONSTRAINT ck_compra_qtd  CHECK (quantidade > 0),
    CONSTRAINT ck_compra_custo CHECK (custo > 0)
);

-- =============================================================================
-- TABELA: Sensor
-- Entidade fraca dependente de Ativo. Monitora variaveis operacionais.
-- Chave primaria composta: (numeroSerie, numeroSensor).
-- (Justificativa 8: mapeamento 1:N com FK no lado N)
-- =============================================================================
CREATE TABLE Sensor (
    numeroSerie     VARCHAR2(50)  NOT NULL,
    numeroSensor    INTEGER       NOT NULL,
    tipoSensor      VARCHAR2(50)  NOT NULL,
    unidadeMedida   VARCHAR2(30)  NOT NULL,
    dataInstalacao  DATE          NOT NULL,
    CONSTRAINT pk_sensor PRIMARY KEY (numeroSerie, numeroSensor),
    CONSTRAINT fk_sensor_ativo FOREIGN KEY (numeroSerie)
        REFERENCES Ativo(numeroSerie)
        ON DELETE CASCADE
);

-- =============================================================================
-- TABELA: Medicao
-- Entidade fraca dependente de Sensor. Armazena leituras temporais.
-- Chave primaria composta: (numeroSerie, numeroSensor, dataHora).
-- (Justificativa 9: tabela propria para serie temporal - garante historico)
-- =============================================================================
CREATE TABLE Medicao (
    numeroSerie     VARCHAR2(50)  NOT NULL,
    numeroSensor    INTEGER       NOT NULL,
    dataHora        TIMESTAMP     NOT NULL,
    valorMedido     NUMBER(10,4)  NOT NULL,
    CONSTRAINT pk_medicao PRIMARY KEY (numeroSerie, numeroSensor, dataHora),
    CONSTRAINT fk_medicao_sensor FOREIGN KEY (numeroSerie, numeroSensor)
        REFERENCES Sensor(numeroSerie, numeroSensor)
        ON DELETE CASCADE
);

-- =============================================================================
-- TABELA: TipoFalha
-- Catalogo de tipos de falha que podem ocorrer nos ativos.
-- Chave primaria: tipoFalha (descricao do tipo).
-- =============================================================================
CREATE TABLE TipoFalha (
    tipoFalha       VARCHAR2(100) NOT NULL,
    descricao       VARCHAR2(500),
    nivelSeveridade VARCHAR2(20)  NOT NULL,
    CONSTRAINT pk_tipofalha PRIMARY KEY (tipoFalha),
    CONSTRAINT ck_tipofalha_niv CHECK (nivelSeveridade IN ('Baixo', 'Medio', 'Alto', 'Critico'))
);

-- =============================================================================
-- TABELA: Ocorrencia (Agregacao Ocorrencia)
-- Representa o evento especifico de uma falha em um determinado ativo.
-- Chave primaria natural: (numeroSerie, tipoFalha, dataFalha).
-- (Justificativa 6: agregacao Ocorrencia como tabela para permitir que
--  Manutencao Corretiva se associe ao evento especifico, nao a falha genérica)
-- =============================================================================
CREATE TABLE Ocorrencia (
    numeroSerie VARCHAR2(50)  NOT NULL,
    tipoFalha   VARCHAR2(100) NOT NULL,
    dataFalha   DATE          NOT NULL,
    descricao   VARCHAR2(500),
    CONSTRAINT pk_ocorrencia PRIMARY KEY (numeroSerie, tipoFalha, dataFalha),
    CONSTRAINT fk_ocorr_ativo FOREIGN KEY (numeroSerie)
        REFERENCES Ativo(numeroSerie),
    CONSTRAINT fk_ocorr_falha FOREIGN KEY (tipoFalha)
        REFERENCES TipoFalha(tipoFalha)
);

-- =============================================================================
-- TABELA: Manutencao (superclasse, entidade fraca de Ativo)
-- Chave sintetica idManutencao para simplificar FKs nas subtabelas.
-- Restricao UNIQUE (numeroSerie, dataInicio) preserva a semantica da
-- chave original do modelo conceitual.
-- (Justificativa 2: chave sintetica em Manutencao)
-- Note 5: dataFim >= dataInicio
-- =============================================================================
CREATE TABLE Manutencao (
    idManutencao    INTEGER       NOT NULL,
    numeroSerie     VARCHAR2(50)  NOT NULL,
    dataInicio      DATE          NOT NULL,
    dataFim         DATE,
    descricao       VARCHAR2(1000),
    -- Tipo indica qual subtabela contem os detalhes (garantido pela aplicacao)
    tipoManutencao  VARCHAR2(20)  NOT NULL,
    CONSTRAINT pk_manutencao PRIMARY KEY (idManutencao),
    CONSTRAINT uq_manut_nat UNIQUE (numeroSerie, dataInicio),
    CONSTRAINT fk_manut_ativo FOREIGN KEY (numeroSerie)
        REFERENCES Ativo(numeroSerie),
    CONSTRAINT ck_manut_tipo CHECK (
        tipoManutencao IN ('Corretiva', 'Preventiva', 'Preditiva', 'Prescritiva')
    ),
    -- Note 5: data de termino deve ser >= data de inicio
    CONSTRAINT ck_manut_datas CHECK (dataFim IS NULL OR dataFim >= dataInicio)
);

-- =============================================================================
-- TABELA: Corretiva (subtipo de Manutencao)
-- Possui FK para Ocorrencia (participacao total: toda corretiva exige ocorrencia)
-- UNIQUE sobre (FK de Ocorrencia) garante cardinalidade 1:1 entre
-- ManutencaoCorretiva e Ocorrencia (Justificativa 4)
-- =============================================================================
CREATE TABLE Corretiva (
    idManutencao        INTEGER      NOT NULL,
    causaFalha          VARCHAR2(500),
    -- FK tripla referenciando a chave primaria composta de Ocorrencia
    ocorr_numeroSerie   VARCHAR2(50)  NOT NULL,
    ocorr_tipoFalha     VARCHAR2(100) NOT NULL,
    ocorr_dataFalha     DATE          NOT NULL,
    CONSTRAINT pk_corretiva PRIMARY KEY (idManutencao),
    CONSTRAINT fk_corr_manut FOREIGN KEY (idManutencao)
        REFERENCES Manutencao(idManutencao)
        ON DELETE CASCADE,
    CONSTRAINT fk_corr_ocorr FOREIGN KEY (ocorr_numeroSerie, ocorr_tipoFalha, ocorr_dataFalha)
        REFERENCES Ocorrencia(numeroSerie, tipoFalha, dataFalha),
    -- UNIQUE garante que uma ocorrencia nao receba multiplas corretivas (1:1)
    CONSTRAINT uq_corr_ocorr UNIQUE (ocorr_numeroSerie, ocorr_tipoFalha, ocorr_dataFalha)
);

-- =============================================================================
-- TABELA: Preventiva (subtipo de Manutencao)
-- Atributo especifico: periodicidade (intervalo em dias entre manutencoes)
-- =============================================================================
CREATE TABLE Preventiva (
    idManutencao    INTEGER      NOT NULL,
    periodicidade   INTEGER,  -- intervalo em dias
    CONSTRAINT pk_preventiva PRIMARY KEY (idManutencao),
    CONSTRAINT fk_prev_manut FOREIGN KEY (idManutencao)
        REFERENCES Manutencao(idManutencao)
        ON DELETE CASCADE,
    CONSTRAINT ck_prev_period CHECK (periodicidade IS NULL OR periodicidade > 0)
);

-- =============================================================================
-- TABELA: Preditiva (subtipo de Manutencao)
-- Baseada em dados de sensores; detalhes em PreditivaMedicao
-- =============================================================================
CREATE TABLE Preditiva (
    idManutencao    INTEGER NOT NULL,
    CONSTRAINT pk_preditiva PRIMARY KEY (idManutencao),
    CONSTRAINT fk_pred_manut FOREIGN KEY (idManutencao)
        REFERENCES Manutencao(idManutencao)
        ON DELETE CASCADE
);

-- =============================================================================
-- TABELA: Prescritiva (subtipo de Manutencao)
-- Manutencao baseada em prescricoes de algoritmos ou especialistas
-- =============================================================================
CREATE TABLE Prescritiva (
    idManutencao    INTEGER      NOT NULL,
    recomendacao    VARCHAR2(1000),
    CONSTRAINT pk_prescritiva PRIMARY KEY (idManutencao),
    CONSTRAINT fk_presc_manut FOREIGN KEY (idManutencao)
        REFERENCES Manutencao(idManutencao)
        ON DELETE CASCADE
);

-- =============================================================================
-- TABELA: PreditivaMedicao
-- Relacionamento N:N entre Preditiva e Medicao.
-- Vincula medicoes que motivaram a manutencao preditiva.
-- Note 10: medicoes devem pertencer ao mesmo ativo da manutencao.
--          Essa restricao e tratada na aplicacao.
-- =============================================================================
CREATE TABLE PreditivaMedicao (
    idManutencao    INTEGER      NOT NULL,
    med_numeroSerie VARCHAR2(50) NOT NULL,
    med_numSensor   INTEGER      NOT NULL,
    med_dataHora    TIMESTAMP    NOT NULL,
    CONSTRAINT pk_predmedicao PRIMARY KEY (idManutencao, med_numeroSerie, med_numSensor, med_dataHora),
    CONSTRAINT fk_predmed_pred FOREIGN KEY (idManutencao)
        REFERENCES Preditiva(idManutencao)
        ON DELETE CASCADE,
    CONSTRAINT fk_predmed_med FOREIGN KEY (med_numeroSerie, med_numSensor, med_dataHora)
        REFERENCES Medicao(numeroSerie, numeroSensor, dataHora)
);

-- =============================================================================
-- TABELA: Realiza
-- Relacionamento N:N entre Tecnico e Manutencao.
-- Note 11: toda manutencao precisa ter ao menos um tecnico (tratado na aplicacao)
-- (Justificativa 5: relacionamentos N:N mapeados em tabelas associativas)
-- =============================================================================
CREATE TABLE Realiza (
    CPF_tecnico     CHAR(11)    NOT NULL,
    idManutencao    INTEGER     NOT NULL,
    CONSTRAINT pk_realiza PRIMARY KEY (CPF_tecnico, idManutencao),
    CONSTRAINT fk_real_tec FOREIGN KEY (CPF_tecnico)
        REFERENCES Tecnico(CPF),
    CONSTRAINT fk_real_manut FOREIGN KEY (idManutencao)
        REFERENCES Manutencao(idManutencao)
        ON DELETE CASCADE
);

-- =============================================================================
-- TABELA: UtilizaPeca
-- Relacionamento N:N entre Manutencao e Peca, com atributo quantidadeUtilizada.
-- (Justificativa 5: N:N com atributo proprio na tabela associativa)
-- Note 9: quantidadeUtilizada deve ser maior que zero
-- =============================================================================
CREATE TABLE UtilizaPeca (
    idManutencao        INTEGER      NOT NULL,
    nome_peca           VARCHAR2(100) NOT NULL,
    quantidadeUtilizada INTEGER      NOT NULL,
    CONSTRAINT pk_utilizapeca PRIMARY KEY (idManutencao, nome_peca),
    CONSTRAINT fk_util_manut FOREIGN KEY (idManutencao)
        REFERENCES Manutencao(idManutencao)
        ON DELETE CASCADE,
    CONSTRAINT fk_util_peca FOREIGN KEY (nome_peca)
        REFERENCES Peca(nome),
    -- Note 9: quantidade deve ser maior que zero
    CONSTRAINT ck_util_qtd CHECK (quantidadeUtilizada > 0)
);

-- =============================================================================
-- TABELA: Descarte
-- Entidade fraca dependente de Ativo. Registra o fim do ciclo de vida.
-- Chave primaria: (numeroSerie, dataDescarte).
-- Note 8: dataDescarte deve ser posterior a dataInstalacao do ativo
--         (verificada na aplicacao via trigger ou logica de negocio)
-- =============================================================================
CREATE TABLE Descarte (
    numeroSerie         VARCHAR2(50)  NOT NULL,
    dataDescarte        DATE          NOT NULL,
    metodoDescarte      VARCHAR2(200) NOT NULL,
    empresaResponsavel  VARCHAR2(200) NOT NULL,
    CONSTRAINT pk_descarte PRIMARY KEY (numeroSerie, dataDescarte),
    CONSTRAINT fk_desc_ativo FOREIGN KEY (numeroSerie)
        REFERENCES Ativo(numeroSerie)
        ON DELETE CASCADE
);