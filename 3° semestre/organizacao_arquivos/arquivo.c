#include <stdio.h>
#include <stdlib.h>
#include "arquivo.h"
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


// Função para inicializar a struct Cabecalho com os valores iniciais
int inicializarCabecalhoStruct(Cabecalho *cabecalho, FILE *arquivo) {
    char *linha = NULL;
    size_t len = 0;
    char *token;
    int campo = 0;
    
    cabecalho->status = '0';
    cabecalho->topo = -1;
    cabecalho->proxByteOffset = 0;
    cabecalho->nroRegArq = 0;
    cabecalho->nroRegRem = 0;
    cabecalho->codDescreveContry[0] = '1';
    cabecalho->codDescreveType[0] = '2';
    cabecalho->codDescreveTargetIndustry[0] = '3';
    cabecalho->codDescreveDefense[0] = '4';

    if (getline(&linha, &len, arquivo) == -1) {
        perror("Erro ao ler a linha de cabeçalho do arquivo CSV");
        free(linha);
        return 0; // Falha na leitura
    }
    linha[strcspn(linha, "\n")] = 0; // Remover a quebra de linha
  
    token = linha;
    for (int i = 0; linha[i] != '\0'; i++) {
        if (linha[i] == ',' || linha[i + 1] == '\0') {
            if (linha[i + 1] == '\0') i++; // Inclui o último caractere se for o final da linha
            linha[i] = '\0';

            // Skip empty tokens caused by trailing commas
            

            switch (campo) {
                case 0: // descreveIdentificador
                    memcpy(cabecalho->descreveIdentificador, token, 23);
                    break;
                case 1: // descreveYear
                    memcpy(cabecalho->descreveYear, token, 27);
                    break;
                case 2: // descreveFinancialLoss
                    memcpy(cabecalho->descreveFinancialLoss, token, 28);
                    break;
                case 3: // descreveCountry
                    memcpy(cabecalho->descreveCountry, token, 26);
                    break;
                case 4: // descreveType
                    memcpy(cabecalho->descreveType, token, 38);
                    break;
                case 5: // descreveTargetIndustry
                    memcpy(cabecalho->descreveTargetIndustry, token, 38);
                    break;
                case 6: // descreveDefense
                    memcpy(cabecalho->descreveDefense, token, 67);
                    break;
            }

            token = &linha[i + 1];
            campo++;
        }
    }
    return 1; // Sucesso na leitura das descrições
}

// Função para escrever o cabeçalho no arquivo binário
int escreverCabecalho(FILE *arquivo, Cabecalho *cabecalho) {
    fwrite(&cabecalho->status, sizeof(char), 1, arquivo);
    fwrite(&cabecalho->topo, sizeof(long int), 1, arquivo);
    fwrite(&cabecalho->proxByteOffset, sizeof(long int), 1, arquivo);
    fwrite(&cabecalho->nroRegArq, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->nroRegRem, sizeof(int), 1, arquivo);
    fwrite(cabecalho->descreveIdentificador, sizeof(char), 23, arquivo);
    fwrite(cabecalho->descreveYear, sizeof(char), 27, arquivo);
    fwrite(cabecalho->descreveFinancialLoss, sizeof(char), 28, arquivo);
    fwrite(cabecalho->codDescreveContry, sizeof(char), 1, arquivo);
    fwrite(cabecalho->descreveCountry, sizeof(char), 26, arquivo);
    fwrite(cabecalho->codDescreveType, sizeof(char), 1, arquivo);
    fwrite(cabecalho->descreveType, sizeof(char), 38, arquivo);
    fwrite(cabecalho->codDescreveTargetIndustry, sizeof(char), 1, arquivo);
    fwrite(cabecalho->descreveTargetIndustry, sizeof(char), 38, arquivo);
    fwrite(cabecalho->codDescreveDefense, sizeof(char), 1, arquivo);
    fwrite(cabecalho->descreveDefense, sizeof(char), 67, arquivo);
    return 1;
}


