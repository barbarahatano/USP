#include <stdio.h>
#include <stdlib.h>
#include "arquivo.h"
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "exibicao.h"
#include "busca.h"
#include <sys/types.h>

int registroCorresponde(RegistroDados *registro, Cabecalho *cabecalho, Filtro *filtro) {
    for (int i = 0; i < filtro->qtdPares; i++) {
        char *nomeCampo = filtro->campos[i];
        char *valorCampo = filtro->valores[i];
        
    
        if (strcmp(nomeCampo, "idAttack") == 0) {
            int valor=atoi(valorCampo);
            
            // Se o valor de busca for "NADA CONSTA" e o registro tem idAttack == -1
            if (strcmp(valorCampo, "NADA CONSTA") == 0) {
                if (registro->idAttack != -1) {
                    return 0; // Não corresponde
                }
            }
            // Se o valor de busca NÃO for "NADA CONSTA"
            else {
                if (registro->idAttack != valor) {
                    return 0; // Não corresponde
                }
            }
        } 
        else if (strcmp(nomeCampo, "year") == 0) {
            if (strcmp(valorCampo, "NADA CONSTA") == 0) {
                if (registro->year != -1) {
                    return 0; // Não corresponde
                }
            } else {
                int valorBusca = atoi(valorCampo);
                if (registro->year != valorBusca) {
                    return 0; // Não corresponde
                }
            }
        } 
        else if (strcmp(nomeCampo, "financialLoss") == 0) {
            if (strcmp(valorCampo, "NADA CONSTA") == 0) {
                if (registro->financialLoss != -1.0) {
                    return 0; // Não corresponde
                }
            } else {
                float valorBusca = strtof(valorCampo, NULL);
                if (registro->financialLoss != valorBusca) {
                    return 0; // Não corresponde
                }
            }
        } 
        else if (strcmp(nomeCampo, "country") == 0) {
            if (strcmp(valorCampo, "NADA CONSTA") == 0) {
                if (registro->country != NULL) {
                    return 0; // Não corresponde
                }
            } else {
                if (registro->country == NULL || strcmp(registro->country, valorCampo) != 0) {
                    return 0; // Não corresponde
                }
            }
        } 
        else if (strcmp(nomeCampo, "attackType") == 0) {
            if (strcmp(valorCampo, "NADA CONSTA") == 0) {
                if (registro->attackType != NULL) {
                    return 0; // Não corresponde
                }
            } else {
                if (registro->attackType == NULL || strcmp(registro->attackType, valorCampo) != 0) {
                    return 0; // Não corresponde
                }
            }
        } 
        else if (strcmp(nomeCampo, "targetIndustry") == 0) {
            if (strcmp(valorCampo, "NADA CONSTA") == 0) {
                if (registro->targetIndustry != NULL) {
                    return 0; // Não corresponde
                }
            } else {
                if (registro->targetIndustry == NULL || strcmp(registro->targetIndustry, valorCampo) != 0) {
                    return 0; // Não corresponde
                }
            }
        } 
        else if (strcmp(nomeCampo, "defenseMechanism") == 0) {
            if (strcmp(valorCampo, "NADA CONSTA") == 0) {
                if (registro->defenseMechanism != NULL) {
                    return 0; // Não corresponde
                }
            } else {
                if (registro->defenseMechanism == NULL || strcmp(registro->defenseMechanism, valorCampo) != 0) {
                    return 0; // Não corresponde
                }
            }
        } 
        else {
            fprintf(stderr, "Erro: Campo desconhecido '%s'\n", nomeCampo);
            return 0;
        }
    }
    
    return 1; // Todos os filtros correspondem
}

