#!/usr/bin/env python3
# =============================================================================
# PRESERVE5.0 – Sistema de Manutenção Prescritiva e Gestão Sustentável
#               de Ativos Industriais 5.0
# =============================================================================
# Arquivo: app.py
# Descrição: Protótipo operacional com interface de linha de comando (CLI).
#            Implementa:
#              (a) Cadastro de dados: registro de Ativo, Técnico, Manutenção
#              (b) Consulta parametrizada: histórico de manutenções de um ativo
#            Todas as operações usam SQL explícito com parâmetros bind (OBS 3/6).
#            Controle transacional com COMMIT/ROLLBACK em erros (OBS 6).
# Linguagem: Python 3.8+
# Dependências: oracledb, python-dotenv  (ver requirements.txt)
# =============================================================================

import sys
import os
import oracledb
from datetime import date, datetime

# Adiciona o diretório pai ao path para importar db.py
sys.path.insert(0, os.path.dirname(__file__))
from db import get_connection

# ─────────────────────────────────────────────────────────────────────────────
# Utilitários de UI
# ─────────────────────────────────────────────────────────────────────────────

def cls():
    """Limpa a tela."""
    os.system('cls' if os.name == 'nt' else 'clear')

def linha(char='─', largura=60):
    print(char * largura)

def titulo(texto):
    cls()
    linha('═')
    print(f"  PRESERVE5.0  │  {texto}")
    linha('═')
    print()

def erro(msg):
    print(f"\n  ❌  {msg}")

def sucesso(msg):
    print(f"\n  ✅  {msg}")

def aviso(msg):
    print(f"\n  ⚠️   {msg}")

def ler_texto(prompt, obrigatorio=True, max_len=None):
    """Lê string do usuário com validação básica."""
    while True:
        valor = input(f"  {prompt}: ").strip()
        if not valor and obrigatorio:
            aviso("Este campo é obrigatório.")
            continue
        if max_len and len(valor) > max_len:
            aviso(f"Máximo de {max_len} caracteres.")
            continue
        return valor

def ler_data(prompt, obrigatorio=True):
    """Lê uma data no formato DD/MM/AAAA."""
    while True:
        s = input(f"  {prompt} (DD/MM/AAAA): ").strip()
        if not s and not obrigatorio:
            return None
        try:
            return datetime.strptime(s, "%d/%m/%Y").date()
        except ValueError:
            aviso("Data inválida. Use o formato DD/MM/AAAA.")

def ler_int(prompt, minimo=None, maximo=None, obrigatorio=True):
    """Lê um inteiro do usuário."""
    while True:
        s = input(f"  {prompt}: ").strip()
        if not s and not obrigatorio:
            return None
        try:
            v = int(s)
            if minimo is not None and v < minimo:
                aviso(f"Valor mínimo: {minimo}.")
                continue
            if maximo is not None and v > maximo:
                aviso(f"Valor máximo: {maximo}.")
                continue
            return v
        except ValueError:
            aviso("Por favor, informe um número inteiro.")

def ler_opcao(opcoes):
    """Lê uma opção de menu (número)."""
    while True:
        s = input("\n  Escolha uma opção: ").strip()
        if s.isdigit() and int(s) in opcoes:
            return int(s)
        aviso(f"Opção inválida. Escolha entre: {', '.join(str(o) for o in opcoes)}")

def pausar():
    input("\n  Pressione ENTER para continuar...")

# ─────────────────────────────────────────────────────────────────────────────
# MÓDULO A: CADASTROS
# ─────────────────────────────────────────────────────────────────────────────