// Função para escrever um registro de dados no arquivo binário
int escreverRegistro(FILE *arquivo, RegistroDados *registro, Cabecalho *cabecalho) {
    int tamanho_country = 0;
    int tamanho_attackType = 0;
    int tamanho_targetIndustry = 0;
    int tamanho_defenseMechanism = 0;

    if (registro->country != NULL) tamanho_country = strlen(registro->country);
    if (registro->attackType != NULL) tamanho_attackType = strlen(registro->attackType);
    if (registro->targetIndustry != NULL) tamanho_targetIndustry = strlen(registro->targetIndustry);
    if (registro->defenseMechanism != NULL) tamanho_defenseMechanism = strlen(registro->defenseMechanism);

    registro->tamanhoRegistro = sizeof(long int) + // prox
                                sizeof(int) + // idAttack
                                sizeof(int) + // year
                                sizeof(float);    // financialLoss

    if (registro->country != NULL) registro->tamanhoRegistro += 1 + tamanho_country + 1;
    if (registro->attackType != NULL) registro->tamanhoRegistro += 1 + tamanho_attackType + 1;
    if (registro->targetIndustry != NULL) registro->tamanhoRegistro += 1 + tamanho_targetIndustry + 1;
    if (registro->defenseMechanism != NULL) registro->tamanhoRegistro += 1 + tamanho_defenseMechanism + 1;

    if (fwrite(&registro->removido, sizeof(char), 1, arquivo) != 1) return 0;
    if (fwrite(&registro->tamanhoRegistro, sizeof(int), 1, arquivo) != 1) return 0;
    if (fwrite(&registro->prox, sizeof(long int), 1, arquivo) != 1) return 0;
    if (fwrite(&registro->idAttack, sizeof(int), 1, arquivo) != 1) return 0;
    if (fwrite(&registro->year, sizeof(int), 1, arquivo) != 1) return 0;
    if (fwrite(&registro->financialLoss, sizeof(float), 1, arquivo) != 1) return 0;

    if (registro->country != NULL && strcmp(registro->country, ",") != 0) {
        if (fwrite(&cabecalho->codDescreveContry[0], sizeof(char), 1, arquivo) != 1) return 0;
        if (fwrite(registro->country, sizeof(char), tamanho_country, arquivo) != tamanho_country) return 0;
        if (fwrite("|", sizeof(char), 1, arquivo) != 1) return 0;
    }
    

    if (registro->attackType != NULL && strcmp(registro->attackType, ",") != 0) {
        if (fwrite(&cabecalho->codDescreveType[0], sizeof(char), 1, arquivo) != 1) return 0;
        if (fwrite(registro->attackType, sizeof(char), tamanho_attackType, arquivo) != tamanho_attackType) return 0;
        if (fwrite("|", sizeof(char), 1, arquivo) != 1) return 0;
    }

    if (registro->targetIndustry != NULL && strcmp(registro->targetIndustry, ",") != 0) {
        if (fwrite(&cabecalho->codDescreveTargetIndustry[0], sizeof(char), 1, arquivo) != 1) return 0;
        if (fwrite(registro->targetIndustry, sizeof(char), tamanho_targetIndustry, arquivo) != tamanho_targetIndustry) return 0;
        if (fwrite("|", sizeof(char), 1, arquivo) != 1) return 0;
    }

    if (registro->defenseMechanism != NULL&& strcmp(registro->defenseMechanism, ",") != 0) {
        if (fwrite(&cabecalho->codDescreveDefense[0], sizeof(char), 1, arquivo) != 1) return 0;
        if (fwrite(registro->defenseMechanism, sizeof(char), tamanho_defenseMechanism, arquivo) != tamanho_defenseMechanism) return 0;
        if (fwrite("|", sizeof(char), 1, arquivo) != 1) return 0;
    }

    return 1;
}