Filtro *lerFiltroEntrada() {
    Filtro *filtro = (Filtro *)malloc(sizeof(Filtro));
    if (filtro == NULL) {
        perror("Erro ao alocar memória para filtro");
        return NULL;
    }

    // Lê toda a linha usando getchar
    size_t buffer_capacity = 256;
    char *linha_completa = (char *)malloc(buffer_capacity);
    if (linha_completa == NULL) {
        perror("Erro ao alocar buffer para linha");
        free(filtro);
        return NULL;
    }
    
    size_t buffer_size = 0;
    int c;
    
    
    // Lê caractere por caractere até encontrar \n ou EOF
    while ((c = getchar()) != EOF && c != '\n') {
        // Verifica se precisa expandir o buffer
        if (buffer_size >= buffer_capacity - 1) {
            buffer_capacity *= 2;
            char *temp = (char *)realloc(linha_completa, buffer_capacity);
            if (temp == NULL) {
                perror("Erro ao redimensionar buffer");
                free(linha_completa);
                free(filtro);
                return NULL;
            }
            linha_completa = temp;
        }
        linha_completa[buffer_size++] = c;
    }
    
    // Termina a string
    linha_completa[buffer_size] = '\0';
    
    // Verifica se chegou ao EOF sem ler nada
    if (c == EOF && buffer_size == 0) {
        printf("Fim de arquivo detectado.\n");
        free(linha_completa);
        free(filtro);
        return NULL;
    }
    

    // Verifica se a linha não está vazia
    if (buffer_size == 0) {
        fprintf(stderr, "Erro: linha vazia.\n");
        free(linha_completa);
        free(filtro);
        return NULL;
    }

    // Parsing manual
    char *ptr = linha_completa;
    char *endptr;
    
    // Pula espaços iniciais
    while (*ptr == ' ' || *ptr == '\t') ptr++;
    
    // Extrai qtdPares
    filtro->qtdPares = strtol(ptr, &endptr, 10);
    if (ptr == endptr) {
        fprintf(stderr, "Erro: não foi possível ler qtdPares da linha: '%s'\n", linha_completa);
        free(linha_completa);
        free(filtro);
        return NULL;
    }
    
    ptr = endptr;

    if (filtro->qtdPares <= 0) {
        filtro->campos = NULL;
        filtro->valores = NULL;
        free(linha_completa);
        return filtro;
    }

    // Aloca memória para os arrays
    filtro->campos = (char **)malloc(filtro->qtdPares * sizeof(char *));
    if (filtro->campos == NULL) {
        perror("Erro ao alocar memória para campos");
        free(linha_completa);
        free(filtro);
        return NULL;
    }
    
    filtro->valores = (char **)malloc(filtro->qtdPares * sizeof(char *));
    if (filtro->valores == NULL) {
        perror("Erro ao alocar memória para valores");
        free(filtro->campos);
        free(linha_completa);
        free(filtro);
        return NULL;
    }

    // Inicializa ponteiros
    for(int i = 0; i < filtro->qtdPares; i++) {
        filtro->campos[i] = NULL;
        filtro->valores[i] = NULL;
    }

    // Extrai os pares campo-valor
    for (int i = 0; i < filtro->qtdPares; i++) {
        // Pula espaços
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        
        if (*ptr == '\0') {
            fprintf(stderr, "Erro: fim da linha inesperado ao procurar campo %d\n", i + 1);
            free(linha_completa);
            liberarFiltro(filtro);
            return NULL;
        }
        
        // Extrai campo
        char *inicio_campo = ptr;
        while (*ptr != ' ' && *ptr != '\t' && *ptr != '\0') ptr++;
        
        if (*ptr == '\0' && i < filtro->qtdPares - 1) {
            fprintf(stderr, "Erro: valor não encontrado para campo %d\n", i + 1);
            free(linha_completa);
            liberarFiltro(filtro);
            return NULL;
        }
        
        size_t tam_campo = ptr - inicio_campo;
        filtro->campos[i] = (char *)malloc(tam_campo + 1);
        if (filtro->campos[i] == NULL) {
            perror("Erro ao alocar memória para campo");
            free(linha_completa);
            liberarFiltro(filtro);
            return NULL;
        }
        strncpy(filtro->campos[i], inicio_campo, tam_campo);
        filtro->campos[i][tam_campo] = '\0';
        
        
        // Pula espaços antes do valor
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        
        if (*ptr == '\0') {
            fprintf(stderr, "Erro: valor não encontrado para campo '%s'\n", filtro->campos[i]);
            free(linha_completa);
            liberarFiltro(filtro);
            return NULL;
        }
        
        // Extrai valor (com suporte a aspas)
        char *inicio_valor;
        size_t tam_valor;
        
        if (*ptr == '"') {
            // Valor entre aspas
            ptr++; // Pula primeira aspa
            inicio_valor = ptr;
            
            while (*ptr != '"' && *ptr != '\0') ptr++;
            
            if (*ptr != '"') {
                fprintf(stderr, "Erro: aspas não fechadas no valor do campo '%s'\n", filtro->campos[i]);
                free(linha_completa);
                liberarFiltro(filtro);
                return NULL;
            }
            
            tam_valor = ptr - inicio_valor;
            ptr++; // Pula aspa de fechamento
        } else {
            // Valor sem aspas
            inicio_valor = ptr;
            while (*ptr != ' ' && *ptr != '\t' && *ptr != '\0') ptr++;
            tam_valor = ptr - inicio_valor;
        }
        
        filtro->valores[i] = (char *)malloc(tam_valor + 1);
        if (filtro->valores[i] == NULL) {
            perror("Erro ao alocar memória para valor");
            free(linha_completa);
            liberarFiltro(filtro);
            return NULL;
        }
        strncpy(filtro->valores[i], inicio_valor, tam_valor);
        filtro->valores[i][tam_valor] = '\0';
        
    }

    free(linha_completa);
    
    return filtro;
}