def cadastrar_ativo(conn):
    """
    Cadastra um novo ativo industrial.
    SQL: INSERT INTO Ativo — usa bind variables para prevenir SQL Injection.
    Controle transacional: ROLLBACK automático em caso de erro do SGBD.
    """
    titulo("Cadastro de Ativo Industrial")
    print("  Preencha os dados do equipamento:\n")

    numero_serie = ler_texto("Número de série (único)", max_len=50)
    nome         = ler_texto("Nome do equipamento",     max_len=100)
    tipo         = ler_texto("Tipo (ex: Motor, Compressor, Robô)", max_len=50)
    fabricante   = ler_texto("Fabricante", max_len=100)
    modelo       = ler_texto("Modelo",     max_len=100)
    data_inst    = ler_data("Data de instalação")
    localizacao  = ler_texto("Localização na planta", max_len=200)

    print("\n  Status operacional:")
    print("    1. Ativo")
    print("    2. Em manutenção")
    print("    3. Desativado")
    op = ler_opcao([1, 2, 3])
    status_map = {1: "Ativo", 2: "Em manutencao", 3: "Desativado"}
    status = status_map[op]

    # Confirmação antes de gravar
    print()
    linha()
    print(f"  Número de Série : {numero_serie}")
    print(f"  Nome            : {nome}")
    print(f"  Tipo            : {tipo}")
    print(f"  Fabricante/Mod. : {fabricante} / {modelo}")
    print(f"  Instalação      : {data_inst.strftime('%d/%m/%Y')}")
    print(f"  Localização     : {localizacao}")
    print(f"  Status          : {status}")
    linha()
    confirmar = input("\n  Confirmar cadastro? (S/N): ").strip().upper()
    if confirmar != "S":
        aviso("Cadastro cancelado.")
        pausar()
        return

    # ── SQL com bind variables (previne SQL Injection – OBS 6) ──
    sql = """
        INSERT INTO Ativo
            (numeroSerie, nome, tipo, fabricante, modelo,
             dataInstalacao, localizacao, statusOperacional)
        VALUES
            (:1, :2, :3, :4, :5, :6, :7, :8)
    """

    cursor = conn.cursor()
    try:
        cursor.execute(sql, [
            numero_serie, nome, tipo, fabricante, modelo,
            data_inst, localizacao, status
        ])
        conn.commit()  # Controle transacional explícito (OBS 6)
        sucesso(f"Ativo '{nome}' cadastrado com sucesso!")
    except oracledb.IntegrityError as e:
        conn.rollback()
        if "PK_ATIVO" in str(e).upper() or "UNIQUE" in str(e).upper():
            erro(f"Já existe um ativo com o número de série '{numero_serie}'.")
        else:
            erro(f"Erro de integridade: {e}")
    except oracledb.DatabaseError as e:
        conn.rollback()
        erro(f"Erro no banco de dados: {e}")
    finally:
        cursor.close()
    pausar()


def cadastrar_tecnico(conn):
    """
    Cadastra um novo técnico de manutenção.
    SQL: INSERT INTO Tecnico — usa bind variables.
    """
    titulo("Cadastro de Técnico")
    print("  Preencha os dados do profissional:\n")

    cpf = ler_texto("CPF (somente números, 11 dígitos)", max_len=11)
    # Validação básica de CPF (formato)
    if not cpf.isdigit() or len(cpf) != 11:
        erro("CPF inválido. Informe 11 dígitos numéricos.")
        pausar()
        return

    nome        = ler_texto("Nome completo", max_len=100)
    especialidade = ler_texto("Especialidade (ex: Eletromecânica)", max_len=100)
    telefone    = ler_texto("Telefone", max_len=20)
    endereco    = ler_texto("Endereço completo", max_len=200)

    sql = """
        INSERT INTO Tecnico (CPF, nome, especialidade, telefone, endereco)
        VALUES (:1, :2, :3, :4, :5)
    """

    cursor = conn.cursor()
    try:
        cursor.execute(sql, [cpf, nome, especialidade, telefone, endereco])
        conn.commit()
        sucesso(f"Técnico '{nome}' cadastrado com sucesso!")
    except oracledb.IntegrityError:
        conn.rollback()
        erro(f"Já existe um técnico com o CPF '{cpf}'.")
    except oracledb.DatabaseError as e:
        conn.rollback()
        erro(f"Erro no banco de dados: {e}")
    finally:
        cursor.close()
    pausar()


