# =============================================================================
# PRESERVE5.0 – Sistema de Manutenção Prescritiva e Gestão Sustentável
#               de Ativos Industriais 5.0
# =============================================================================
# Arquivo: db.py
# Descrição: Módulo de conexão com o banco Oracle.
#            Utiliza oracledb (thin mode) com prepared statements para
#            prevenir SQL Injection (OBS 6 do enunciado).
#            Credenciais lidas de variáveis de ambiente (arquivo .env).
# =============================================================================

import os
import oracledb
from dotenv import load_dotenv

load_dotenv()  # Carrega variáveis do arquivo .env

def get_connection():
    """
    Cria e retorna uma conexão com o Oracle usando as credenciais
    definidas nas variáveis de ambiente.
    Lança ConnectionError com mensagem amigável se falhar.
    """
    try:
        conn = oracledb.connect(
            user     = os.environ["DB_USER"],
            password = os.environ["DB_PASSWORD"],
            dsn      = os.environ["DB_DSN"],   # ex: "host:porta/service_name"
        )
        return conn
    except oracledb.DatabaseError as e:
        raise ConnectionError(
            f"Não foi possível conectar ao banco de dados.\n"
            f"Verifique as credenciais no arquivo .env\n"
            f"Detalhe técnico: {e}"
        )
    except KeyError as e:
        raise ConnectionError(
            f"Variável de ambiente {e} não definida.\n"
            f"Configure o arquivo .env com DB_USER, DB_PASSWORD e DB_DSN."
        )