void liberarFiltro(Filtro *filtro) {
    // 1. Verifica se o ponteiro para a struct Filtro não é nulo.
    // Se for nulo, não há nada para liberar.
    
    if (filtro == NULL) {
        // Opcional: Você pode adicionar uma mensagem de aviso aqui se quiser,
        // mas geralmente não é necessário para um ponteiro nulo.
        // fprintf(stderr, "Aviso: Tentativa de liberar Filtro nulo.\n");
        return;
    }

    // 2. Libera a memória alocada para cada string em filtro->campos.
    // Primeiro, verifica se o array 'campos' não é nulo.
    if (filtro->campos != NULL) {
        // Percorre o array e libera cada string individualmente.
        for (int i = 0; i < filtro->qtdPares; i++) {
            if (filtro->campos[i] != NULL) { // Verifica se a string não é nula antes de liberar
                free(filtro->campos[i]);
                filtro->campos[i] = NULL; // Boa prática: define o ponteiro para NULL após liberar
            }
        }
        // Após liberar todas as strings, libera o array de ponteiros 'campos'.
        free(filtro->campos);
        filtro->campos = NULL; // Boa prática: define o ponteiro para NULL
    }

    // 3. Libera a memória alocada para cada string em filtro->valores.
    // Similarmente, verifica se o array 'valores' não é nulo.
    if (filtro->valores != NULL) {
        // Percorre o array e libera cada string individualmente.
        for (int i = 0; i < filtro->qtdPares; i++) {
            if (filtro->valores[i] != NULL) { // Verifica se a string não é nula antes de liberar
                free(filtro->valores[i]);
                filtro->valores[i] = NULL; // Boa prática: define o ponteiro para NULL após liberar
            }
        }
        // Após liberar todas as strings, libera o array de ponteiros 'valores'.
        free(filtro->valores);
        filtro->valores = NULL; // Boa prática: define o ponteiro para NULL
    }
    
    // 4. Finalmente, libera a memória da própria struct Filtro.
    free(filtro);
    
}