def registrar_manutencao(conn):
    """
    Registra uma nova manutenção em um ativo existente.
    Insere em Manutencao + subtipo escolhido + técnico responsável.
    Usa transação explícita: todas as inserções num único bloco COMMIT/ROLLBACK.
    SQL: INSERT em Manutencao, subtipo e Realiza.
    Localização no código: função registrar_manutencao() – app.py
    """
    titulo("Registrar Manutenção")
    print("  Informe os dados da manutenção:\n")

    numero_serie = ler_texto("Número de série do ativo", max_len=50)

    # Verifica se o ativo existe (consulta parametrizada)
    cursor = conn.cursor()
    cursor.execute(
        "SELECT nome, statusOperacional FROM Ativo WHERE numeroSerie = :1",
        [numero_serie]
    )
    row = cursor.fetchone()
    cursor.close()

    if not row:
        erro(f"Ativo '{numero_serie}' não encontrado no sistema.")
        pausar()
        return

    nome_ativo, status_ativo = row
    print(f"\n  Ativo encontrado: {nome_ativo} (Status: {status_ativo})")

    print("\n  Tipo de manutenção:")
    print("    1. Corretiva")
    print("    2. Preventiva")
    print("    3. Preditiva")
    print("    4. Prescritiva")
    tipo_op = ler_opcao([1, 2, 3, 4])
    tipos = {1: "Corretiva", 2: "Preventiva", 3: "Preditiva", 4: "Prescritiva"}
    tipo_manut = tipos[tipo_op]

    data_inicio = ler_data("Data de início")
    data_fim    = ler_data("Data de conclusão (deixe em branco se ainda em andamento)", obrigatorio=False)
    descricao   = ler_texto("Descrição das atividades", max_len=1000)

    # Dados específicos por subtipo
    causa_falha     = None
    ocorr_serie     = None
    ocorr_falha     = None
    ocorr_data      = None
    periodicidade   = None
    recomendacao    = None

    if tipo_op == 1:  # Corretiva
        print("\n  Dados da ocorrência de falha associada:")
        ocorr_serie = ler_texto("  Número de série do ativo da ocorrência", max_len=50)
        ocorr_falha = ler_texto("  Tipo de falha (ex: Superaquecimento)", max_len=100)
        ocorr_data  = ler_data("  Data da falha")
        causa_falha = ler_texto("  Causa identificada da falha", max_len=500)

    elif tipo_op == 2:  # Preventiva
        periodicidade = ler_int(
            "Periodicidade em dias (ex: 90 para trimestral)", minimo=1, obrigatorio=False
        )

    elif tipo_op == 4:  # Prescritiva
        recomendacao = ler_texto("Recomendação do sistema/especialista", max_len=1000)

    cpf_tecnico = ler_texto("\n  CPF do técnico responsável (11 dígitos)", max_len=11)

    # Verifica técnico
    cursor = conn.cursor()
    cursor.execute("SELECT nome FROM Tecnico WHERE CPF = :1", [cpf_tecnico])
    tec_row = cursor.fetchone()
    cursor.close()

    if not tec_row:
        erro(f"Técnico com CPF '{cpf_tecnico}' não encontrado.")
        pausar()
        return

    print(f"  Técnico encontrado: {tec_row[0]}")

    # ── Início da transação ──
    cursor = conn.cursor()
    try:
        # 1. Gera próximo ID da sequência
        cursor.execute("SELECT seq_manutencao_id.NEXTVAL FROM DUAL")
        id_manut = cursor.fetchone()[0]

        # 2. Insere na superclasse Manutencao
        cursor.execute("""
            INSERT INTO Manutencao
                (idManutencao, numeroSerie, dataInicio, dataFim, descricao, tipoManutencao)
            VALUES (:1, :2, :3, :4, :5, :6)
        """, [id_manut, numero_serie, data_inicio, data_fim, descricao, tipo_manut])

        # 3. Insere no subtipo correspondente
        if tipo_op == 1:
            cursor.execute("""
                INSERT INTO Corretiva
                    (idManutencao, causaFalha, ocorr_numeroSerie, ocorr_tipoFalha, ocorr_dataFalha)
                VALUES (:1, :2, :3, :4, :5)
            """, [id_manut, causa_falha, ocorr_serie, ocorr_falha, ocorr_data])

        elif tipo_op == 2:
            cursor.execute(
                "INSERT INTO Preventiva (idManutencao, periodicidade) VALUES (:1, :2)",
                [id_manut, periodicidade]
            )

        elif tipo_op == 3:
            cursor.execute(
                "INSERT INTO Preditiva (idManutencao) VALUES (:1)",
                [id_manut]
            )

        elif tipo_op == 4:
            cursor.execute(
                "INSERT INTO Prescritiva (idManutencao, recomendacao) VALUES (:1, :2)",
                [id_manut, recomendacao]
            )

        # 4. Associa técnico (tabela Realiza)
        cursor.execute(
            "INSERT INTO Realiza (CPF_tecnico, idManutencao) VALUES (:1, :2)",
            [cpf_tecnico, id_manut]
        )

        conn.commit()  # Confirma toda a transação
        sucesso(
            f"Manutenção {tipo_manut} registrada com sucesso!\n"
            f"  ID: {id_manut}  |  Ativo: {nome_ativo}"
        )

    except oracledb.IntegrityError as e:
        conn.rollback()
        if "FK_CORR_OCORR" in str(e).upper():
            erro("Ocorrência de falha não encontrada. Verifique os dados informados.")
        elif "UQ_MANUT_NAT" in str(e).upper():
            erro(f"Já existe uma manutenção para '{numero_serie}' na data {data_inicio}.")
        elif "UQ_CORR_OCORR" in str(e).upper():
            erro("Essa ocorrência já possui uma manutenção corretiva associada.")
        else:
            erro(f"Erro de integridade: {e}")
    except oracledb.DatabaseError as e:
        conn.rollback()
        erro(f"Erro no banco de dados: {e}")
    finally:
        cursor.close()
    pausar()

