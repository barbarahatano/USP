#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcionalidades.h"
#include "arquivo.h"
#include "busca.h"
#include "exibicao.h"
#include "arvoreB.h"
#include "buscaArvore.h"


static RegistroDados* ler_registro_da_entrada() {
    // Aloca memória e inicializa com zeros/NULLs
    RegistroDados *reg = (RegistroDados*) calloc(1, sizeof(RegistroDados));
    if (reg == NULL) {
        perror("Falha ao alocar registro");
        return NULL;
    }

    char buffer_numerico[100];
    char buffer_string[200];

    // Lê os campos de tamanho fixo primeiro
    // idAttack (int)
    scanf("%s", buffer_numerico);
    reg->idAttack = (strcmp(buffer_numerico, "NULO") == 0) ? -1 : atoi(buffer_numerico);

    // year (int)
    scanf("%s", buffer_numerico);
    reg->year = (strcmp(buffer_numerico, "NULO") == 0) ? -1 : atoi(buffer_numerico);

    // financialLoss (float)
    scanf("%s", buffer_numerico);
    reg->financialLoss = (strcmp(buffer_numerico, "NULO") == 0) ? -1.0f : strtof(buffer_numerico, NULL);

    // Lê os campos de tamanho variável usando scan_quote_string
    // country (string)
    scan_quote_string(buffer_string);
    if (strlen(buffer_string) > 0) {
        reg->country = strdup(buffer_string);
    } else {
        reg->country = NULL;
    }

    // attackType (string)
    scan_quote_string(buffer_string);
    if (strlen(buffer_string) > 0) {
        reg->attackType = strdup(buffer_string);
    } else {
        reg->attackType = NULL;
    }

    // targetIndustry (string)
    scan_quote_string(buffer_string);
    if (strlen(buffer_string) > 0) {
        reg->targetIndustry = strdup(buffer_string);
    } else {
        reg->targetIndustry = NULL;
    }

    // defenseMechanism (string)
    scan_quote_string(buffer_string);
    if (strlen(buffer_string) > 0) {
        reg->defenseMechanism = strdup(buffer_string);
    } else {
        reg->defenseMechanism = NULL;
    }

    // Define os valores padrão para um registro novo
    reg->removido = '0';
    reg->prox = -1;

    return reg;
}

void funcionalidade3(FILE *binFile, Filtro *filtro, Cabecalho cabecalho, int funcionalidade) {
    
    int encontrou = 0;
    fseek(binFile, 0, SEEK_SET);
    lerCabecalho(binFile, &cabecalho); // Relê o cabeçalho para cada busca
    long byteOffsetAtual = ftell(binFile);

    while (byteOffsetAtual < cabecalho.proxByteOffset) {
        fseek(binFile, byteOffsetAtual, SEEK_SET);

        RegistroDados registro = {0};
        long offsetInicioRegistro = ftell(binFile);

        if (fread(&registro.removido, sizeof(char), 1, binFile) != 1) break;
        if (fread(&registro.tamanhoRegistro, sizeof(int), 1, binFile) != 1) break;

        byteOffsetAtual = offsetInicioRegistro + sizeof(char) + sizeof(int) + registro.tamanhoRegistro;

        if (registro.removido == '1') {
            continue;
        }

        if (fread(&registro.prox, sizeof(long int), 1, binFile) != 1) break;
        if (fread(&registro.idAttack, sizeof(int), 1, binFile) != 1) break;
        if (fread(&registro.year, sizeof(int), 1, binFile) != 1) break;
        if (fread(&registro.financialLoss, sizeof(float), 1, binFile) != 1) break;

        char codigo;
        registro.country = NULL;
        registro.attackType = NULL;
        registro.targetIndustry = NULL;
        registro.defenseMechanism = NULL;

        while (ftell(binFile) < offsetInicioRegistro + sizeof(char) + sizeof(int) + registro.tamanhoRegistro) {
            if (fread(&codigo, sizeof(char), 1, binFile) != 1) break;

            if (codigo == cabecalho.codDescreveContry[0]) {
                registro.country = lerCampoVariavel(binFile);
            } else if (codigo == cabecalho.codDescreveType[0]) {
                registro.attackType = lerCampoVariavel(binFile);
            } else if (codigo == cabecalho.codDescreveTargetIndustry[0]) {
                registro.targetIndustry = lerCampoVariavel(binFile);
            } else if (codigo == cabecalho.codDescreveDefense[0]) {
                registro.defenseMechanism = lerCampoVariavel(binFile);
            } else {
                fseek(binFile, -1, SEEK_CUR);
                break;
            }
        }

        if (registro.removido == '0' && registroCorresponde(&registro, &cabecalho, filtro)) {
            exibirRegistro(registro, cabecalho);
            encontrou = 1;
        }

        if (registro.country != NULL) free(registro.country);
        if (registro.attackType != NULL) free(registro.attackType);
        if (registro.targetIndustry != NULL) free(registro.targetIndustry);
        if (registro.defenseMechanism != NULL) free(registro.defenseMechanism);
    }
    liberarFiltro(filtro);
    if (encontrou == 0) {
        printf("Registro inexistente.\n\n");
    }
    printf("**********\n"); // Separador para a funcionalidade 3

    if (funcionalidade == 3){
        fclose(binFile); // Fecha o arquivo da funcionalidade 3
    }
    
}

