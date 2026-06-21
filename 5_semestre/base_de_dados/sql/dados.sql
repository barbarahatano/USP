-- =============================================================================
-- PRESERVE5.0 -- Sistema de Manutencao Prescritiva e Gestao Sustentavel
--              de Ativos Industriais 5.0
-- =============================================================================
-- Script:  dados.sql
-- Descricao: Alimentacao inicial da base de dados com dados representativos
--            do dominio industrial. Minimo de 2 tuplas por tabela.
-- SGBD:    Oracle
-- Executar APOS esquema.sql
-- =============================================================================

-- -----------------------------------------------------------------------------
-- Fornecedores
-- -----------------------------------------------------------------------------
INSERT INTO Fornecedor (CNPJ, nome, telefone, endereco)
VALUES ('12345678000101', 'TechParts Industria Ltda', '(11) 3344-5566',
        'Av. Industrial, 1500 - Sao Paulo, SP');

INSERT INTO Fornecedor (CNPJ, nome, telefone, endereco)
VALUES ('98765432000199', 'Global Componentes S/A', '(19) 2233-4455',
        'Rod. SP-310, Km 235 - Sao Carlos, SP');

INSERT INTO Fornecedor (CNPJ, nome, telefone, endereco)
VALUES ('55544433000177', 'EcoRecicla Solucoes Ltda', '(11) 4455-6677',
        'Rua da Reciclagem, 800 - Santo Andre, SP');

-- -----------------------------------------------------------------------------
-- Pecas
-- -----------------------------------------------------------------------------
INSERT INTO Peca (nome, dimensoes, fabricante, CNPJ_forn)
VALUES ('Rolamento SKF 6205', '25x52x15mm', 'SKF', '12345678000101');

INSERT INTO Peca (nome, dimensoes, fabricante, CNPJ_forn)
VALUES ('Correia dentada GT3 HTD', '450mm x 15mm', 'Gates', '12345678000101');

INSERT INTO Peca (nome, dimensoes, fabricante, CNPJ_forn)
VALUES ('Sensor de temperatura PT100', '6mm x 100mm', 'Omega', '98765432000199');

INSERT INTO Peca (nome, dimensoes, fabricante, CNPJ_forn)
VALUES ('Filtro de oleo hidraulico HF35', '150x80mm', 'Parker', '98765432000199');

INSERT INTO Peca (nome, dimensoes, fabricante, CNPJ_forn)
VALUES ('Veda-cao mecanica 20mm', '20x35x7mm', 'Flowserve', '55544433000177');

-- -----------------------------------------------------------------------------
-- Compras (Agregacao Compra: historico de fornecimento)
-- -----------------------------------------------------------------------------
INSERT INTO Compra (CNPJ_forn, nome_peca, dataCompra, quantidade, custo)
VALUES ('12345678000101', 'Rolamento SKF 6205',
        DATE '2025-03-10', 50, 1250.00);

INSERT INTO Compra (CNPJ_forn, nome_peca, dataCompra, quantidade, custo)
VALUES ('12345678000101', 'Rolamento SKF 6205',
        DATE '2025-09-15', 30, 780.00);

INSERT INTO Compra (CNPJ_forn, nome_peca, dataCompra, quantidade, custo)
VALUES ('12345678000101', 'Correia dentada GT3 HTD',
        DATE '2025-04-20', 20, 940.00);

INSERT INTO Compra (CNPJ_forn, nome_peca, dataCompra, quantidade, custo)
VALUES ('98765432000199', 'Sensor de temperatura PT100',
        DATE '2025-06-01', 10, 2100.00);

INSERT INTO Compra (CNPJ_forn, nome_peca, dataCompra, quantidade, custo)
VALUES ('98765432000199', 'Filtro de oleo hidraulico HF35',
        DATE '2026-01-08', 40, 3600.00);

INSERT INTO Compra (CNPJ_forn, nome_peca, dataCompra, quantidade, custo)
VALUES ('55544433000177', 'Veda-cao mecanica 20mm',
        DATE '2025-11-22', 25, 875.00);

-- -----------------------------------------------------------------------------
-- Ativos
-- -----------------------------------------------------------------------------
INSERT INTO Ativo (numeroSerie, nome, tipo, fabricante, modelo,
                   dataInstalacao, localizacao, statusOperacional)