# ─────────────────────────────────────────────────────────────────────────────
# MÓDULO B: CONSULTAS PARAMETRIZADAS
# ─────────────────────────────────────────────────────────────────────────────

def consultar_historico_ativo(conn):
    """
    Consulta 1: Histórico completo de manutenções de um ativo.
    O usuário informa o número de série; o sistema exibe todas as manutenções,
    com tipo, datas, duração, técnicos e quantidade de peças usadas.
    SQL explícito com bind variable (previne SQL Injection – OBS 6).
    Localização: função consultar_historico_ativo() – app.py
    """
    titulo("Consulta: Histórico de Manutenções")
    numero_serie = ler_texto("Número de série do ativo", max_len=50)

    # Verifica ativo
    cursor = conn.cursor()
    cursor.execute(
        "SELECT nome, tipo, statusOperacional FROM Ativo WHERE numeroSerie = :1",
        [numero_serie]
    )
    ativo = cursor.fetchone()
    cursor.close()

    if not ativo:
        erro(f"Ativo '{numero_serie}' não encontrado.")
        pausar()
        return

    print(f"\n  Equipamento: {ativo[0]}  |  Tipo: {ativo[1]}  |  Status: {ativo[2]}")
    linha()

    sql = """
        SELECT
            m.idManutencao,
            m.tipoManutencao,
            TO_CHAR(m.dataInicio, 'DD/MM/YYYY')   AS inicio,
            TO_CHAR(m.dataFim,    'DD/MM/YYYY')   AS fim,
            (m.dataFim - m.dataInicio)             AS duracao_dias,
            LISTAGG(t.nome, ', ')
                WITHIN GROUP (ORDER BY t.nome)     AS tecnicos,
            (SELECT COUNT(*)
             FROM   UtilizaPeca up2
             WHERE  up2.idManutencao = m.idManutencao) AS qtd_pecas
        FROM  Manutencao m
        JOIN  Realiza  r ON r.idManutencao  = m.idManutencao
        JOIN  Tecnico  t ON t.CPF           = r.CPF_tecnico
        WHERE m.numeroSerie = :1
        GROUP BY
            m.idManutencao, m.tipoManutencao,
            m.dataInicio, m.dataFim
        ORDER BY m.dataInicio DESC
    """

    cursor = conn.cursor()
    try:
        cursor.execute(sql, [numero_serie])
        rows = cursor.fetchall()

        if not rows:
            aviso("Nenhuma manutenção encontrada para este ativo.")
        else:
            print(f"  Total de manutenções: {len(rows)}\n")
            for r in rows:
                id_m, tipo, ini, fim, dur, tecs, qtd_p = r
                print(f"  ┌── Manutenção #{id_m}  [{tipo}]")
                print(f"  │   Início : {ini}   Fim: {fim or 'Em andamento'}", end="")
                print(f"   ({dur} dias)" if dur is not None else "")
                print(f"  │   Técnicos : {tecs}")
                print(f"  │   Peças utilizadas: {qtd_p} tipo(s)")
                print(f"  └{'─'*50}")
                print()

    except oracledb.DatabaseError as e:
        erro(f"Erro na consulta: {e}")
    finally:
        cursor.close()
    pausar()


