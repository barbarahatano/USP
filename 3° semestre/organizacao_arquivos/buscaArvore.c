#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buscaArvore.h"
#include "arvoreB.h"
#include "arquivo.h"
#include "busca.h"
#include "exibicao.h"

// Constante definida localmente, pois é privada em arvoreB.c
#define B_TREE_HEADER_PAGE_SIZE 44

// Estrutura privada para passar informações durante a travessia da árvore.
typedef struct {
    FILE *f_dados;
    Cabecalho *header_dados;
    Filtro *filtro;
    int *encontrou_flag;
} SearchContext;


static void verificar_registro_e_filtrar(long byteOffset, void *context_ptr) {
    SearchContext *ctx = (SearchContext *)context_ptr;
    fseek(ctx->f_dados, byteOffset, SEEK_SET);
    
    RegistroDados reg = {0};
    long offsetInicioRegistro = byteOffset;

    fread(&reg.removido, sizeof(char), 1, ctx->f_dados);
    fread(&reg.tamanhoRegistro, sizeof(int), 1, ctx->f_dados);

    if (reg.removido == '1') return;

    fread(&reg.prox, sizeof(long int), 1, ctx->f_dados);
    fread(&reg.idAttack, sizeof(int), 1, ctx->f_dados);
    fread(&reg.year, sizeof(int), 1, ctx->f_dados);
    fread(&reg.financialLoss, sizeof(float), 1, ctx->f_dados);

    reg.country = NULL;
    reg.attackType = NULL;
    reg.targetIndustry = NULL;
    reg.defenseMechanism = NULL;

    char codigo;
    while (ftell(ctx->f_dados) < offsetInicioRegistro + sizeof(char) + sizeof(int) + reg.tamanhoRegistro) {
        if (fread(&codigo, sizeof(char), 1, ctx->f_dados) != 1) break;
        if (codigo == ctx->header_dados->codDescreveContry[0]) reg.country = lerCampoVariavel(ctx->f_dados);
        else if (codigo == ctx->header_dados->codDescreveType[0]) reg.attackType = lerCampoVariavel(ctx->f_dados);
        else if (codigo == ctx->header_dados->codDescreveTargetIndustry[0]) reg.targetIndustry = lerCampoVariavel(ctx->f_dados);
        else if (codigo == ctx->header_dados->codDescreveDefense[0]) reg.defenseMechanism = lerCampoVariavel(ctx->f_dados);
        else { fseek(ctx->f_dados, -1, SEEK_CUR); break; }
    }

    if (registroCorresponde(&reg, ctx->header_dados, ctx->filtro)) {
        exibirRegistro(reg, *(ctx->header_dados));
        *(ctx->encontrou_flag) = 1;
    }

    if (reg.country) free(reg.country);
    if (reg.attackType) free(reg.attackType);
    if (reg.targetIndustry) free(reg.targetIndustry);
    if (reg.defenseMechanism) free(reg.defenseMechanism);
}

// Lógica de travessia da árvore, implementada localmente.
static void busca_traverse_recursive(FILE *f_indice, int rrn_no_atual, void (*visitor_func)(long, void *), void *context_ptr) {
    if (rrn_no_atual == -1) return;

    BTreeNode *no = read_b_tree_node(f_indice, rrn_no_atual);
    if (no == NULL) return;

    for (int i = 0; i < no->nroChaves; i++) {
        busca_traverse_recursive(f_indice, no->P[i], visitor_func, context_ptr);
        visitor_func(no->PR[i], context_ptr);
    }
    busca_traverse_recursive(f_indice, no->P[no->nroChaves], visitor_func, context_ptr);

    free(no);
}

// Implementação da função pública que orquestra as buscas.
void realizarBuscaComArvoreB(char *nomeArquivoDados, char *nomeArquivoIndice, int n) {
    FILE *f_dados = fopen(nomeArquivoDados, "rb");
    FILE *f_indice = fopen(nomeArquivoIndice, "rb");

    if (f_dados == NULL || f_indice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        if(f_dados) fclose(f_dados);
        if(f_indice) fclose(f_indice);
        return;
    }

    Cabecalho header_dados;
    BTreeHeader header_indice;
    lerCabecalho(f_dados, &header_dados);
    ler_b_tree_header(f_indice, &header_indice); // Esta função é pública em arvoreB.h
    
    if (header_dados.status == '0' || header_indice.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(f_dados);
        fclose(f_indice);
        return;
    }

    for (int i = 0; i < n; i++) {
        Filtro *filtro = lerFiltroEntrada();
        if (filtro == NULL) {
            printf("Registro inexistente.\n\n");
            if (n > 1 && i < n - 1) printf("**********\n");
            continue;
        }

        int encontrou_nesta_busca = 0;
        SearchContext contexto_busca = {f_dados, &header_dados, filtro, &encontrou_nesta_busca};

        // Inicia a travessia recursiva a partir do nó raiz
        if (header_indice.noRaiz != -1) {
            busca_traverse_recursive(f_indice, header_indice.noRaiz, &verificar_registro_e_filtrar, &contexto_busca);
        }

        if (!encontrou_nesta_busca) {
            printf("Registro inexistente.\n\n");
        }
        
        
        printf("**********\n");
        

        liberarFiltro(filtro);
    }

    fclose(f_dados);
    fclose(f_indice);
}