VALUES ('MTR-001', 'Motor Eletrico Principal', 'Motor Eletrico',
        'WEG', 'W22 IE3 75cv',
        DATE '2020-03-15', 'Linha A - Posto 3', 'Ativo');

INSERT INTO Ativo (numeroSerie, nome, tipo, fabricante, modelo,
                   dataInstalacao, localizacao, statusOperacional)
VALUES ('CMP-002', 'Compressor de Ar Rotativo', 'Compressor',
        'Atlas Copco', 'GA30 Plus',
        DATE '2019-07-22', 'Sala de Utilidades - Bloco B', 'Ativo');

INSERT INTO Ativo (numeroSerie, nome, tipo, fabricante, modelo,
                   dataInstalacao, localizacao, statusOperacional)
VALUES ('RBT-003', 'Braco Robotico de Soldagem', 'Robo Industrial',
        'KUKA', 'KR 10 R1100',
        DATE '2021-11-05', 'Linha B - Celula 2', 'Em manutencao');

INSERT INTO Ativo (numeroSerie, nome, tipo, fabricante, modelo,
                   dataInstalacao, localizacao, statusOperacional)
VALUES ('EST-004', 'Esteira de Transporte', 'Esteira',
        'Intral', 'ET-300',
        DATE '2018-02-10', 'Patio de Expedicao', 'Ativo');

INSERT INTO Ativo (numeroSerie, nome, tipo, fabricante, modelo,
                   dataInstalacao, localizacao, statusOperacional)
VALUES ('TRF-005', 'Transformador de Potencia', 'Transformador',
        'ABB', 'TrafoPlus 500KVA',
        DATE '2016-06-30', 'Subestacao Principal', 'Desativado');

-- -----------------------------------------------------------------------------
-- Sensores (entidade fraca de Ativo)
-- -----------------------------------------------------------------------------
INSERT INTO Sensor (numeroSerie, numeroSensor, tipoSensor, unidadeMedida, dataInstalacao)
VALUES ('MTR-001', 1, 'Temperatura', 'Celsius', DATE '2020-03-15');

INSERT INTO Sensor (numeroSerie, numeroSensor, tipoSensor, unidadeMedida, dataInstalacao)
VALUES ('MTR-001', 2, 'Vibracao', 'mm/s', DATE '2020-03-15');

INSERT INTO Sensor (numeroSerie, numeroSensor, tipoSensor, unidadeMedida, dataInstalacao)
VALUES ('CMP-002', 1, 'Pressao', 'Bar', DATE '2019-07-22');

INSERT INTO Sensor (numeroSerie, numeroSensor, tipoSensor, unidadeMedida, dataInstalacao)
VALUES ('CMP-002', 2, 'Consumo Energetico', 'kWh', DATE '2019-07-22');

INSERT INTO Sensor (numeroSerie, numeroSensor, tipoSensor, unidadeMedida, dataInstalacao)
VALUES ('RBT-003', 1, 'Temperatura', 'Celsius', DATE '2021-11-05');

INSERT INTO Sensor (numeroSerie, numeroSensor, tipoSensor, unidadeMedida, dataInstalacao)
VALUES ('EST-004', 1, 'Vibracao', 'mm/s', DATE '2018-02-10');

-- -----------------------------------------------------------------------------
-- Medicoes (entidade fraca de Sensor)
-- -----------------------------------------------------------------------------
INSERT INTO Medicao (numeroSerie, numeroSensor, dataHora, valorMedido)
VALUES ('MTR-001', 1, TIMESTAMP '2026-06-01 08:00:00', 72.5);

INSERT INTO Medicao (numeroSerie, numeroSensor, dataHora, valorMedido)
VALUES ('MTR-001', 1, TIMESTAMP '2026-06-01 09:00:00', 74.8);

INSERT INTO Medicao (numeroSerie, numeroSensor, dataHora, valorMedido)
VALUES ('MTR-001', 1, TIMESTAMP '2026-06-01 10:00:00', 89.3);

INSERT INTO Medicao (numeroSerie, numeroSensor, dataHora, valorMedido)
VALUES ('MTR-001', 2, TIMESTAMP '2026-06-01 08:00:00', 2.1);

INSERT INTO Medicao (numeroSerie, numeroSensor, dataHora, valorMedido)
VALUES ('MTR-001', 2, TIMESTAMP '2026-06-01 09:00:00', 5.7);