void funcionalidade5(char *nomeArquivoEntrada, int n) {
    FILE *binFile = fopen(nomeArquivoEntrada, "rb+");
    if (binFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho cabecalho;
    lerCabecalho(binFile, &cabecalho);

    if (cabecalho.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(binFile);
        return;
    }

    // 1. Marcar o arquivo como inconsistente para segurança
    cabecalho.status = '0';
    fseek(binFile, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, binFile);
    
    // 2. Loop para as 'n' inserções
    for (int i = 0; i < n; i++) {
        RegistroDados *novo_registro = ler_registro_da_entrada();
        if (novo_registro == NULL) continue; // Pula em caso de erro na leitura

        // 3. Calcula o tamanho que o novo registro ocupará no arquivo
        novo_registro->tamanhoRegistro = calcular_tamanho_novo_registro(novo_registro);

        // 4. Reutiliza a função da Funcionalidade 6 para inserir com First Fit
        inserir_registro_first_fit(binFile, novo_registro, &cabecalho);

        // 5. Libera a memória alocada para o registro que acabamos de inserir
        free(novo_registro->country);
        free(novo_registro->attackType);
        free(novo_registro->targetIndustry);
        free(novo_registro->defenseMechanism);
        free(novo_registro);
    }

    // 6. Marcar o arquivo como consistente no final
    cabecalho.status = '1';
    fseek(binFile, 0, SEEK_SET);
    escreverCabecalho(binFile, &cabecalho); // Escreve o cabeçalho atualizado

    fclose(binFile);

    // 7. Exibe o arquivo na tela conforme especificado 
    binarioNaTela(nomeArquivoEntrada);
}


/*
 * =================================================================================
 * FUNCIONALIDADE 6
 * =================================================================================
 */

void funcionalidade6(char *nomeArquivoEntrada, int n) {
    FILE *binFile = fopen(nomeArquivoEntrada, "rb+");
    if (binFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho cabecalho;
    lerCabecalho(binFile, &cabecalho);

    if (cabecalho.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(binFile);
        return;
    }

    // Marca o arquivo como inconsistente
    cabecalho.status = '0';
    fseek(binFile, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, binFile);

    // Loop para as 'n' atualizações
    for (int i = 0; i < n; i++) {
        Filtro *criterios_busca = lerFiltroEntrada();
        Filtro *dados_atualizacao = lerFiltroEntrada();

        lerCabecalho(binFile, &cabecalho);
        
        long byteOffsetAtual = ftell(binFile);

        while (byteOffsetAtual < cabecalho.proxByteOffset) {
            fseek(binFile, byteOffsetAtual, SEEK_SET);
            RegistroDados registro = {0};
            // Lê o registro atual
            char removido_flag;
            int tamanho_registro_atual;

            if (fread(&removido_flag, sizeof(char), 1, binFile) != 1) break;
            if (fread(&tamanho_registro_atual, sizeof(int), 1, binFile) != 1) break;

            byteOffsetAtual += tamanho_registro_atual + sizeof(char) + sizeof(int);

            if (removido_flag == '0') {
                 // Volta para ler o registro completo para a verificação
                
                // (Aqui você precisaria de uma função que lê o registro inteiro do arquivo)
                // Vamos simular a leitura para a verificação:
                registro.removido = '0'; // Já sabemos
                registro.tamanhoRegistro = tamanho_registro_atual;
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
                // ... ler os outros campos para a struct 'registro' ...

                // Se o registro corresponde aos critérios de busca
                if (registroCorresponde(&registro, &cabecalho, criterios_busca)) {
                    // Cria uma cópia para atualização
                    RegistroDados registro_atualizado = registro;
                    
                    // Aplica as atualizações nos campos
                    atualizar_campos_do_registro(&registro_atualizado, dados_atualizacao);
                    
                    // Calcula o novo tamanho
                    int tamanho_novo = calcular_tamanho_novo_registro(&registro_atualizado);

                    if (tamanho_novo <= registro.tamanhoRegistro) {
                        // Atualiza no lugar (in-place)
                        fseek(binFile, byteOffsetAtual - tamanho_registro_atual - sizeof(char) - sizeof(int), SEEK_SET);
                        registro_atualizado.tamanhoRegistro = registro.tamanhoRegistro; // Mantém o tamanho original no campo
                        escreverRegistro(binFile, &registro_atualizado, &cabecalho); // Reusa sua função de escrita

                        // Preenche o lixo com '$'
                        int lixo = registro.tamanhoRegistro - tamanho_novo;
                        for (int k = 0; k < lixo; k++) {
                            fwrite("$", sizeof(char), 1, binFile);
                        }
                    } else {
                        // Remove o registro antigo logicamente
                        removerRegistro(binFile, &registro, &cabecalho, byteOffsetAtual - tamanho_registro_atual - sizeof(char) - sizeof(int));
                        
                        // Insere o novo registro usando First Fit
                        registro_atualizado.tamanhoRegistro = tamanho_novo;
                        inserir_registro_first_fit(binFile, &registro_atualizado, &cabecalho);
                    }
                }
            }
             // Liberar memória dos campos variáveis lidos
            free(registro.country);
            free(registro.attackType);
            free(registro.targetIndustry);
            free(registro.defenseMechanism);
        }

        liberarFiltro(criterios_busca);
        liberarFiltro(dados_atualizacao);
    }

    // Atualiza o cabeçalho final
    cabecalho.status = '1';
    fseek(binFile, 0, SEEK_SET);
    escreverCabecalho(binFile, &cabecalho);

    fclose(binFile);
    binarioNaTela(nomeArquivoEntrada);
}


/*
 * =================================================================================
 * FUNCIONALIDADE 4: Remoção Lógica de Registros
 * =================================================================================
 */

void funcionalidade4(char *nomeArquivoEntrada, int n) {
    FILE *binFile = fopen(nomeArquivoEntrada, "rb+");
    if (binFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho cabecalho;
    // Tenta ler o cabeçalho. Se falhar ou o status for '0' (inconsistente), indica erro.
    if (!lerCabecalho(binFile, &cabecalho) || cabecalho.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(binFile);
        return;
    }

    // 1. Marcar o arquivo como inconsistente para segurança
    cabecalho.status = '0';
    fseek(binFile, 0, SEEK_SET); // Volta para o início do arquivo para escrever o status
    fwrite(&cabecalho.status, sizeof(char), 1, binFile);
    fflush(binFile); // Garante que o status '0' seja escrito imediatamente no disco

    // 2. Loop para as 'n' operações de remoção
    for (int i = 0; i < n; i++) {
        Filtro *filtro = lerFiltroEntrada(); // Lê o filtro de busca para a remoção atual
        if (filtro == NULL) {
            fprintf(stderr, "Erro ao ler filtro para remoção %d. Pulando esta operação.\n", i + 1);
            // Continua para a próxima iteração se houver erro na leitura do filtro
            liberarFiltro(filtro); // A função lida com NULL
            continue;
        }

        int encontrou_e_removeu = 0; // Flag para indicar se algum registro foi removido nesta iteração

        fseek(binFile, 0, SEEK_SET);
        lerCabecalho(binFile, &cabecalho); // Relê o cabeçalho para ter as informações mais atualizadas

        // Começa a leitura dos registros após o cabeçalho
        long byteOffsetLeitura = ftell(binFile); // Posição atual após ler o cabeçalho

        // Percorre todos os registros do arquivo
        while (byteOffsetLeitura < cabecalho.proxByteOffset) {
            fseek(binFile, byteOffsetLeitura, SEEK_SET); // Posiciona no início do registro atual

            RegistroDados registro = {0}; // Inicializa struct com zeros para evitar lixo de memória
            long offsetInicioRegistro = ftell(binFile); // Salva o offset do início do registro para `removerRegistro`

            // Lê o indicador de removido e o tamanho do registro
            if (fread(&registro.removido, sizeof(char), 1, binFile) != 1) break;
            if (fread(&registro.tamanhoRegistro, sizeof(int), 1, binFile) != 1) break;

            // Calcula o próximo offset para o loop, pulando este registro para a próxima iteração
            byteOffsetLeitura = offsetInicioRegistro + sizeof(char) + sizeof(int) + registro.tamanhoRegistro;

            // Se o registro já está logicamente removido, apenas pule-o para não tentar removê-lo novamente
            if (registro.removido == '1') {
                continue; // Pula para o próximo registro no arquivo
            }

            // Lê o restante dos campos fixos
            if (fread(&registro.prox, sizeof(long int), 1, binFile) != 1) break;
            if (fread(&registro.idAttack, sizeof(int), 1, binFile) != 1) break;
            if (fread(&registro.year, sizeof(int), 1, binFile) != 1) break;
            if (fread(&registro.financialLoss, sizeof(float), 1, binFile) != 1) break;

            // Processa campos variáveis (recriando a lógica exata de leitura do main.c)
            char codigo;
            // Inicializa ponteiros de strings com NULL para evitar acesso a lixo
            registro.country = NULL;
            registro.attackType = NULL;
            registro.targetIndustry = NULL;
            registro.defenseMechanism = NULL;

            // Loop para ler os campos variáveis, parando no final do registro
            while (ftell(binFile) < offsetInicioRegistro + sizeof(char) + sizeof(int) + registro.tamanhoRegistro) {
                if (fread(&codigo, sizeof(char), 1, binFile) != 1) break; // Leitura de um byte de código

                if (codigo == cabecalho.codDescreveContry[0]) {
                    registro.country = lerCampoVariavel(binFile);
                } else if (codigo == cabecalho.codDescreveType[0]) {
                    registro.attackType = lerCampoVariavel(binFile);
                } else if (codigo == cabecalho.codDescreveTargetIndustry[0]) {
                    registro.targetIndustry = lerCampoVariavel(binFile);
                } else if (codigo == cabecalho.codDescreveDefense[0]) {
                    registro.defenseMechanism = lerCampoVariavel(binFile);
                } else {
                    fseek(binFile, -1, SEEK_CUR);
                    break;
                }
            }
            
            // Se o registro atual corresponde aos critérios de busca E não está removido
            if (registroCorresponde(&registro, &cabecalho, filtro)) {
                removerRegistro(binFile, &registro, &cabecalho, offsetInicioRegistro);
                encontrou_e_removeu = 1;
            }

            // Liberar memória alocada para os campos variáveis lidos (se houver)
            if (registro.country != NULL) free(registro.country);
            if (registro.attackType != NULL) free(registro.attackType);
            if (registro.targetIndustry != NULL) free(registro.targetIndustry);
            if (registro.defenseMechanism != NULL) free(registro.defenseMechanism);
        }

        liberarFiltro(filtro); // Libera a memória alocada para o filtro desta iteração
    }

    // 3. Marcar o arquivo como consistente no final
    cabecalho.status = '1';
    fseek(binFile, 0, SEEK_SET); // Volta para o início do arquivo para escrever o cabeçalho
    escreverCabecalho(binFile, &cabecalho); // Escreve o cabeçalho atualizado

    fclose(binFile); // Fecha o arquivo binário

    // 4. Exibe o arquivo na tela conforme especificado
    binarioNaTela(nomeArquivoEntrada);
}

// Função principal que orquestra a criação da árvore-B
void funcionalidade7(char *nomeArquivoDados, char *nomeArquivoIndice) {
    FILE *f_dados = fopen(nomeArquivoDados, "rb");
    if (f_dados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Lê o cabeçalho do arquivo de dados para pegar o status e o fim do arquivo.
    Cabecalho header_dados;
    lerCabecalho(f_dados, &header_dados); // Supondo que você tenha esta função em arquivo.c
    if (header_dados.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(f_dados);
        return;
    }

    FILE *f_indice = fopen(nomeArquivoIndice, "wb+");
    if (f_indice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(f_dados);
        return;
    }

    // 1. Criação e escrita do Cabeçalho inicial do Arquivo de Índice
    BTreeHeader header_indice;
    header_indice.status = '0';
    header_indice.noRaiz = -1;
    header_indice.proxRRN = 0;
    header_indice.nroNos = 0;
    
    fseek(f_indice, 0, SEEK_SET);
    fwrite(&header_indice.status, sizeof(char), 1, f_indice);
    fwrite(&header_indice.noRaiz, sizeof(int), 1, f_indice);
    fwrite(&header_indice.proxRRN, sizeof(int), 1, f_indice);
    fwrite(&header_indice.nroNos, sizeof(int), 1, f_indice);
    char lixo = '$';
    for (int i = 0; i < B_TREE_NODE_SIZE - 13; i++) { // Preenche o resto do nó do cabeçalho
        fwrite(&lixo, sizeof(char), 1, f_indice);
    }

    // =======================================================================================
    // 2. LÓGICA DE LEITURA "REGISTRO POR REGISTRO"
    // =======================================================================================
    long byteOffset = ftell(f_dados); // Posição inicial dos dados (ajuste se seu header de dados for diferente)

    // O laço percorre o arquivo usando o offset, do início até o fim (proxByteOffset)
    while (byteOffset < header_dados.proxByteOffset) {
        // Posiciona o ponteiro no início do registro atual
        fseek(f_dados, byteOffset, SEEK_SET);

        char removido_flag;
        int tamanho_registro;

        // Tenta ler os metadados do registro
        if (fread(&removido_flag, sizeof(char), 1, f_dados) != 1) {
            printf("Erro ao ler o registro no offset %ld\n", byteOffset);
            break; // Fim do arquivo ou erro de leitura
        }
        if (fread(&tamanho_registro, sizeof(int), 1, f_dados) != 1) {
            printf("Erro ao ler o tamanho do registro no offset %ld\n", byteOffset);
            break; // Fim do arquivo ou erro de leitura
        }

        // Se o registro está ativo, processa-o para inserção
        if (removido_flag == '0') {
            int idAttack;
            // Pula o campo 'prox' para chegar até o 'idAttack'
            fseek(f_dados, sizeof(long), SEEK_CUR); 
            // Lê a chave que será inserida no índice
            fread(&idAttack, sizeof(int), 1, f_dados);

            // INSERÇÃO UM-A-UM: Chama a função de inserção na Árvore-B
            // para este registro específico, passando a chave e sua localização (byteOffset).
            b_tree_insert(f_indice, &header_indice, idAttack, byteOffset);
        }

        // Avança o offset para o início do próximo registro
        byteOffset += tamanho_registro+sizeof(char)+sizeof(int); // tamanho do registro + removido_flag + tamanho_registro
    }

    // 3. Finalização
    header_indice.status = '1';
    fseek(f_indice, 0, SEEK_SET);
    fwrite(&header_indice.status, sizeof(char), 1, f_indice);
    fwrite(&header_indice.noRaiz, sizeof(int), 1, f_indice); // Reescreve a raiz final
    fwrite(&header_indice.proxRRN, sizeof(int), 1, f_indice);
    fwrite(&header_indice.nroNos, sizeof(int), 1, f_indice);
    atualizarNoRaiz(f_indice, &header_indice); // Atualiza o nó raiz com os dados finais
    

    fclose(f_dados);
    fclose(f_indice);

    // 4. Exibição
    binarioNaTela(nomeArquivoIndice);
}


void funcionalidade8(char *nomeArquivoDados, char *nomeArquivoIndice, int n, int funcionalidade) {
    // A lógica complexa foi movida para o novo módulo.
    // Esta função agora apenas delega a chamada.
    realizarBuscaComArvoreB(nomeArquivoDados, nomeArquivoIndice, n);
}

void funcionalidade10(char *nomeArquivoDados, char *nomeArquivoIndice, int n) {
    FILE *f_dados = fopen(nomeArquivoDados, "rb+");
    FILE *f_indice = fopen(nomeArquivoIndice, "rb+");

    if (f_dados == NULL || f_indice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        if(f_dados) fclose(f_dados);
        if(f_indice) fclose(f_indice);
        return;
    }


    Cabecalho header_dados ={0};
    BTreeHeader header_indice = {0} ;
    lerCabecalho(f_dados, &header_dados); // Lê cabeçalho do arquivo de dados
    
    ler_b_tree_header(f_indice, &header_indice); // Lê cabeçalho do índice
    
    if (header_dados.status == '0' || header_indice.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(f_dados);
        fclose(f_indice);
        return;
    }

    header_dados.status = '0'; // Marca o arquivo de dados como inconsistente para segurança
    fseek(f_dados, 0, SEEK_SET);
    fwrite(&header_dados.status, sizeof(char), 1, f_dados); // Escreve o status '0' no cabeçalho do arquivo de dados   
    
    header_indice.status = '0'; // Marca o arquivo de índice como inconsistente
    fseek(f_indice, 0, SEEK_SET);
    fwrite(&header_indice.status, sizeof(char), 1, f_indice); // Escreve o status '0' no cabeçalho do arquivo de índice


    for (int i = 0; i < n; i++) {
        RegistroDados *novo_reg = ler_registro_da_entrada();
        if (novo_reg == NULL) {
            continue; // Pula para a próxima iteração se houver erro na leitura do registro
        }

        novo_reg->tamanhoRegistro = calcular_tamanho_novo_registro(novo_reg);
        long byteOffsetInserido = inserir_registro_first_fit(f_dados, novo_reg, &header_dados);

        if (byteOffsetInserido != -1) {
            b_tree_insert(f_indice, &header_indice, novo_reg->idAttack, byteOffsetInserido);
        }

        // Libera memória do registro lido da entrada
        free(novo_reg->country);
        free(novo_reg->attackType);
        free(novo_reg->targetIndustry);
        free(novo_reg->defenseMechanism);
        free(novo_reg);
    
    }

    header_indice.status = '1';
    fseek(f_indice, 0, SEEK_SET);
    fwrite(&header_indice.status, sizeof(char), 1, f_indice);
    fwrite(&header_indice.noRaiz, sizeof(int), 1, f_indice); // Reescreve a raiz final
    fwrite(&header_indice.proxRRN, sizeof(int), 1, f_indice);
    fwrite(&header_indice.nroNos, sizeof(int), 1, f_indice);
    
    header_dados.status = '1';
    fseek(f_dados, 0, SEEK_SET); 
    escreverCabecalho(f_dados, &header_dados); // Marca o arquivo de dados como consistente

    // Exibe o conteúdo do arquivo de dados

    fclose(f_dados);
    fclose(f_indice);

    binarioNaTela(nomeArquivoDados);
    binarioNaTela(nomeArquivoIndice);
}

void funcionalidade11(char *nomeArquivoDados, char *nomeArquivoIndice, int n_operacoes) {
    // Abrir o arquivo de dados em modo de leitura e escrita binária (rb+).
    // Isso permite ler e escrever no arquivo, e o ponteiro de arquivo pode ser movido livremente.
    FILE *f_dados = fopen(nomeArquivoDados, "rb+");
    // Abrir o arquivo de índice da Árvore-B em modo de leitura e escrita binária (rb+).
    FILE *f_indice = fopen(nomeArquivoIndice, "rb+");

    // Verificar se os arquivos foram abertos com sucesso.
    if (f_dados == NULL || f_indice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        // Fechar os arquivos que foram abertos, se houver.
        if (f_dados) fclose(f_dados);
        if (f_indice) fclose(f_indice);
        return; // Retorna em caso de falha.
    }

    // Declaração das structs de cabeçalho para o arquivo de dados e para a Árvore-B.
    Cabecalho header_dados;
    BTreeHeader header_indice;

    // Ler os cabeçalhos iniciais dos arquivos para verificar sua consistência.
    // Lendo o cabeçalho completo do arquivo de dados.
    lerCabecalho(f_dados, &header_dados);
    // Lendo o cabeçalho completo do arquivo de índice da Árvore-B.
    fseek(f_indice, 0, SEEK_SET); // Posiciona no início do arquivo de índice.
    fread(&header_indice.status, sizeof(char), 1, f_indice);
    fread(&header_indice.noRaiz, sizeof(int), 1, f_indice);
    fread(&header_indice.proxRRN, sizeof(int), 1, f_indice);
    fread(&header_indice.nroNos, sizeof(int), 1, f_indice);

    // Verificar se algum dos arquivos está inconsistente (status '0').
    if (header_dados.status == '0' || header_indice.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(f_dados);
        fclose(f_indice);
        return;
    }

    // 1. Marcar ambos os arquivos como INCONSISTENTES (status '0') para garantir a integridade em caso de falha.
    header_dados.status = '0'; // Define o status do cabeçalho de dados como inconsistente.
    fseek(f_dados, 0, SEEK_SET); // Volta para o início do arquivo de dados.
    // Escreve apenas o byte do status para marcar rapidamente como inconsistente.
    fwrite(&header_dados.status, sizeof(char), 1, f_dados);
    fflush(f_dados); // Garante que a escrita do status seja efetivada no disco.

    header_indice.status = '0'; // Define o status do cabeçalho de índice como inconsistente.
    fseek(f_indice, 0, SEEK_SET); // Volta para o início do arquivo de índice.
    // Escreve apenas o byte do status para marcar rapidamente como inconsistente.
    fwrite(&header_indice.status, sizeof(char), 1, f_indice);
    // É uma boa prática reescrever o cabeçalho completo da B-Tree após modificar o status.
    fseek(f_indice, 0, SEEK_SET); // Volta para o início para reescrever todo o cabeçalho.
    fwrite(&header_indice.status, sizeof(char), 1, f_indice);
    fwrite(&header_indice.noRaiz, sizeof(int), 1, f_indice);
    fwrite(&header_indice.proxRRN, sizeof(int), 1, f_indice);
    fwrite(&header_indice.nroNos, sizeof(int), 1, f_indice);
    fflush(f_indice); // Garante que a escrita do status seja efetivada no disco.

    // Loop principal para processar 'n_operacoes' de atualização.
    for (int i = 0; i < n_operacoes; i++) {
        // Passo 3: Leitura dos Critérios de Busca (m) e Dados de Atualização (p).
        Filtro *criterios_busca = lerFiltroEntrada();    // Lê os critérios que definem quais registros buscar.
        Filtro *dados_atualizacao = lerFiltroEntrada(); // Lê os dados que devem ser atualizados nos registros encontrados.

        // Verificar se a leitura dos filtros foi bem-sucedida.
        if (criterios_busca == NULL || dados_atualizacao == NULL) {
            fprintf(stderr, "Erro ao ler filtros para a operacao %d. Pulando.\n", i + 1);
            // Libera a memória alocada para os filtros, mesmo que um deles seja NULL (a função gerencia).
            liberarFiltro(criterios_busca);
            liberarFiltro(dados_atualizacao);
            continue; // Pula para a próxima operação 'n' se houver erro na leitura.
        }

        // --- Início do percorrimento do arquivo de dados para encontrar registros ---
        // Sempre releia o cabeçalho do arquivo de dados antes de iniciar uma nova busca/percorrimento.
        // O cabeçalho pode ter sido modificado por operações de remoção ou inserção de um registro anterior,
        // afetando 'topo', 'nroRegArq', 'proxByteOffset', etc.
        fseek(f_dados, 0, SEEK_SET);
        lerCabecalho(f_dados, &header_dados);

        // 'byteOffsetAtual' controla a posição no arquivo de dados. Começa após o cabeçalho.
        long byteOffsetAtual = ftell(f_dados);

        // Percorre o arquivo de dados até o 'proxByteOffset' (o final lógico do arquivo).
        while (byteOffsetAtual < header_dados.proxByteOffset) {
            fseek(f_dados, byteOffsetAtual, SEEK_SET); // Posiciona no início do possível registro.

            RegistroDados registro_lido = {0}; // Inicializa a struct para armazenar o registro lido (evita lixo).
            long offsetInicioRegistro = ftell(f_dados); // Salva o offset inicial do registro atual.

            // Tenta ler o byte de 'removido' e o tamanho do registro.
            if (fread(&registro_lido.removido, sizeof(char), 1, f_dados) != 1) break;
            if (fread(&registro_lido.tamanhoRegistro, sizeof(int), 1, f_dados) != 1) break;

            // Calcula o próximo offset para o loop 'while'.
            // Isso permite pular para o próximo registro, independentemente de ser processado ou não.
            long proximo_byteOffset_registro_lido = offsetInicioRegistro + sizeof(char) + sizeof(int) + registro_lido.tamanhoRegistro;

            // Se o registro está logicamente removido ('1'), pule-o e vá para o próximo.
            if (registro_lido.removido == '1') {
                byteOffsetAtual = proximo_byteOffset_registro_lido;
                continue; // Volta para o início do loop 'while'.
            }

            // Lê o restante dos campos fixos do registro.
            if (fread(&registro_lido.prox, sizeof(long int), 1, f_dados) != 1) break;
            if (fread(&registro_lido.idAttack, sizeof(int), 1, f_dados) != 1) break;
            if (fread(&registro_lido.year, sizeof(int), 1, f_dados) != 1) break;
            if (fread(&registro_lido.financialLoss, sizeof(float), 1, f_dados) != 1) break;

            // Processa campos variáveis:
            // IMPORTANTE: Inicializar os ponteiros de string com NULL e alocar memória dinamicamente para eles.
            registro_lido.country = NULL;
            registro_lido.attackType = NULL;
            registro_lido.targetIndustry = NULL;
            registro_lido.defenseMechanism = NULL;

            long pos_leitura_variavel = ftell(f_dados); // Salva a posição atual para leitura de campos variáveis.
            // Loop para ler os campos variáveis até o final do espaço alocado para o registro.
            while (pos_leitura_variavel < offsetInicioRegistro + sizeof(char) + sizeof(int) + registro_lido.tamanhoRegistro) {
                fseek(f_dados, pos_leitura_variavel, SEEK_SET); // Posiciona para ler o código do campo.
                char codigo_campo;
                if (fread(&codigo_campo, sizeof(char), 1, f_dados) != 1) break; // Lê o código do campo.

                // Identifica o campo pelo código e chama a função para ler o valor variável.
                if (codigo_campo == header_dados.codDescreveContry[0]) {
                    registro_lido.country = lerCampoVariavel(f_dados);
                } else if (codigo_campo == header_dados.codDescreveType[0]) {
                    registro_lido.attackType = lerCampoVariavel(f_dados);
                } else if (codigo_campo == header_dados.codDescreveTargetIndustry[0]) {
                    registro_lido.targetIndustry = lerCampoVariavel(f_dados);
                } else if (codigo_campo == header_dados.codDescreveDefense[0]) {
                    registro_lido.defenseMechanism = lerCampoVariavel(f_dados);
                } else {
                    // Se o caractere lido não é um código de campo válido,
                    // assumimos que chegamos ao lixo ou ao final dos campos válidos.
                    fseek(f_dados, -1, SEEK_CUR); // Volta o ponteiro para não perder o byte não reconhecido.
                    break; // Sai do loop de leitura de campos variáveis.
                }
                pos_leitura_variavel = ftell(f_dados); // Atualiza a posição para o próximo campo variável.
            }

            // === Ponto chave: Verifica se o registro lido corresponde aos critérios de busca ===
            if (registroCorresponde(&registro_lido, &header_dados, criterios_busca)) {
                // Passo 5: Lógica de Atualização (In-place vs. Remove & Insert).

                // Crie uma cópia "profunda" (deep copy) do registro lido.
                // Isso é crucial porque 'atualizar_campos_do_registro' pode `free` e `strdup` strings,
                // invalidando os ponteiros originais de 'registro_lido' antes da liberação final.
                RegistroDados registro_atualizado_copia = registro_lido; // Faz uma cópia superficial (shallow copy) inicial.
                // Copia as strings uma a uma para novas alocações de memória.
                registro_atualizado_copia.country = (registro_lido.country != NULL) ? strdup(registro_lido.country) : NULL;
                registro_atualizado_copia.attackType = (registro_lido.attackType != NULL) ? strdup(registro_lido.attackType) : NULL;
                registro_atualizado_copia.targetIndustry = (registro_lido.targetIndustry != NULL) ? strdup(registro_lido.targetIndustry) : NULL;
                registro_atualizado_copia.defenseMechanism = (registro_lido.defenseMechanism != NULL) ? strdup(registro_lido.defenseMechanism) : NULL;

                // Aplica as atualizações nos campos da CÓPIA (registro_atualizado_copia).
                // Esta função modificará os campos da cópia com base em 'dados_atualizacao'.
                atualizar_campos_do_registro(&registro_atualizado_copia, dados_atualizacao);

                // Calcula o novo tamanho lógico que o registro ocuparia após as alterações.
                int novo_tamanho_logico = calcular_tamanho_novo_registro(&registro_atualizado_copia);

                // --- Cenário 1: Novo tamanho MENOR ou IGUAL ao tamanho original ---
                // Neste caso, a atualização é feita "in-place" (no mesmo local do arquivo).
                if (novo_tamanho_logico <= registro_lido.tamanhoRegistro) {
                    fseek(f_dados, offsetInicioRegistro, SEEK_SET); // Volta para o início do registro a ser atualizado.

                    // Reescreve o cabeçalho do registro (flag de removido e tamanhoRegistro original).
                    // A flag deve ser '0' (ativo), e o tamanho é o tamanho ORIGINAL do espaço.
                    fwrite(&registro_lido.removido, sizeof(char), 1, f_dados); // Reafirma '0' (não removido).
                    fwrite(&registro_lido.tamanhoRegistro, sizeof(int), 1, f_dados); // Mantém o tamanho do espaço original.

                    // Escreve os campos fixos da cópia atualizada.
                    fwrite(&registro_atualizado_copia.prox, sizeof(long int), 1, f_dados);
                    fwrite(&registro_atualizado_copia.idAttack, sizeof(int), 1, f_dados);
                    fwrite(&registro_atualizado_copia.year, sizeof(int), 1, f_dados);
                    fwrite(&registro_atualizado_copia.financialLoss, sizeof(float), 1, f_dados);

                    // Escreve os campos variáveis da cópia atualizada.
                    if (registro_atualizado_copia.country) {
                        fwrite(header_dados.codDescreveContry, sizeof(char), 1, f_dados);
                        fwrite(registro_atualizado_copia.country, strlen(registro_atualizado_copia.country), 1, f_dados);
                        fwrite("|", sizeof(char), 1, f_dados);
                    }
                    if (registro_atualizado_copia.attackType) {
                        fwrite(header_dados.codDescreveType, sizeof(char), 1, f_dados);
                        fwrite(registro_atualizado_copia.attackType, strlen(registro_atualizado_copia.attackType), 1, f_dados);
                        fwrite("|", sizeof(char), 1, f_dados);
                    }
                    if (registro_atualizado_copia.targetIndustry) {
                        fwrite(header_dados.codDescreveTargetIndustry, sizeof(char), 1, f_dados);
                        fwrite(registro_atualizado_copia.targetIndustry, strlen(registro_atualizado_copia.targetIndustry), 1, f_dados);
                        fwrite("|", sizeof(char), 1, f_dados);
                    }
                    if (registro_atualizado_copia.defenseMechanism) {
                        fwrite(header_dados.codDescreveDefense, sizeof(char), 1, f_dados);
                        fwrite(registro_atualizado_copia.defenseMechanism, strlen(registro_atualizado_copia.defenseMechanism), 1, f_dados);
                        fwrite("|", sizeof(char), 1, f_dados);
                    }

                    // Preenche o espaço restante (lixo) com '$'.
                    // Calcula quantos bytes foram realmente escritos do novo registro.
                    int bytes_escritos_no_registro = ftell(f_dados) - offsetInicioRegistro - sizeof(char) - sizeof(int);
                    int lixo = registro_lido.tamanhoRegistro - bytes_escritos_no_registro; // Espaço restante no registro.
                    for (int k = 0; k < lixo; k++) {
                        fwrite("$", 1, 1, f_dados); // Preenche com '$'.
                    }
                    fflush(f_dados); // Garante que a escrita seja efetivada.

                    // Libera a memória alocada para as strings da cópia, pois elas não são mais necessárias.
                    if (registro_atualizado_copia.country != NULL) free(registro_atualizado_copia.country);
                    if (registro_atualizado_copia.attackType != NULL) free(registro_atualizado_copia.attackType);
                    if (registro_atualizado_copia.targetIndustry != NULL) free(registro_atualizado_copia.targetIndustry);
                    if (registro_atualizado_copia.defenseMechanism != NULL) free(registro_atualizado_copia.defenseMechanism);

                    // Não é necessário atualizar a Árvore-B, pois o byte offset do registro não mudou.

                } else { // --- Cenário 2: Novo tamanho MAIOR que o tamanho original ---
                    // Neste caso, o registro original é logicamente removido e o novo é inserido em outro local.

                    // 1. Remover logicamente o registro antigo.
                    // A função 'removerRegistro' já atualiza a flag de removido no arquivo,
                    // o campo 'prox' do registro removido, e o cabeçalho do arquivo de dados (topo, nroRegArq, nroRegRem).
                    removerRegistro(f_dados, &registro_lido, &header_dados, offsetInicioRegistro);
                    // IMPORTANTÍSSIMO: 'removerRegistro' deve reescrever o cabeçalho no arquivo de dados.

                    // 2. Inserir o novo registro atualizado usando a estratégia First Fit.
                    // 'inserir_registro_first_fit' calcula o espaço necessário, encontra um local (ou insere no final),
                    // e atualiza o cabeçalho do arquivo de dados ('topo', 'nroRegArq', 'proxByteOffset').
                    registro_atualizado_copia.tamanhoRegistro = novo_tamanho_logico; // Define o tamanho real do novo registro.
                    long novo_byte_offset = inserir_registro_first_fit(f_dados, &registro_atualizado_copia, &header_dados);
                    // IMPORTANTÍSSIMO: 'inserir_registro_first_fit' deve reescrever o cabeçalho no arquivo de dados.

                    // 3. Atualização do Índice Árvore-B.
                    // O registro (identificado pelo idAttack) agora está em um novo local físico (novo_byte_offset).
                    // Precisamos atualizar o ponteiro de referência (PR) na Árvore-B.
                    if (novo_byte_offset != -1) { // Verifica se a inserção do novo registro foi bem-sucedida.
                        // Chama a nova função para atualizar o PR da chave (idAttack) para o novo byte offset.
                        b_tree_update_pr(f_indice, registro_atualizado_copia.idAttack, novo_byte_offset);
                    } else {
                        fprintf(stderr, "Erro: Falha critica na insercao do registro atualizado (tamanho maior).\n");
                    }

                    // Após qualquer modificação na Árvore-B (mesmo que apenas um PR),
                    // reescreva o cabeçalho do arquivo de índice.
                    fseek(f_indice, 0, SEEK_SET);
                    fwrite(&header_indice.status, sizeof(char), 1, f_indice);
                    fwrite(&header_indice.noRaiz, sizeof(int), 1, f_indice);
                    fwrite(&header_indice.proxRRN, sizeof(int), 1, f_indice);
                    fwrite(&header_indice.nroNos, sizeof(int), 1, f_indice);
                    fflush(f_indice); // Garante que a escrita seja efetivada.

                    // Libera a memória alocada para as strings da cópia.
                    if (registro_atualizado_copia.country != NULL) free(registro_atualizado_copia.country);
                    if (registro_atualizado_copia.attackType != NULL) free(registro_atualizado_copia.attackType);
                    if (registro_atualizado_copia.targetIndustry != NULL) free(registro_atualizado_copia.targetIndustry);
                    if (registro_atualizado_copia.defenseMechanism != NULL) free(registro_atualizado_copia.defenseMechanism);
                }
                // Fim da lógica de atualização para um único registro que correspondeu.
            } // Fim do if (registroCorresponde(...))

            // Liberação de memória para os campos variáveis do 'registro_lido'.
            // Esta struct é usada para a leitura de CADA registro no loop 'while',
            // então suas strings devem ser liberadas a cada iteração.
            if (registro_lido.country != NULL) free(registro_lido.country);
            if (registro_lido.attackType != NULL) free(registro_lido.attackType);
            if (registro_lido.targetIndustry != NULL) free(registro_lido.targetIndustry);
            if (registro_lido.defenseMechanism != NULL) free(registro_lido.defenseMechanism);

            // Avance para o próximo registro no arquivo de dados.
            // Isso garante que o loop 'while' continue corretamente, mesmo se o registro atual foi "movido".
            byteOffsetAtual = proximo_byteOffset_registro_lido;
        } // Fim do while (byteOffsetAtual < header_dados.proxByteOffset)

        // Libera a memória alocada para os filtros de busca e atualização da operação 'n' atual.
        liberarFiltro(criterios_busca);
        liberarFiltro(dados_atualizacao);
    } // Fim do loop for (n_operacoes)

    // 2. Marcar ambos os arquivos como CONSISTENTES (status '1') no final, se todas as operações foram bem-sucedidas.
    header_dados.status = '1'; // Define o status como consistente.
    fseek(f_dados, 0, SEEK_SET); // Volta para o início do arquivo de dados.
    // Reescreve o cabeçalho completo do arquivo de dados.
    escreverCabecalho(f_dados, &header_dados);
    fflush(f_dados);

    header_indice.status = '1'; // Define o status como consistente.
    fseek(f_indice, 0, SEEK_SET); // Volta para o início do arquivo de índice.
    // Reescreve o cabeçalho completo do arquivo de índice.
    fwrite(&header_indice.status, sizeof(char), 1, f_indice);
    fwrite(&header_indice.noRaiz, sizeof(int), 1, f_indice);
    fwrite(&header_indice.proxRRN, sizeof(int), 1, f_indice);
    fwrite(&header_indice.nroNos, sizeof(int), 1, f_indice);
    fflush(f_indice);

    // Fechar os arquivos.
    fclose(f_dados);
    fclose(f_indice);

    // 3. Exibir o conteúdo dos arquivos na tela, conforme especificado.
    binarioNaTela(nomeArquivoDados);
    binarioNaTela(nomeArquivoIndice);
}