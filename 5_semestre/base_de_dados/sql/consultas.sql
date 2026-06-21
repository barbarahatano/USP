-- =============================================================================
-- PRESERVE5.0 -- Sistema de Manutencao Prescritiva e Gestao Sustentavel
--              de Ativos Industriais 5.0
-- =============================================================================
-- Script:  consultas.sql
-- Descricao: 6 consultas SQL de complexidade media e alta, cobrindo junções
--            internas e externas, agrupamentos, subconsultas correlacionadas
--            e não correlacionadas, e divisao relacional.
-- SGBD:    Oracle
-- Executar APOS dados.sql
-- OBS: As consultas com &v_serie, &v_data_ini e &v_data_fim usam variaveis de
--      substituicao do SQL Developer. Ao executar, uma janela pedira o valor.
--      Para executar uma consulta especifica: selecione o texto e pressione F9.
--      Para executar o script inteiro: F5 (sera pedido cada parametro).
-- =============================================================================


-- =============================================================================
-- CONSULTA 1: Historico completo de manutencoes de um ativo
-- =============================================================================
-- Contexto: Permite ao gestor auditar todas as intervencoes realizadas
--           em um equipamento especifico, com tipo, datas e tecnicos envolvidos.
-- Tecnicas: JOIN multiplo (Ativo, Manutencao, Realiza, Tecnico),
--           LISTAGG para agregar nomes de tecnicos (agrupamento),
--           subquery para contagem de pecas usadas.
--           Parametro: numeroSerie do ativo (substituir :v_serie).
-- =============================================================================
SELECT
    m.idManutencao,
    m.tipoManutencao,
    m.dataInicio,
    m.dataFim,
    (m.dataFim - m.dataInicio)      AS duracao_dias,
    m.descricao,
    LISTAGG(t.nome, ', ')
        WITHIN GROUP (ORDER BY t.nome) AS tecnicos,
    (SELECT COUNT(*)
     FROM   UtilizaPeca up2
     WHERE  up2.idManutencao = m.idManutencao) AS qtd_tipos_peca
FROM  Manutencao m
JOIN  Realiza  r ON r.idManutencao = m.idManutencao
JOIN  Tecnico  t ON t.CPF = r.CPF_tecnico
WHERE m.numeroSerie = '&v_serie'        -- SQL Developer pedira o valor (ex: MTR-001)
GROUP BY
    m.idManutencao, m.tipoManutencao,
    m.dataInicio, m.dataFim, m.descricao
ORDER BY m.dataInicio DESC;


-- =============================================================================
-- CONSULTA 2: DIVISAO RELACIONAL
--             Tecnicos que participaram de TODAS as manutencoes de um ativo
-- =============================================================================
-- Contexto: Identifica profissionais com experiencia total em um equipamento
--           especifico, util para designar responsaveis por grandes revisoes.
-- Tecnica:  Divisao relacional implementada via NOT EXISTS duplo (padrao SQL).
--           "Tecnicos T tais que nao existe manutencao do ativo A para a qual
--            T nao tenha participado."
-- Parametro: numeroSerie do ativo (substituir :v_serie).
-- =============================================================================
-- &&v_serie: o && define o valor uma unica vez na sessao (evita perguntar duas vezes)
SELECT
    t.CPF,
    t.nome,
    t.especialidade
FROM Tecnico t
WHERE NOT EXISTS (
    -- manutencoes do ativo que este tecnico NAO realizou
    SELECT 1
    FROM   Manutencao m
    WHERE  m.numeroSerie = '&&v_serie'
    AND    NOT EXISTS (
        SELECT 1
        FROM   Realiza r
        WHERE  r.CPF_tecnico  = t.CPF
        AND    r.idManutencao = m.idManutencao
    )
)
-- Garante que o tecnico participou de ao menos uma manutencao do ativo
AND EXISTS (
    SELECT 1
    FROM   Realiza   r2
    JOIN   Manutencao m2 ON m2.idManutencao = r2.idManutencao
    WHERE  r2.CPF_tecnico = t.CPF
    AND    m2.numeroSerie = '&&v_serie'
)
ORDER BY t.nome;
-- Limpa a definicao para que a proxima execucao peca novamente o valor
UNDEFINE v_serie


-- =============================================================================
-- CONSULTA 3: Ranking de pecas mais utilizadas com custo total
-- =============================================================================
-- Contexto: Auxilia o gestor de estoque a priorizar o ressuprimento dos
--           componentes mais demandados e a calcular o custo de manutencao.
-- Tecnicas: JOIN entre UtilizaPeca, Peca e Compra (LEFT JOIN para pecas sem
--           historico de compra), GROUP BY com SUM e COUNT,
--           subconsulta correlacionada para custo medio unitario,
--           ORDER BY com funcao analitica RANK().
-- =============================================================================
SELECT
    p.nome                              AS peca,
    p.fabricante,
    SUM(up.quantidadeUtilizada)         AS total_utilizado,
    COUNT(DISTINCT up.idManutencao)     AS em_quantas_manutencoes,
    ROUND(
        (SELECT AVG(c.custo / c.quantidade)
         FROM   Compra c
         WHERE  c.nome_peca = p.nome), 2
    )                                   AS custo_medio_unitario,
    RANK() OVER (
        ORDER BY SUM(up.quantidadeUtilizada) DESC
    )                                   AS ranking
FROM  Peca       p
JOIN  UtilizaPeca up ON up.nome_peca = p.nome
GROUP BY p.nome, p.fabricante
ORDER BY total_utilizado DESC;


