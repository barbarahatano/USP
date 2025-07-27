#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arquivo.h"
#include "busca.h"
#include "exibicao.h"
#include "funcionalidades.h"

// Calcula o tamanho que um registro ocuparia no arquivo
int calcular_tamanho_novo_registro(RegistroDados *registro) {
    int tamanho = sizeof(long) + sizeof(int) * 2 + sizeof(float); // prox, idAttack, year, financialLoss

    if (registro->country != NULL) tamanho += strlen(registro->country) + 2; // +1 para código, +1 para delimitador
    if (registro->attackType != NULL) tamanho += strlen(registro->attackType) + 2;
    if (registro->targetIndustry != NULL) tamanho += strlen(registro->targetIndustry) + 2;
    if (registro->defenseMechanism != NULL) tamanho += strlen(registro->defenseMechanism) + 2;
    
    return tamanho;
}


// Atualiza os campos da struct 'registro' com base nos 'novos_dados'
void atualizar_campos_do_registro(RegistroDados *registro, Filtro *novos_dados) {
    for (int i = 0; i < novos_dados->qtdPares; i++) {
        char *nomeCampo = novos_dados->campos[i];
        char *valorCampo = novos_dados->valores[i];

        if (strcmp(nomeCampo, "idAttack") == 0) {
            registro->idAttack = (strcmp(valorCampo, "NULO") == 0) ? -1 : atoi(valorCampo);
        } else if (strcmp(nomeCampo, "year") == 0) {
            registro->year = (strcmp(valorCampo, "NULO") == 0) ? -1 : atoi(valorCampo);
        } else if (strcmp(nomeCampo, "financialLoss") == 0) {
            registro->financialLoss = (strcmp(valorCampo, "NULO") == 0) ? -1.0f : strtof(valorCampo, NULL);
        } else if (strcmp(nomeCampo, "country") == 0) {
            free(registro->country);
            registro->country = (strcmp(valorCampo, "NULO") == 0) ? NULL : strdup(valorCampo);
        } else if (strcmp(nomeCampo, "attackType") == 0) {
            free(registro->attackType);
            registro->attackType = (strcmp(valorCampo, "NULO") == 0) ? NULL : strdup(valorCampo);
        } else if (strcmp(nomeCampo, "targetIndustry") == 0) {
            free(registro->targetIndustry);
            registro->targetIndustry = (strcmp(valorCampo, "NULO") == 0) ? strdup("NULO"): strdup(valorCampo);
        } else if (strcmp(nomeCampo, "defenseMechanism") == 0) {
            free(registro->defenseMechanism);
            registro->defenseMechanism = (strcmp(valorCampo, "NULO") == 0) ? NULL : strdup(valorCampo);
        }
    }
}