INSERT INTO Medicao (numeroSerie, numeroSensor, dataHora, valorMedido)
VALUES ('CMP-002', 1, TIMESTAMP '2026-06-05 07:30:00', 8.2);

INSERT INTO Medicao (numeroSerie, numeroSensor, dataHora, valorMedido)
VALUES ('CMP-002', 1, TIMESTAMP '2026-06-05 08:30:00', 8.5);

INSERT INTO Medicao (numeroSerie, numeroSensor, dataHora, valorMedido)
VALUES ('CMP-002', 2, TIMESTAMP '2026-06-05 07:30:00', 45.2);

INSERT INTO Medicao (numeroSerie, numeroSensor, dataHora, valorMedido)
VALUES ('RBT-003', 1, TIMESTAMP '2026-06-10 06:00:00', 55.0);

INSERT INTO Medicao (numeroSerie, numeroSensor, dataHora, valorMedido)
VALUES ('EST-004', 1, TIMESTAMP '2026-06-12 09:00:00', 1.5);

-- -----------------------------------------------------------------------------
-- Tipos de Falha
-- -----------------------------------------------------------------------------
INSERT INTO TipoFalha (tipoFalha, descricao, nivelSeveridade)
VALUES ('Superaquecimento', 'Temperatura acima do limite operacional seguro', 'Alto');

INSERT INTO TipoFalha (tipoFalha, descricao, nivelSeveridade)
VALUES ('Vibracao Excessiva', 'Niveis de vibracao acima do limite aceitavel', 'Medio');

INSERT INTO TipoFalha (tipoFalha, descricao, nivelSeveridade)
VALUES ('Queda de Pressao', 'Pressao abaixo do minimo operacional', 'Alto');

INSERT INTO TipoFalha (tipoFalha, descricao, nivelSeveridade)
VALUES ('Falha Eletrica', 'Curto-circuito ou interrupcao no fornecimento eletrico', 'Critico');

INSERT INTO TipoFalha (tipoFalha, descricao, nivelSeveridade)
VALUES ('Desgaste Mecanico', 'Degradacao progressiva de componentes moveis', 'Baixo');

-- -----------------------------------------------------------------------------
-- Ocorrencias (Agregacao: Ativo x TipoFalha)
-- -----------------------------------------------------------------------------
INSERT INTO Ocorrencia (numeroSerie, tipoFalha, dataFalha, descricao)
VALUES ('MTR-001', 'Superaquecimento', DATE '2026-06-01',
        'Sensor 1 registrou temperatura de 89.3C as 10h, acima do limite de 85C');

INSERT INTO Ocorrencia (numeroSerie, tipoFalha, dataFalha, descricao)
VALUES ('MTR-001', 'Vibracao Excessiva', DATE '2026-06-01',
        'Sensor 2 registrou vibracao de 5.7mm/s, limite e 4.5mm/s');

INSERT INTO Ocorrencia (numeroSerie, tipoFalha, dataFalha, descricao)
VALUES ('CMP-002', 'Queda de Pressao', DATE '2026-06-05',
        'Pressao caiu para 6.8 bar durante operacao de pico');

INSERT INTO Ocorrencia (numeroSerie, tipoFalha, dataFalha, descricao)
VALUES ('RBT-003', 'Falha Eletrica', DATE '2026-06-10',
        'Falha no modulo de controle do servo motor 3');

-- -----------------------------------------------------------------------------
-- Tecnicos
-- -----------------------------------------------------------------------------
INSERT INTO Tecnico (CPF, nome, especialidade, telefone, endereco)
VALUES ('12345678901', 'Carlos Eduardo Ramos', 'Eletromecânica',
        '(16) 99812-3456', 'Rua das Maquinas, 200 - Sao Carlos, SP');

INSERT INTO Tecnico (CPF, nome, especialidade, telefone, endereco)
VALUES ('98765432100', 'Ana Paula Ferreira', 'Automacao Industrial',
        '(16) 99723-4567', 'Av. Trabalhista, 850 - Sao Carlos, SP');

INSERT INTO Tecnico (CPF, nome, especialidade, telefone, endereco)
VALUES ('11122233344', 'Marcos Vinicius Souza', 'Manutencao Preditiva',
        '(11) 99634-5678', 'Rua da Industria, 412 - Sao Paulo, SP');