def consultar_divisao_relacional(conn):
    """
    Consulta 2 (DIVISÃO RELACIONAL): Técnicos que participaram de TODAS
    as manutenções de um ativo específico.
    Implementada via NOT EXISTS duplo (padrão clássico de divisão relacional).
    Localização: função consultar_divisao_relacional() – app.py
    """
    titulo("Consulta: Técnicos em Todas as Manutenções")
    print("  Esta consulta usa DIVISÃO RELACIONAL para encontrar técnicos")
    print("  que participaram de TODAS as manutenções de um ativo.\n")
    numero_serie = ler_texto("Número de série do ativo", max_len=50)

    sql = """
        SELECT t.CPF, t.nome, t.especialidade
        FROM Tecnico t
        WHERE NOT EXISTS (
            SELECT 1 FROM Manutencao m
            WHERE m.numeroSerie = :1
            AND NOT EXISTS (
                SELECT 1 FROM Realiza r
                WHERE r.CPF_tecnico  = t.CPF
                AND   r.idManutencao = m.idManutencao
            )
        )
        AND EXISTS (
            SELECT 1 FROM Realiza r2
            JOIN   Manutencao m2 ON m2.idManutencao = r2.idManutencao
            WHERE  r2.CPF_tecnico = t.CPF
            AND    m2.numeroSerie = :2
        )
        ORDER BY t.nome
    """

    cursor = conn.cursor()
    try:
        cursor.execute(sql, [numero_serie, numero_serie])
        rows = cursor.fetchall()

        if not rows:
            aviso("Nenhum técnico participou de TODAS as manutenções deste ativo.")
        else:
            print(f"\n  Técnicos que realizaram todas as manutenções do ativo '{numero_serie}':\n")
            linha()
            print(f"  {'CPF':<14} {'Nome':<30} {'Especialidade'}")
            linha()
            for r in rows:
                cpf, nome, esp = r
                print(f"  {cpf:<14} {nome:<30} {esp}")
            linha()

    except oracledb.DatabaseError as e:
        erro(f"Erro na consulta: {e}")
    finally:
        cursor.close()
    pausar()


def consultar_ranking_pecas(conn):
    """
    Consulta 3: Ranking de peças mais utilizadas.
    Mostra peças ordenadas por quantidade total consumida em manutenções.
    """
    titulo("Consulta: Ranking de Peças Mais Utilizadas")

    sql = """
        SELECT
            p.nome,
            p.fabricante,
            SUM(up.quantidadeUtilizada)     AS total_usado,
            COUNT(DISTINCT up.idManutencao) AS em_manutencoes,
            ROUND(
                (SELECT AVG(c.custo / c.quantidade)
                 FROM Compra c WHERE c.nome_peca = p.nome), 2
            ) AS custo_medio_unit
        FROM Peca p
        JOIN UtilizaPeca up ON up.nome_peca = p.nome
        GROUP BY p.nome, p.fabricante
        ORDER BY total_usado DESC
    """

    cursor = conn.cursor()
    try:
        cursor.execute(sql)
        rows = cursor.fetchall()

        if not rows:
            aviso("Nenhuma peça foi utilizada ainda.")
        else:
            linha()
            print(f"  {'#':<4} {'Peça':<35} {'Qtd':<8} {'Manutenções':<14} {'Custo Médio (R$)'}")
            linha()
            for i, r in enumerate(rows, 1):
                nome, fab, qtd, em_man, custo = r
                custo_str = f"R$ {custo:.2f}" if custo else "N/A"
                print(f"  {i:<4} {nome:<35} {qtd:<8} {em_man:<14} {custo_str}")
            linha()

    except oracledb.DatabaseError as e:
        erro(f"Erro na consulta: {e}")
    finally:
        cursor.close()
    pausar()


def consultar_ativos_sem_manutencao(conn):
    """
    Consulta 4: Ativos sem nenhuma manutenção registrada.
    Usa LEFT JOIN com IS NULL.
    """
    titulo("Consulta: Ativos Sem Manutenção")

    sql = """
        SELECT
            a.numeroSerie,
            a.nome,
            a.tipo,
            a.localizacao,
            TO_CHAR(a.dataInstalacao, 'DD/MM/YYYY') AS instalado_em,
            a.statusOperacional,
            TRUNC(SYSDATE - a.dataInstalacao) AS dias_sem_manutencao
        FROM  Ativo a
        LEFT JOIN Manutencao m ON m.numeroSerie = a.numeroSerie
        WHERE m.idManutencao IS NULL
        ORDER BY a.dataInstalacao
    """

    cursor = conn.cursor()
    try:
        cursor.execute(sql)
        rows = cursor.fetchall()

        if not rows:
            sucesso("Todos os ativos possuem ao menos uma manutenção registrada.")
        else:
            print(f"  {len(rows)} ativo(s) sem nenhuma manutenção registrada:\n")
            linha()
            for r in rows:
                serie, nome, tipo, local, inst, status, dias = r
                print(f"  Série   : {serie}")
                print(f"  Nome    : {nome}  ({tipo})")
                print(f"  Local   : {local}")
                print(f"  Instalado em: {inst}  |  {dias} dias sem manutenção")
                print(f"  Status  : {status}")
                linha()

    except oracledb.DatabaseError as e:
        erro(f"Erro na consulta: {e}")
    finally:
        cursor.close()
    pausar()