// Insere um registro usando a estratégia First Fit
long inserir_registro_first_fit(FILE *binFile, RegistroDados *registro, Cabecalho *cabecalho) {
    long rrn_atual = cabecalho->topo;
    long rrn_anterior = -1;
    
    // 1. Calcula e guarda o tamanho LÓGICO (real) do novo registro.
    int tamanho_logico_novo = calcular_tamanho_novo_registro(registro);

    // 2. Tenta encontrar um espaço na lista de removidos.
    while (rrn_atual != -1) {
        fseek(binFile, rrn_atual + sizeof(char), SEEK_SET); // Pula 'removido'
        int tamanho_disponivel; // Tamanho FÍSICO do buraco
        long proximo_removido;
        fread(&tamanho_disponivel, sizeof(int), 1, binFile);
        fread(&proximo_removido, sizeof(long int), 1, binFile);

        // 3. Verifica se o registro novo CABE no buraco.
        if (tamanho_logico_novo <= tamanho_disponivel) {
            // ENCONTROU ESPAÇO!
            
            // Remove o buraco da lista de livres (lógica de ponteiros).
            if (rrn_anterior == -1) {
                cabecalho->topo = proximo_removido;
            } else {
                fseek(binFile, rrn_anterior + sizeof(char) + sizeof(int), SEEK_SET);
                fwrite(&proximo_removido, sizeof(long int), 1, binFile);
            }
            
            // Prepara o registro para a escrita.
            // O campo 'tamanhoRegistro' no disco DEVE refletir o tamanho FÍSICO do buraco.
            registro->tamanhoRegistro = tamanho_disponivel;
            registro->removido = '0'; // Garante que o registro seja marcado como não-removido.

            // ===== BLOCO DE ESCRITA MANUAL (SUBSTITUIÇÃO DE escreverRegistro) =====
            fseek(binFile, rrn_atual, SEEK_SET); // Posiciona no início do buraco.

            fwrite(&registro->removido, sizeof(char), 1, binFile);
            fwrite(&registro->tamanhoRegistro, sizeof(int), 1, binFile);
            fwrite(&registro->prox, sizeof(long int), 1, binFile);
            fwrite(&registro->idAttack, sizeof(int), 1, binFile);
            fwrite(&registro->year, sizeof(int), 1, binFile);
            fwrite(&registro->financialLoss, sizeof(float), 1, binFile);

            // Escreve os campos variáveis.
            if (registro->country) {
                fwrite(cabecalho->codDescreveContry, sizeof(char), 1, binFile);
                fwrite(registro->country, strlen(registro->country), 1, binFile);
                fwrite("|", sizeof(char), 1, binFile);
            }
            if (registro->attackType) {
                fwrite(cabecalho->codDescreveType, sizeof(char), 1, binFile);
                fwrite(registro->attackType, strlen(registro->attackType), 1, binFile);
                fwrite("|", sizeof(char), 1, binFile);
            }
            if (registro->targetIndustry) {
                fwrite(cabecalho->codDescreveTargetIndustry, sizeof(char), 1, binFile);
                fwrite(registro->targetIndustry, strlen(registro->targetIndustry), 1, binFile);
                fwrite("|", sizeof(char), 1, binFile);
            }
            if (registro->defenseMechanism) {
                fwrite(cabecalho->codDescreveDefense, sizeof(char), 1, binFile);
                fwrite(registro->defenseMechanism, strlen(registro->defenseMechanism), 1, binFile);
                fwrite("|", sizeof(char), 1, binFile);
            }
            // ================= FIM DO BLOCO DE ESCRITA =================

            // Preenche o lixo restante com '$'.
            int lixo = tamanho_disponivel - tamanho_logico_novo;
            for(int k = 0; k < lixo; k++) {
                fwrite("$", 1, 1, binFile);
            }
            
            cabecalho->nroRegArq++;
            cabecalho->nroRegRem--;
            return rrn_atual; // Inserção concluída.
        }
        
        // Avança para o próximo da lista de removidos.
        rrn_anterior = rrn_atual;
        rrn_atual = proximo_removido;
    }
    
    // 4. SE NÃO ACHOU ESPAÇO, insere no final do arquivo.
    fseek(binFile, cabecalho->proxByteOffset, SEEK_SET);
    registro->tamanhoRegistro = tamanho_logico_novo; // No final, o tamanho físico e lógico são iguais.
    registro->removido = '0';
    rrn_atual = cabecalho->proxByteOffset;

    // ===== BLOCO DE ESCRITA MANUAL (PARA O FINAL DO ARQUIVO) =====
    fwrite(&registro->removido, sizeof(char), 1, binFile);
    fwrite(&registro->tamanhoRegistro, sizeof(int), 1, binFile);
    fwrite(&registro->prox, sizeof(long int), 1, binFile);
    fwrite(&registro->idAttack, sizeof(int), 1, binFile);
    fwrite(&registro->year, sizeof(int), 1, binFile);
    fwrite(&registro->financialLoss, sizeof(float), 1, binFile);

    // Escreve os campos variáveis.
    if (registro->country) {
        fwrite(cabecalho->codDescreveContry, sizeof(char), 1, binFile);
        fwrite(registro->country, strlen(registro->country), 1, binFile);
        fwrite("|", sizeof(char), 1, binFile);
    }
    if (registro->attackType) {
        fwrite(cabecalho->codDescreveType, sizeof(char), 1, binFile);
        fwrite(registro->attackType, strlen(registro->attackType), 1, binFile);
        fwrite("|", sizeof(char), 1, binFile);
    }
    if (registro->targetIndustry) {
        fwrite(cabecalho->codDescreveTargetIndustry, sizeof(char), 1, binFile);
        fwrite(registro->targetIndustry, strlen(registro->targetIndustry), 1, binFile);
        fwrite("|", sizeof(char), 1, binFile);
    }
    if (registro->defenseMechanism) {
        fwrite(cabecalho->codDescreveDefense, sizeof(char), 1, binFile);
        fwrite(registro->defenseMechanism, strlen(registro->defenseMechanism), 1, binFile);
        fwrite("|", sizeof(char), 1, binFile);
    }
    // ================= FIM DO BLOCO DE ESCRITA =================

    cabecalho->proxByteOffset = ftell(binFile); // Atualiza o novo final do arquivo.
    cabecalho->nroRegArq++;
    return rrn_atual; // Retorna o RRN do novo registro inserido.
    printf("rrn_atual: %ld\n", rrn_atual);
}