INSERT INTO Tecnico (CPF, nome, especialidade, telefone, endereco)
VALUES ('55566677788', 'Juliana Martins Costa', 'Sistemas Hidraulicos',
        '(19) 99545-6789', 'Av. Bandeirantes, 1100 - Campinas, SP');

-- -----------------------------------------------------------------------------
-- Manutencoes (superclasse)
-- Nota: idManutencao gerado pela sequencia seq_manutencao_id
-- -----------------------------------------------------------------------------

-- Manutencao 1: Corretiva no MTR-001 (superaquecimento)
INSERT INTO Manutencao (idManutencao, numeroSerie, dataInicio, dataFim,
                        descricao, tipoManutencao)
VALUES (seq_manutencao_id.NEXTVAL, 'MTR-001', DATE '2026-06-02', DATE '2026-06-02',
        'Substituicao do rolamento danificado e limpeza dos dissipadores de calor',
        'Corretiva');

-- Manutencao 2: Preventiva no CMP-002
INSERT INTO Manutencao (idManutencao, numeroSerie, dataInicio, dataFim,
                        descricao, tipoManutencao)
VALUES (seq_manutencao_id.NEXTVAL, 'CMP-002', DATE '2026-05-15', DATE '2026-05-15',
        'Revisao periodica: troca de filtros, lubrificacao e verificacao de pressao',
        'Preventiva');

-- Manutencao 3: Preditiva no MTR-001 (baseada em medicoes de vibracao)
INSERT INTO Manutencao (idManutencao, numeroSerie, dataInicio, dataFim,
                        descricao, tipoManutencao)
VALUES (seq_manutencao_id.NEXTVAL, 'MTR-001', DATE '2026-06-08', DATE '2026-06-08',
        'Balanceamento do rotor com base em analise de tendencia de vibracao',
        'Preditiva');

-- Manutencao 4: Corretiva no RBT-003 (falha eletrica)
INSERT INTO Manutencao (idManutencao, numeroSerie, dataInicio, dataFim,
                        descricao, tipoManutencao)
VALUES (seq_manutencao_id.NEXTVAL, 'RBT-003', DATE '2026-06-10', DATE '2026-06-12',
        'Substituicao do modulo de controle defeituoso do servo motor 3',
        'Corretiva');

-- Manutencao 5: Prescritiva no EST-004
INSERT INTO Manutencao (idManutencao, numeroSerie, dataInicio, dataFim,
                        descricao, tipoManutencao)
VALUES (seq_manutencao_id.NEXTVAL, 'EST-004', DATE '2026-06-14', DATE '2026-06-14',
        'Ajuste de tensao da correia conforme prescricao do sistema de monitoramento',
        'Prescritiva');

-- Manutencao 6: Preventiva no RBT-003 (agendada antes da falha eletrica)
INSERT INTO Manutencao (idManutencao, numeroSerie, dataInicio, dataFim,
                        descricao, tipoManutencao)
VALUES (seq_manutencao_id.NEXTVAL, 'RBT-003', DATE '2026-03-10', DATE '2026-03-10',
        'Revisao semestral: verificacao de cabos, conexoes e calibracao dos sensores',
        'Preventiva');

-- -----------------------------------------------------------------------------
-- Subtipos de Manutencao
-- Nota: os idManutencao correspondem aos valores inseridos acima (1 a 6)
-- -----------------------------------------------------------------------------

-- Corretiva 1: resolve ocorrencia de superaquecimento no MTR-001
INSERT INTO Corretiva (idManutencao, causaFalha,
                       ocorr_numeroSerie, ocorr_tipoFalha, ocorr_dataFalha)
VALUES (1, 'Rolamento deteriorado causou atrito excessivo e superaquecimento',
        'MTR-001', 'Superaquecimento', DATE '2026-06-01');

-- Corretiva 4: resolve falha eletrica no RBT-003
INSERT INTO Corretiva (idManutencao, causaFalha,
                       ocorr_numeroSerie, ocorr_tipoFalha, ocorr_dataFalha)
VALUES (4, 'Pico de tensao danificou o modulo de controle do servo',
        'RBT-003', 'Falha Eletrica', DATE '2026-06-10');

-- Preventiva 2: revisao periodica no CMP-002 (a cada 90 dias)
INSERT INTO Preventiva (idManutencao, periodicidade)
VALUES (2, 90);

-- Preventiva 6: revisao semestral do RBT-003 (a cada 180 dias)
INSERT INTO Preventiva (idManutencao, periodicidade)
VALUES (6, 180);