def consultar_custo_fornecedores(conn):
    """
    Consulta 5: Custo total de compras por fornecedor em intervalo de datas.
    Usa RATIO_TO_REPORT para calcular percentual do total.
    """
    titulo("Consulta: Custo por Fornecedor no Período")
    data_ini = ler_data("Data inicial")
    data_fim = ler_data("Data final")

    sql = """
        SELECT
            f.CNPJ,
            f.nome,
            COUNT(c.dataCompra)       AS transacoes,
            SUM(c.quantidade)         AS itens_comprados,
            SUM(c.custo)              AS custo_total,
            ROUND(RATIO_TO_REPORT(SUM(c.custo)) OVER () * 100, 2) AS pct
        FROM  Fornecedor f
        JOIN  Compra c ON c.CNPJ_forn = f.CNPJ
        WHERE c.dataCompra BETWEEN :1 AND :2
        GROUP BY f.CNPJ, f.nome
        HAVING SUM(c.custo) > 0
        ORDER BY custo_total DESC
    """

    cursor = conn.cursor()
    try:
        cursor.execute(sql, [data_ini, data_fim])
        rows = cursor.fetchall()

        if not rows:
            aviso(f"Nenhuma compra registrada no período {data_ini} – {data_fim}.")
        else:
            total_geral = sum(r[4] for r in rows)
            print(f"\n  Período: {data_ini.strftime('%d/%m/%Y')} a {data_fim.strftime('%d/%m/%Y')}")
            print(f"  Total geral: R$ {total_geral:,.2f}\n")
            linha()
            print(f"  {'Fornecedor':<35} {'Transações':<13} {'Itens':<8} {'Total (R$)':<15} {'%'}")
            linha()
            for r in rows:
                cnpj, nome, trans, itens, custo, pct = r
                print(f"  {nome:<35} {trans:<13} {itens:<8} R$ {custo:>10,.2f}   {pct:>5.1f}%")
            linha()
            print(f"  {'TOTAL':<35} {'':13} {'':8} R$ {total_geral:>10,.2f}   100.0%")
            linha()

    except oracledb.DatabaseError as e:
        erro(f"Erro na consulta: {e}")
    finally:
        cursor.close()
    pausar()


