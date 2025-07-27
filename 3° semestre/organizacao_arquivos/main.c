#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include "arquivo.h"   
#include "busca.h"
#include "exibicao.h"
#include "funcionalidades.h"
#include "arvoreB.h"
#include "buscaArvore.h"


int main() {
    
    // Ler linha de entrada e separar argumentos
     // Ler linha de entrada e separar argumentos
     char *argv[256];
    
     int argc = 0;
 
     char linha[1024]; // Declare a buffer for the input line
     if (fgets(linha, sizeof(linha), stdin) == NULL) {
         fprintf(stderr, "Erro ao ler a linha de entrada.\n");
         return 1;
     }

     char *token = strtok(linha, " \n");
     while (token != NULL && argc < 256) {
         argv[argc++] = token;
         token = strtok(NULL, " \n");
     }

    char *arquivoEntrada = NULL;
    int funcionalidade = 0;
    if (sscanf(argv[0], "%d", &funcionalidade) != 1) {
        fprintf(stderr, "Erro ao converter funcionalidade para inteiro.\n");
        return 0;
    }
    char *arquivoSaida = NULL;
    int n_buscas =0;
    int argOffset = 2;

    if (funcionalidade == 1) {
        
        arquivoEntrada = argv[1];
        arquivoSaida = argv[2];


        // Remove leading/trailing whitespace from the file name
        while (isspace((unsigned char)*arquivoEntrada)) arquivoEntrada++;
        char *end = arquivoEntrada + strlen(arquivoEntrada) - 1;
        while (end > arquivoEntrada && isspace((unsigned char)*end)) end--;
        *(end + 1) = '\0';

        FILE *csvFile = fopen(arquivoEntrada, "r");
        if (csvFile == NULL) {
            printf("Falha no processamento do arquivo.\n");
            return 0;
        }
        
        FILE *binFile = fopen(arquivoSaida, "wb");
        if (binFile == NULL) {
            printf("Erro ao abrir o arquivo binário\n");
            fclose(csvFile);
            return 0;
        }
        Cabecalho cabecalho;
        inicializarCabecalhoStruct(&cabecalho,csvFile);
        escreverCabecalho(binFile, &cabecalho);
        // Inicializar o cabeçalho do arquivo binário
        // Exibir os primeiros 1024 bytes do arquivo binário em formato hex dump
        
        // Ler o arquivo CSV e escrever os registros no arquivo binário
        RegistroDados *registro;
        while ((registro = lerRegistroCSV(csvFile)) != NULL) {
        // Processar o registro lido (por exemplo, escrever no arquivo binário)
            if (registro != NULL) {
            // Chamar sua função para escrever o registro no arquivo binário
                if (escreverRegistro(binFile, registro, &cabecalho)) {
                    // Atualizar o cabeçalho com os dados do registro
                    cabecalho.nroRegArq++;
                    cabecalho.proxByteOffset=ftell(binFile);
                } else {
                fprintf(stderr, "Erro ao escrever registro no arquivo binário.\n");
                // Tratar erro de escrita
                }

        // Liberar a memória alocada para o registro lido
                // Liberar a memória dos campos variáveis
                free(registro->country);
                free(registro->attackType);
                free(registro->targetIndustry);
                free(registro->defenseMechanism);
                // Liberar o registro em si 
                free(registro);
            }   
        }
        // Atualizar o cabeçalho do arquivo binário
        cabecalho.status = '1'; // Definir status como '1' para indicar que o arquivo está ativo
        fseek(binFile, 0, SEEK_SET);

        escreverCabecalho(binFile, &cabecalho);

        fclose(csvFile);
        fclose(binFile);

        binarioNaTela(arquivoSaida);

    }else if (funcionalidade == 2) {
        
        arquivoEntrada = argv[1];
        // Remove leading/trailing whitespace from the file name
        while (isspace((unsigned char)*arquivoEntrada)) arquivoEntrada++;
        char *end = arquivoEntrada + strlen(arquivoEntrada) - 1;
        while (end > arquivoEntrada && isspace((unsigned char)*end)) end--;
        *(end + 1) = '\0';
        
        // Abrir o arquivo binário para leitura
        // Verificar se o arquivo foi aberto corretamente
        FILE *binFile = fopen(arquivoEntrada, "rb");
        if (binFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
          return 0;
        }
        // Chamar a função lerCabecalho para ler o cabeçalho do arquivo.
        Cabecalho cabecalho;
        if (!lerCabecalho(binFile, &cabecalho) || cabecalho.status != '1') {
            fprintf(stderr, "Falha no processamento do arquivo.\n");
            fclose(binFile);
            return 1;
        }
        
        if (cabecalho.nroRegArq == 0) {
            printf("Registro inexistente.\n");
            fclose(binFile);
            return 0;
        }
        
        
        while (ftell(binFile) <= cabecalho.proxByteOffset) {
            RegistroDados registro = {0};
        
            // Ler campos fixos
            if (fread(&registro.removido, sizeof(char), 1, binFile) != 1) break;
            if (fread(&registro.tamanhoRegistro, sizeof(int), 1, binFile) != 1) break;
            if (fread(&registro.prox, sizeof(long int), 1, binFile) != 1) break;
            if (fread(&registro.idAttack, sizeof(int), 1, binFile) != 1) break;
            if (fread(&registro.year, sizeof(int), 1, binFile) != 1) break;
            if (fread(&registro.financialLoss, sizeof(float), 1, binFile) != 1) break;
        
            // Processar campos variáveis
            char codigo;
            while (1) {
            if (fread(&codigo, sizeof(char), 1, binFile) != 1) break;

            // Identificar campo pelo código
            if (codigo == cabecalho.codDescreveContry[0]) {
                registro.country = lerCampoVariavel(binFile);
            }
            else if (codigo == cabecalho.codDescreveType[0]) {
                registro.attackType = lerCampoVariavel(binFile);
            }
            else if (codigo == cabecalho.codDescreveTargetIndustry[0]) {
                registro.targetIndustry = lerCampoVariavel(binFile);
            }
            else if (codigo == cabecalho.codDescreveDefense[0]) {
                registro.defenseMechanism = lerCampoVariavel(binFile);
            }
            else if (codigo == '\0' || codigo == '0') {
                fseek(binFile, -1, SEEK_CUR);
                break; 
            }
            }

            if (registro.removido == '0') {
                exibirRegistro(registro, cabecalho);
            } 

            // Liberar memória dos campos variáveis
            free(registro.country);
            free(registro.attackType);
            free(registro.targetIndustry);
            free(registro.defenseMechanism);
        }
        fclose(binFile);

    }
    else if (funcionalidade == 3 || funcionalidade == 4) { // Bloco que precisa ser ajustado
        arquivoEntrada = argv[1];
        int n_operacoes = 0; // Renomeado de n_buscas para ser mais genérico para 3 e 4
        if (sscanf(argv[2], "%d", &n_operacoes) != 1) {
            fprintf(stderr, "Erro ao converter o número de operações para inteiro.\n");
            return 0;
        }

        if (funcionalidade == 3) {
            // Lógica original da funcionalidade 3 permanece aqui
            FILE *binFile = fopen(arquivoEntrada, "rb"); // Abre apenas para leitura
            if (binFile == NULL) {
            printf("Falha no processamento do arquivo.\n");
                return 0;
            }
            Cabecalho cabecalho;
            if (!lerCabecalho(binFile, &cabecalho) || cabecalho.status != '1') {
                fprintf(stderr, "Falha no processamento do arquivo.\n");
                fclose(binFile);
                return 0;
            }
            if (cabecalho.nroRegArq == 0) {
                printf("Registro inexistente.\n");
                fclose(binFile);
                return 0 ;
            }

            for (int i = 0; i < n_operacoes; i++) { // Usa n_operacoes
                Filtro *filtro = lerFiltroEntrada();
                if (filtro == NULL) {
                    fprintf(stderr, "Erro ao ler filtro.\n");
                    fclose(binFile);
                    return 0;
                }
                funcionalidade3(binFile, filtro, cabecalho, funcionalidade);
            }

        } else if (funcionalidade == 4) {
            
            funcionalidade4(arquivoEntrada, n_operacoes); 
        }
    }

    else if (funcionalidade == 5) {
        arquivoEntrada = argv[1];
        int n_insercoes = 0;
        if (sscanf(argv[2], "%d", &n_insercoes) != 1) {
            fprintf(stderr, "Erro ao converter o número de inserções para inteiro.\n");
            return 0;
        }

        // Delega o trabalho para a função especialista
        funcionalidade5(arquivoEntrada, n_insercoes);
    }

    else if (funcionalidade == 6) {
        arquivoEntrada = argv[1];
        int n_atualizacoes = 0;
        if (sscanf(argv[2], "%d", &n_atualizacoes) != 1) {
            fprintf(stderr, "Erro ao converter o número de atualizações para inteiro.\n");
            return 0;
        }

        funcionalidade6(arquivoEntrada, n_atualizacoes);
    }
    else if(funcionalidade == 7) {
        arquivoEntrada = argv[1];
        arquivoSaida = argv[2];

        // Remove leading/trailing whitespace from the file name
        while (isspace((unsigned char)*arquivoEntrada)) arquivoEntrada++;
        char *end = arquivoEntrada + strlen(arquivoEntrada) - 1;
        while (end > arquivoEntrada && isspace((unsigned char)*end)) end--;
        *(end + 1) = '\0';

        funcionalidade7(arquivoEntrada, arquivoSaida);
    }
    else if (funcionalidade == 8) {
        char *arquivoDados = argv[1];
        char *arquivoIndice = argv[2];
        int n = atoi(argv[3]);

    // Delega o trabalho para a função especialista
        funcionalidade8(arquivoDados, arquivoIndice, n, funcionalidade);
    }
    else if (funcionalidade == 10) {
        char *arquivoDados = argv[1];
        char *arquivoIndice = argv[2];
        int n = atoi(argv[3]);

        // Delega o trabalho para a função especialista
        funcionalidade10(arquivoDados, arquivoIndice, n);
    }
    
    else if (funcionalidade == 11) {
        char *arquivoDados = argv[1];
        char *arquivoIndice = argv[2];
        int n = atoi(argv[3]);

        // Delega o trabalho para a função especialista
        funcionalidade11(arquivoDados, arquivoIndice, n);
    }

    else {
        fprintf(stderr, "Funcionalidade %d não implementada.\n", funcionalidade);
        return 1;
        }
        
    return 0;
}