// Função para ler um registro de dados do arquivo CSV com tratamento de nulos e lixo
RegistroDados* lerRegistroCSV(FILE *csvFile) {
    char *linha = NULL;
    size_t len = 0;

    if (getline(&linha, &len, csvFile) == -1) {
        free(linha);
        return NULL;
    }
    // Remove o caractere '\r' se estiver presente no final da linha
    size_t linha_len = strlen(linha);
    if (linha_len > 0 && linha[linha_len - 1] == '\r') {
        linha[linha_len - 1] = '\0';
    }
    linha[strcspn(linha, "\n")] = '\0'; // Remove a quebra de linha, se existir

    RegistroDados *registro = (RegistroDados*)malloc(sizeof(RegistroDados));
    if (registro == NULL) {
        perror("Erro ao alocar memória para registro");
        return NULL;
    }

    // Inicializar ponteiros de strings com NULL
    registro->country = NULL;
    registro->attackType = NULL;
    registro->targetIndustry = NULL;
    registro->defenseMechanism = NULL;

    char *token = linha;
    int campo = 0; 

    for (int i = 0; linha[i] != '\0'; i++) {
        if (linha[i] == ',' || linha[i + 1] == '\0') {
            if (linha[i + 1] == '\0' && linha[i]!=',') i++; // Inclui o último caractere se for o final da linha
            linha[i] = '\0';

            if (strlen(token) == 0) {
                // Campo vazio, tratar como nulo
                switch (campo) {
                    case 0: // idAttack
                        registro->idAttack = -1;
                        break;
                    case 1: // year
                        registro->year = -1;
                        break;
                    case 2: // financialLoss
                        registro->financialLoss = -1.0;
                        break;
                    case 3: // country
                        registro->country = NULL;
                        break;
                    case 4: // attackType
                        registro->attackType = NULL;
                        break;
                    case 5: // targetIndustry
                        registro->targetIndustry = NULL;
                        break;
                    case 6: // defenseMechanism
                        registro->defenseMechanism = NULL;
                        break;
                }
            } else {
                // Campo preenchido, processar normalmente
                switch (campo) {
                    case 0: // idAttack
                        registro->idAttack = atoi(token);
                        break;
                    case 1: // year
                        registro->year = atoi(token);
                        break;
                    case 2: // financialLoss
                        registro->financialLoss = strtof(token, NULL);
                        break;
                    case 3: // country
                        registro->country = malloc(strlen(token) + 1);
                        if (registro->country != NULL) strcpy(registro->country, token);
                        break;
                    case 4: // attackType
                        registro->attackType = malloc(strlen(token) + 1);
                        if (registro->attackType != NULL) strcpy(registro->attackType, token);
                        break;
                    case 5: // targetIndustry
                        registro->targetIndustry = malloc(strlen(token) + 1);
                        if (registro->targetIndustry != NULL) strcpy(registro->targetIndustry, token);
                        break;
                    case 6: // defenseMechanism
                        registro->defenseMechanism = malloc(strlen(token) + 1);
                        if (registro->defenseMechanism != NULL) strcpy(registro->defenseMechanism, token);
                        break;
                }
            }

            // Avança o token para o próximo campo
            token = &linha[i + 1];
            campo++;

            // Caso a linha termine com uma vírgula, elimina a vírgula e não escreve nada
            if (linha[i + 1] == '\0' && linha[i] == ',') {
                linha[i] = '\0'; // Remove a vírgula
                break; // Sai do loop
            }
        }
    }

    registro->removido = '0';
    registro->prox = -1;
    registro->tamanhoRegistro = 0;
    return registro;
}

char *lerCampoVariavel(FILE *arquivo) {
    char buffer[1024];
    int i = 0;
    char c;

    // Lê caracteres do arquivo até encontrar o delimitador '|', EOF ou atingir o limite do buffer
    while ((c = fgetc(arquivo)) != '|' && c != EOF && i < 1023) {
        buffer[i++] = c;
    }

    buffer[i] = '\0'; // Finaliza a string com o caractere nulo

    // Aloca memória para armazenar o campo lido
    char *campo = malloc(i + 1);
    if (campo == NULL) {
        fprintf(stderr, "Erro de alocação de memória\n");
        return NULL;
    }

    strcpy(campo, buffer); 
    return campo; // Retorna o campo lido
}

int lerCabecalho(FILE *arquivo, Cabecalho *cabecalho) {
    fseek(arquivo, 0, SEEK_SET); // Posiciona o ponteiro do arquivo no início
    fread(&cabecalho->status, sizeof(char), 1, arquivo); // Lê o status
    fread(&cabecalho->topo, sizeof(long int), 1, arquivo); // Lê o topo
    fread(&cabecalho->proxByteOffset, sizeof(long int), 1, arquivo); // Lê o próximo byte offset
    fread(&cabecalho->nroRegArq, sizeof(int), 1, arquivo); // Lê o número de registros ativos
    fread(&cabecalho->nroRegRem, sizeof(int), 1, arquivo); // Lê o número de registros removidos
    fread(cabecalho->descreveIdentificador, sizeof(char), 23, arquivo); // Lê a descrição do identificador // Garante o término da string
    fread(cabecalho->descreveYear, sizeof(char), 27, arquivo); // Lê a descrição do ano // Garante o término da string
    fread(cabecalho->descreveFinancialLoss, sizeof(char), 28, arquivo); // Lê a descrição do prejuízo financeiro
    fread(cabecalho->codDescreveContry, sizeof(char), 1, arquivo); // Lê o código do país
    fread(cabecalho->descreveCountry, sizeof(char), 26, arquivo); // Lê a descrição do país
    fread(cabecalho->codDescreveType, sizeof(char), 1, arquivo); // Lê o código do tipo de ameaça
    fread(cabecalho->descreveType, sizeof(char), 38, arquivo); // Lê a descrição do tipo de ameaça
    fread(cabecalho->codDescreveTargetIndustry, sizeof(char), 1, arquivo); // Lê o código do setor da indústria
    fread(cabecalho->descreveTargetIndustry, sizeof(char), 38, arquivo); // Lê a descrição do setor da indústria
    fread(cabecalho->codDescreveDefense, sizeof(char), 1, arquivo); // Lê o código da estratégia de defesa
    fread(cabecalho->descreveDefense, sizeof(char), 67, arquivo); // Lê a descrição da estratégia de defesa
    return 1; // Retorna 1 para indicar sucesso
}