def consultar_falhas_criticas(conn):
    """
    Consulta 6: Manutenções corretivas por falhas de severidade Alta ou Crítica.
    Exibe rastreabilidade completa: ativo → ocorrência → manutenção → técnicos.
    """
    titulo("Consulta: Rastreabilidade de Falhas Críticas")
    print("  Exibe manutenções corretivas causadas por falhas de nível Alto ou Crítico.\n")

    sql = """
        SELECT
            a.numeroSerie,
            a.nome               AS nome_ativo,
            tf.tipoFalha,
            tf.nivelSeveridade,
            TO_CHAR(o.dataFalha, 'DD/MM/YYYY') AS data_falha,
            m.idManutencao,
            TO_CHAR(m.dataInicio,'DD/MM/YYYY') AS inicio_manut,
            TO_CHAR(m.dataFim,   'DD/MM/YYYY') AS fim_manut,
            cor.causaFalha,
            (SELECT LISTAGG(t2.nome, '; ')
                    WITHIN GROUP (ORDER BY t2.nome)
             FROM   Realiza r2
             JOIN   Tecnico t2 ON t2.CPF = r2.CPF_tecnico
             WHERE  r2.idManutencao = m.idManutencao) AS tecnicos,
            (SELECT LISTAGG(up2.nome_peca||' x'||up2.quantidadeUtilizada, '; ')
                    WITHIN GROUP (ORDER BY up2.nome_peca)
             FROM   UtilizaPeca up2
             WHERE  up2.idManutencao = m.idManutencao)  AS pecas
        FROM  Manutencao m
        JOIN  Corretiva  cor ON cor.idManutencao        = m.idManutencao
        JOIN  Ocorrencia o   ON o.numeroSerie           = cor.ocorr_numeroSerie
                             AND o.tipoFalha            = cor.ocorr_tipoFalha
                             AND o.dataFalha            = cor.ocorr_dataFalha
        JOIN  TipoFalha  tf  ON tf.tipoFalha            = o.tipoFalha
        JOIN  Ativo      a   ON a.numeroSerie           = m.numeroSerie
        WHERE tf.nivelSeveridade IN ('Alto', 'Critico')
        ORDER BY o.dataFalha DESC
    """

    cursor = conn.cursor()
    try:
        cursor.execute(sql)
        rows = cursor.fetchall()

        if not rows:
            aviso("Nenhuma manutenção corretiva de falha crítica encontrada.")
        else:
            for r in rows:
                serie, nome_a, tf, sev, d_falha, id_m, ini, fim, causa, tecs, pecas = r
                linha('─')
                print(f"  Ativo     : [{serie}] {nome_a}")
                print(f"  Falha     : {tf}  [SEVERIDADE: {sev}]  em {d_falha}")
                print(f"  Causa     : {causa or 'Não informada'}")
                print(f"  Manutenção #{id_m}: {ini} → {fim or 'em andamento'}")
                print(f"  Técnicos  : {tecs or 'Não informado'}")
                print(f"  Peças     : {pecas or 'Nenhuma'}")
            linha('─')

    except oracledb.DatabaseError as e:
        erro(f"Erro na consulta: {e}")
    finally:
        cursor.close()
    pausar()

# ─────────────────────────────────────────────────────────────────────────────
# MENUS
# ─────────────────────────────────────────────────────────────────────────────

def menu_cadastros(conn):
    while True:
        titulo("Cadastros")
        print("  1. Cadastrar Ativo Industrial")
        print("  2. Cadastrar Técnico")
        print("  3. Registrar Manutenção")
        print("  0. Voltar")
        op = ler_opcao([0, 1, 2, 3])
        if op == 1:
            cadastrar_ativo(conn)
        elif op == 2:
            cadastrar_tecnico(conn)
        elif op == 3:
            registrar_manutencao(conn)
        else:
            break


def menu_consultas(conn):
    while True:
        titulo("Consultas")
        print("  1. Histórico de manutenções de um ativo")
        print("  2. Técnicos em TODAS as manutenções de um ativo  [Divisão Relacional]")
        print("  3. Ranking de peças mais utilizadas")
        print("  4. Ativos sem nenhuma manutenção registrada")
        print("  5. Custo total por fornecedor em período")
        print("  6. Rastreabilidade de falhas críticas/altas")
        print("  0. Voltar")
        op = ler_opcao([0, 1, 2, 3, 4, 5, 6])
        if op == 1:
            consultar_historico_ativo(conn)
        elif op == 2:
            consultar_divisao_relacional(conn)
        elif op == 3:
            consultar_ranking_pecas(conn)
        elif op == 4:
            consultar_ativos_sem_manutencao(conn)
        elif op == 5:
            consultar_custo_fornecedores(conn)
        elif op == 6:
            consultar_falhas_criticas(conn)
        else:
            break


def menu_principal(conn):
    while True:
        titulo("Menu Principal")
        print("  Bem-vindo ao sistema PRESERVE5.0")
        print("  Plataforma de Gestão de Ativos Industriais\n")
        print("  1. Cadastros")
        print("  2. Consultas")
        print("  0. Sair")
        op = ler_opcao([0, 1, 2])
        if op == 1:
            menu_cadastros(conn)
        elif op == 2:
            menu_consultas(conn)
        else:
            cls()
            print("\n  Encerrando o sistema. Até logo!\n")
            break

# ─────────────────────────────────────────────────────────────────────────────
# PONTO DE ENTRADA
# ─────────────────────────────────────────────────────────────────────────────

if __name__ == "__main__":
    try:
        conn = get_connection()
    except ConnectionError as e:
        print(f"\n{e}\n")
        sys.exit(1)

    try:
        menu_principal(conn)
    finally:
        conn.close()