-- Preditiva 3: baseada em analise de vibracao do MTR-001
INSERT INTO Preditiva (idManutencao)
VALUES (3);

-- Prescritiva 5: recomendacao do sistema para a EST-004
INSERT INTO Prescritiva (idManutencao, recomendacao)
VALUES (5, 'Tensao da correia GT3 HTD fora do intervalo recomendado (130-150N). '
        || 'Ajustar para 145N e verificar alinhamento das polias.');

-- -----------------------------------------------------------------------------
-- Vinculo Preditiva x Medicoes (PreditivaMedicao)
-- Manutencao preditiva 3 usou medicoes de vibracao do MTR-001
-- -----------------------------------------------------------------------------
INSERT INTO PreditivaMedicao (idManutencao, med_numeroSerie, med_numSensor, med_dataHora)
VALUES (3, 'MTR-001', 2, TIMESTAMP '2026-06-01 08:00:00');

INSERT INTO PreditivaMedicao (idManutencao, med_numeroSerie, med_numSensor, med_dataHora)
VALUES (3, 'MTR-001', 2, TIMESTAMP '2026-06-01 09:00:00');

-- -----------------------------------------------------------------------------
-- Realiza (Tecnico x Manutencao)
-- -----------------------------------------------------------------------------
INSERT INTO Realiza (CPF_tecnico, idManutencao) VALUES ('12345678901', 1);
INSERT INTO Realiza (CPF_tecnico, idManutencao) VALUES ('98765432100', 1);
INSERT INTO Realiza (CPF_tecnico, idManutencao) VALUES ('55566677788', 2);
INSERT INTO Realiza (CPF_tecnico, idManutencao) VALUES ('11122233344', 3);
INSERT INTO Realiza (CPF_tecnico, idManutencao) VALUES ('12345678901', 3);
INSERT INTO Realiza (CPF_tecnico, idManutencao) VALUES ('98765432100', 4);
INSERT INTO Realiza (CPF_tecnico, idManutencao) VALUES ('55566677788', 5);
INSERT INTO Realiza (CPF_tecnico, idManutencao) VALUES ('12345678901', 6);
INSERT INTO Realiza (CPF_tecnico, idManutencao) VALUES ('98765432100', 6);

-- -----------------------------------------------------------------------------
-- UtilizaPeca (Manutencao x Peca)
-- -----------------------------------------------------------------------------
-- Corretiva no MTR-001 usou rolamento e veda-cao
INSERT INTO UtilizaPeca (idManutencao, nome_peca, quantidadeUtilizada)
VALUES (1, 'Rolamento SKF 6205', 2);

INSERT INTO UtilizaPeca (idManutencao, nome_peca, quantidadeUtilizada)
VALUES (1, 'Veda-cao mecanica 20mm', 1);

-- Preventiva no CMP-002 usou filtro e veda-cao
INSERT INTO UtilizaPeca (idManutencao, nome_peca, quantidadeUtilizada)
VALUES (2, 'Filtro de oleo hidraulico HF35', 1);

INSERT INTO UtilizaPeca (idManutencao, nome_peca, quantidadeUtilizada)
VALUES (2, 'Veda-cao mecanica 20mm', 1);

-- Prescritiva no EST-004 usou correia
INSERT INTO UtilizaPeca (idManutencao, nome_peca, quantidadeUtilizada)
VALUES (5, 'Correia dentada GT3 HTD', 1);

-- -----------------------------------------------------------------------------
-- Descartes (entidade fraca de Ativo)
-- Note 8: dataDescarte posterior a dataInstalacao do ativo
-- -----------------------------------------------------------------------------
INSERT INTO Descarte (numeroSerie, dataDescarte, metodoDescarte, empresaResponsavel)
VALUES ('TRF-005', DATE '2025-08-20',
        'Desmontagem e reciclagem de componentes metalicos',
        'EcoRecicla Solucoes Ltda');

INSERT INTO Descarte (numeroSerie, dataDescarte, metodoDescarte, empresaResponsavel)
VALUES ('TRF-005', DATE '2026-01-10',
        'Destinacao de residuos quimicos (oleo isolante) para descarte especializado',
        'ResidQuim Ambiental S/A');

COMMIT;

-- =============================================================================
-- FIM DO SCRIPT dados.sql
-- =============================================================================