-- =============================================================================
-- CONSULTA 4: Ativos sem nenhuma manutencao registrada (LEFT JOIN / NOT EXISTS)
-- =============================================================================
-- Contexto: Detecta equipamentos que podem estar sendo negligenciados, sem
--           nenhuma intervencao tecnica registrada, fundamental para auditoria.
-- Tecnicas: LEFT JOIN com IS NULL (alternativa NOT EXISTS comentada abaixo),
--           adicional: exibe data de instalacao e tempo em operacao sem manutencao.
-- =============================================================================
SELECT
    a.numeroSerie,
    a.nome,
    a.tipo,
    a.localizacao,
    a.dataInstalacao,
    a.statusOperacional,
    TRUNC(SYSDATE - a.dataInstalacao) AS dias_desde_instalacao
FROM  Ativo     a
LEFT JOIN Manutencao m ON m.numeroSerie = a.numeroSerie
WHERE m.idManutencao IS NULL
ORDER BY a.dataInstalacao;

-- Alternativa equivalente com NOT EXISTS (subconsulta nao correlacionada):
-- SELECT a.* FROM Ativo a
-- WHERE NOT EXISTS (SELECT 1 FROM Manutencao m WHERE m.numeroSerie = a.numeroSerie);


-- =============================================================================
-- CONSULTA 5: Custo total de compras por fornecedor em um intervalo de datas
-- =============================================================================
-- Contexto: Relatorio financeiro da cadeia de suprimentos para auditoria
--           de gastos com fornecedores em um periodo especifico.
-- Tecnicas: JOIN Fornecedor + Compra, GROUP BY, HAVING para filtrar
--           fornecedores com gasto significativo, funcao analitica RATIO_TO_REPORT
--           para percentual em relacao ao total geral.
-- Parametro: intervalo de datas (substituir :v_data_ini e :v_data_fim).
-- =============================================================================
SELECT
    f.CNPJ,
    f.nome                              AS fornecedor,
    COUNT(c.dataCompra)                 AS num_transacoes,
    SUM(c.quantidade)                   AS total_itens_comprados,
    SUM(c.custo)                        AS custo_total,
    ROUND(
        RATIO_TO_REPORT(SUM(c.custo)) OVER () * 100, 2
    )                                   AS pct_do_total
FROM  Fornecedor f
JOIN  Compra     c ON c.CNPJ_forn = f.CNPJ
WHERE c.dataCompra BETWEEN TO_DATE('&v_data_ini','DD/MM/YYYY')
                       AND TO_DATE('&v_data_fim','DD/MM/YYYY')
GROUP BY f.CNPJ, f.nome
HAVING SUM(c.custo) > 0
ORDER BY custo_total DESC;


-- =============================================================================
-- CONSULTA 6: Manutencoes corretivas provocadas por falhas de alta severidade,
--             com rastreabilidade completa (ativo -> ocorrencia -> manutencao)
-- =============================================================================
-- Contexto: Relatório de rastreabilidade para auditorias de seguranca e ESG.
--           Mostra a cadeia completa: falha critica -> ocorrencia -> corretiva
--           -> tecnicos responsaveis -> pecas utilizadas.
-- Tecnicas: JOIN multiplo (Manutencao, Corretiva, Ocorrencia, TipoFalha, Ativo),
--           subconsulta correlacionada para listar tecnicos,
--           subconsulta correlacionada para listar pecas usadas,
--           filtro em atributo de subclasse (nivelSeveridade).
-- =============================================================================
SELECT
    a.numeroSerie,
    a.nome                              AS nome_ativo,
    a.localizacao,
    tf.tipoFalha,
    tf.nivelSeveridade,
    o.dataFalha,
    o.descricao                         AS descricao_ocorrencia,
    m.idManutencao,
    m.dataInicio                        AS inicio_manutencao,
    m.dataFim                           AS fim_manutencao,
    cor.causaFalha,
    -- Subconsulta correlacionada: nomes dos tecnicos da manutencao
    (SELECT LISTAGG(t2.nome, '; ')
            WITHIN GROUP (ORDER BY t2.nome)
     FROM   Realiza r2
     JOIN   Tecnico t2 ON t2.CPF = r2.CPF_tecnico
     WHERE  r2.idManutencao = m.idManutencao
    )                                   AS tecnicos_responsaveis,
    -- Subconsulta correlacionada: pecas utilizadas
    (SELECT LISTAGG(up2.nome_peca || ' (x' || up2.quantidadeUtilizada || ')', '; ')
            WITHIN GROUP (ORDER BY up2.nome_peca)
     FROM   UtilizaPeca up2
     WHERE  up2.idManutencao = m.idManutencao
    )                                   AS pecas_utilizadas
FROM  Manutencao m
JOIN  Corretiva  cor ON cor.idManutencao         = m.idManutencao
JOIN  Ocorrencia o   ON o.numeroSerie            = cor.ocorr_numeroSerie
                     AND o.tipoFalha             = cor.ocorr_tipoFalha
                     AND o.dataFalha             = cor.ocorr_dataFalha
JOIN  TipoFalha  tf  ON tf.tipoFalha             = o.tipoFalha
JOIN  Ativo      a   ON a.numeroSerie            = m.numeroSerie
WHERE tf.nivelSeveridade IN ('Alto', 'Critico')
ORDER BY o.dataFalha DESC, tf.nivelSeveridade;


-- =============================================================================
-- FIM DO SCRIPT consultas.sql
-- =============================================================================
