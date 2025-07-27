#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvoreB.h"
#include "arquivo.h"
#include "exibicao.h"

#define B_TREE_HEADER_PAGE_SIZE 44 // Usando o mesmo tamanho de um nó por simplicidade


 //Protótipos de funções estáticas (privadas para este arquivo)
static void write_b_tree_node(FILE *indexFile, int rrn, BTreeNode *node);
static int b_tree_insert_recursive(FILE *f_indice, BTreeHeader *header, int RRN_ATUAL, int CHAVE_A_INSERIR, long REF_DADOS_A_INSERIR, int *CHAVE_PROMOVIDA, long *REF_PROMOVIDA);
static int split_node(FILE* f_indice, BTreeHeader* header, int rrn_cheio, int chave_extra, long ref_extra, int filho_dir_extra, int *chave_promovida, long *ref_promovida);
static void insert_key_into_node(BTreeNode* node, int chave, long referencia, int filho_direita_rrn);



int ler_b_tree_header(FILE *f_indice, BTreeHeader *header) {
    // Verifica se os ponteiros de arquivo e da struct são válidos.
    if (f_indice == NULL || header == NULL) {
        return 0; // Retorna falha
    }

    // Posiciona o ponteiro de leitura no início do arquivo (byte 0).
    fseek(f_indice, 0, SEEK_SET);

    // Lê os campos do cabeçalho na ordem especificada.
    // A função fread retorna o número de itens lidos com sucesso.
    // Se a leitura de qualquer campo falhar, a função retorna 0.
    if (fread(&header->status, sizeof(char), 1, f_indice) != 1) return 0;
    if (fread(&header->noRaiz, sizeof(int), 1, f_indice) != 1) return 0;
    if (fread(&header->proxRRN, sizeof(int), 1, f_indice) != 1) return 0;
    if (fread(&header->nroNos, sizeof(int), 1, f_indice) != 1) return 0;

    // Se todas as leituras foram bem-sucedidas, retorna sucesso.
    return 1;
}

// Função "pública" de inserção
void b_tree_insert(FILE *f_indice, BTreeHeader *header, int chave, long ref_dados) {
    // Caso 1: A árvore está vazia. 
    if (header->noRaiz == -1) {
        BTreeNode *raiz = create_b_tree_node();
        raiz->tipoNo = -1; // É raiz e folha. 
        raiz->nroChaves = 1;
        raiz->C[0] = chave;
        raiz->PR[0] = ref_dados;
        
        header->noRaiz = 0;
        header->nroNos = 1;
        header->proxRRN = 1;
        
        write_b_tree_node(f_indice, header->noRaiz, raiz);
        free(raiz);
        return;
    }

    int chave_promovida;
    long ref_promovida;
    int filho_dir_promovido_rrn = b_tree_insert_recursive(f_indice, header, header->noRaiz, chave, ref_dados, &chave_promovida, &ref_promovida);

    // Caso 2: A inserção recursiva causou um split na raiz. 
    if (filho_dir_promovido_rrn != -1) {
        BTreeNode *nova_raiz = create_b_tree_node();
        nova_raiz->tipoNo = 1; // Agora é um nó raiz interno.
        nova_raiz->nroChaves = 1;
        nova_raiz->C[0] = chave_promovida;
        nova_raiz->PR[0] = ref_promovida;
        nova_raiz->P[0] = header->noRaiz; // A antiga raiz vira o filho esquerdo
        nova_raiz->P[1] = filho_dir_promovido_rrn; // O novo nó do split vira o filho direito
        
        header->noRaiz = header->proxRRN;
        header->nroNos++;
        header->proxRRN++;

        

        write_b_tree_node(f_indice, header->noRaiz, nova_raiz);
        free(nova_raiz);
    }
}

// Função recursiva que desce na árvore e insere
static int b_tree_insert_recursive(FILE *f_indice, BTreeHeader *header, int RRN_ATUAL, int CHAVE_A_INSERIR, long REF_DADOS_A_INSERIR, int *CHAVE_PROMOVIDA, long *REF_PROMOVIDA) {
    BTreeNode *no_atual = read_b_tree_node(f_indice, RRN_ATUAL);
    if (no_atual == NULL) return -1; // Erro

    // Caso Base: Nó é folha.
    if (no_atual->tipoNo == -1) {
        if (no_atual->nroChaves < M_ORDER - 1) { // Se cabe na folha
            insert_key_into_node(no_atual, CHAVE_A_INSERIR, REF_DADOS_A_INSERIR, -1);
            write_b_tree_node(f_indice, RRN_ATUAL, no_atual);
            free(no_atual);
            return -1; // Sem promoção
        } else { // Overflow: precisa de split
            int rrn_novo_no = split_node(f_indice, header, RRN_ATUAL, CHAVE_A_INSERIR, REF_DADOS_A_INSERIR, -1, CHAVE_PROMOVIDA, REF_PROMOVIDA);
            free(no_atual);
            return rrn_novo_no;
        }
    } 
    // Caso Recursivo: Nó é interno.
    else {
        int i = 0;
        while (i < no_atual->nroChaves && CHAVE_A_INSERIR > no_atual->C[i]) {
            i++;
        }
        int rrn_filho_a_descer = no_atual->P[i];

        int chave_retornada;
        long ref_retornada;
        int filho_dir_retornado_rrn = b_tree_insert_recursive(f_indice, header, rrn_filho_a_descer, CHAVE_A_INSERIR, REF_DADOS_A_INSERIR, &chave_retornada, &ref_retornada);

        if (filho_dir_retornado_rrn != -1) { // Se o filho fez split e promoveu uma chave
            if (no_atual->nroChaves < M_ORDER - 1) { // Se a chave promovida cabe neste nó
                insert_key_into_node(no_atual, chave_retornada, ref_retornada, filho_dir_retornado_rrn);
                write_b_tree_node(f_indice, RRN_ATUAL, no_atual);
                free(no_atual);
                return -1;
            } else { // Overflow no nó interno: precisa de split também
                int rrn_novo_no = split_node(f_indice, header, RRN_ATUAL, chave_retornada, ref_retornada, filho_dir_retornado_rrn, CHAVE_PROMOVIDA, REF_PROMOVIDA);
                free(no_atual);
                return rrn_novo_no;
            }
        }
    }

    free(no_atual);
    return -1; // Sem promoção
}

static void insert_key_into_node(BTreeNode* node, int chave, long referencia, int filho_dir_rrn) {
    int pos = node->nroChaves;

    // 1. Encontra a posição correta para a nova chave, empurrando as chaves e ponteiros maiores para a direita.
    while (pos > 0 && chave < node->C[pos - 1]) {
        node->C[pos] = node->C[pos - 1];
        node->PR[pos] = node->PR[pos - 1];
        node->P[pos + 1] = node->P[pos];
        pos--;
    }

    // 2. Insere a chave, a referência e o ponteiro do filho direito na posição encontrada.
    node->C[pos] = chave;
    node->PR[pos] = referencia;
    node->P[pos + 1] = filho_dir_rrn; // O novo nó sempre é o filho DIREITO da chave inserida.
    node->nroChaves++;
}


/*
 * Divide um nó cheio (rrn_cheio) após a tentativa de inserir uma nova chave (chave_extra).
 * Promove a chave do meio para o nível superior.
 */
static int split_node(FILE* f_indice, BTreeHeader* header, int rrn_cheio, int chave_extra, long ref_extra, int filho_dir_extra, int *chave_promovida, long *ref_promovida) {
    // 1. Cria arrays temporários que comportam uma chave e um ponteiro a mais.
    int chaves_temp[M_ORDER];
    long refs_temp[M_ORDER];
    int filhos_temp[M_ORDER + 1];

    // 2. Lê o nó que está cheio do disco.
    BTreeNode *no_cheio = read_b_tree_node(f_indice, rrn_cheio);

    // 3. Copia as chaves e ponteiros do nó cheio para os arrays temporários.
    memcpy(chaves_temp, no_cheio->C, (M_ORDER - 1) * sizeof(int));
    memcpy(refs_temp, no_cheio->PR, (M_ORDER - 1) * sizeof(long));
    memcpy(filhos_temp, no_cheio->P, M_ORDER * sizeof(int));

    // 4. Insere a chave extra e seu filho direito nos arrays temporários, mantendo a ordem.
    //    (Esta lógica é a mesma de insert_key_into_node, mas nos arrays temporários).
    int pos = no_cheio->nroChaves;
    while (pos > 0 && chave_extra < chaves_temp[pos - 1]) {
        chaves_temp[pos] = chaves_temp[pos - 1];
        refs_temp[pos] = refs_temp[pos - 1];
        filhos_temp[pos + 1] = filhos_temp[pos];
        pos--;
    }
    chaves_temp[pos] = chave_extra;
    refs_temp[pos] = ref_extra;
    filhos_temp[pos + 1] = filho_dir_extra;

    // 5. A chave do meio dos arrays temporários será promovida.
    int meio_idx = M_ORDER / 2;
    *chave_promovida = chaves_temp[meio_idx];
    *ref_promovida = refs_temp[meio_idx];

    // 6. Cria um novo nó que será o irmão direito do nó que sofreu split.
    BTreeNode *novo_no_direita = create_b_tree_node();
    novo_no_direita->tipoNo = no_cheio->tipoNo;
    int rrn_novo_no = header->proxRRN++; // Pega o próximo RRN livre e o incrementa no cabeçalho.
    header->nroNos++;

    // 7. Redistribui os dados dos arrays temporários para os dois nós.
    // O nó antigo (esquerdo) conterá as chaves à esquerda da chave promovida.
    no_cheio->nroChaves = meio_idx;
    memcpy(no_cheio->C, chaves_temp, meio_idx * sizeof(int));
    memcpy(no_cheio->PR, refs_temp, meio_idx * sizeof(long));
    memcpy(no_cheio->P, filhos_temp, (meio_idx + 1) * sizeof(int));

    // O novo nó (direito) conterá as chaves à direita da chave promovida.
    novo_no_direita->nroChaves = (M_ORDER - 1) - meio_idx;
    memcpy(novo_no_direita->C, &chaves_temp[meio_idx + 1], novo_no_direita->nroChaves * sizeof(int));
    memcpy(novo_no_direita->PR, &refs_temp[meio_idx + 1], novo_no_direita->nroChaves * sizeof(long));
    memcpy(novo_no_direita->P, &filhos_temp[meio_idx + 1], (novo_no_direita->nroChaves + 1) * sizeof(int));

    // 8. Limpa o lixo (chaves e ponteiros antigos) do nó esquerdo.
    for (int i = no_cheio->nroChaves; i < M_ORDER - 1; i++) {
        no_cheio->C[i] = -1;
        no_cheio->PR[i] = -1;
    }
    for (int i = no_cheio->nroChaves + 1; i < M_ORDER; i++) {
        no_cheio->P[i] = -1;
    }

    // 9. Escreve os dois nós (o antigo modificado e o novo) de volta para o disco.
    write_b_tree_node(f_indice, rrn_cheio, no_cheio);
    write_b_tree_node(f_indice, rrn_novo_no, novo_no_direita);

    // 10. Libera a memória alocada.
    free(no_cheio);
    free(novo_no_direita);

    // 11. Retorna o RRN do novo nó criado para que o pai possa apontar para ele.
    return rrn_novo_no;
}

void atualizarNoRaiz(FILE *f_indice, BTreeHeader *header) {
    BTreeNode *raiz = read_b_tree_node(f_indice, header->noRaiz); // Lê o nó raiz 

    // Atualiza o tipo do nó raiz
    raiz->tipoNo = 0;

    // Escreve o nó raiz atualizado de volta no disco
    write_b_tree_node(f_indice, header->noRaiz, raiz);

    // Libera a memória alocada para o nó raiz
    free(raiz);
}

// ---- Funções de I/O e Alocação ----

BTreeNode* create_b_tree_node() {
    BTreeNode *node = (BTreeNode*) calloc(1, sizeof(BTreeNode));
    node->tipoNo = -1;
    node->nroChaves = 0;
    for(int i = 0; i < M_ORDER - 1; i++) {
        node->C[i] = -1;
        node->PR[i] = -1;
    }
    for(int i = 0; i < M_ORDER; i++) {
        node->P[i] = -1;
    }
    return node;
}

/*
 * Função corrigida para escrever um nó da Árvore-B no arquivo de índice,
 * seguindo a estrutura intercalada P, C, PR especificada no PDF do trabalho.
 */
static void write_b_tree_node(FILE *indexFile, int rrn, BTreeNode *node) {
    // Posiciona o ponteiro no local correto para escrita do nó
    fseek(indexFile, (rrn * B_TREE_NODE_SIZE) + B_TREE_HEADER_PAGE_SIZE, SEEK_SET);

    // Escreve os campos fixos iniciais: tipoNo e nroChaves
    fwrite(&node->tipoNo, sizeof(int), 1, indexFile);
    fwrite(&node->nroChaves, sizeof(int), 1, indexFile);

    // Escreve os campos de forma intercalada: (P, C, PR)
    // A ordem da árvore é 3, então temos no máximo 2 chaves e 3 ponteiros.
    for (int i = 0; i < M_ORDER - 1; i++) {
        fwrite(&node->P[i], sizeof(int), 1, indexFile);
        fwrite(&node->C[i], sizeof(int), 1, indexFile);
        fwrite(&node->PR[i], sizeof(long), 1, indexFile);
    }
    
    // Escreve o último ponteiro P[m]
    fwrite(&node->P[M_ORDER - 1], sizeof(int), 1, indexFile);
}

/*
 * Função corrigida para ler um nó da Árvore-B do arquivo de índice,
 * interpretando a estrutura intercalada P, C, PR especificada no PDF do trabalho.
 */
BTreeNode* read_b_tree_node(FILE *indexFile, int rrn) {
    if (rrn == -1) return NULL;

    BTreeNode *node = create_b_tree_node();
    
    // Posiciona o ponteiro no local correto para leitura do nó
    fseek(indexFile, (rrn * B_TREE_NODE_SIZE) + B_TREE_HEADER_PAGE_SIZE, SEEK_SET);
    
    // Lê os campos fixos iniciais: tipoNo e nroChaves
    fread(&node->tipoNo, sizeof(int), 1, indexFile);
    fread(&node->nroChaves, sizeof(int), 1, indexFile);

    // Lê os campos de forma intercalada: (P, C, PR)
    // A ordem da árvore é 3, então temos no máximo 2 chaves e 3 ponteiros.
    for (int i = 0; i < M_ORDER - 1; i++) {
        fread(&node->P[i], sizeof(int), 1, indexFile);
        fread(&node->C[i], sizeof(int), 1, indexFile);
        fread(&node->PR[i], sizeof(long), 1, indexFile);
    }

    // Lê o último ponteiro P[m]
    fread(&node->P[M_ORDER - 1], sizeof(int), 1, indexFile);

    return node;
}

// Adicione estas duas funções em arvoreB.c

// Protótipo da função recursiva estática
static long b_tree_search_recursive(FILE *indexFile, int rrn_atual, int chave);

// Função "pública" de busca
long b_tree_search(FILE *indexFile, int chave) {
    BTreeHeader header;
    fseek(indexFile, 0, SEEK_SET);
    fread(&header.status, sizeof(char), 1, indexFile);
    fread(&header.noRaiz, sizeof(int), 1, indexFile);

    if (header.status == '0' || header.noRaiz == -1) {
        return -1; // Árvore inconsistente ou vazia
    }

    return b_tree_search_recursive(indexFile, header.noRaiz, chave);
}

// Função recursiva que efetivamente busca a chave
static long b_tree_search_recursive(FILE *indexFile, int rrn_atual, int chave) {
    if (rrn_atual == -1) {
        return -1; // Caso base: chegou a um ponteiro nulo, chave não encontrada
    }

    BTreeNode *no = read_b_tree_node(indexFile, rrn_atual); // Reutiliza sua função de leitura de nó
    if (no == NULL) return -1;

    // Procura a chave no nó atual
    for (int i = 0; i < no->nroChaves; i++) {
        if (chave == no->C[i]) {
            long ref = no->PR[i]; // Chave encontrada!
            free(no);
            return ref; // Retorna o byte offset do registro de dados
        }
        if (chave < no->C[i]) {
            // Desce para o filho à esquerda da chave
            long ref = b_tree_search_recursive(indexFile, no->P[i], chave);
            free(no);
            return ref;
        }
    }

    // Se a chave é maior que todas no nó, desce para o último filho (à direita)
    long ref = b_tree_search_recursive(indexFile, no->P[no->nroChaves], chave);
    free(no);
    return ref;
}

static void b_tree_update_pr_recursive(FILE *indexFile, int rrn_atual, int chave, long novo_ref_dados) {
    // Caso base: Se o RRN atual é -1 (ponteiro nulo), a chave não foi encontrada neste caminho.
    if (rrn_atual == -1) return;

    // Lê o nó da Árvore-B correspondente ao RRN atual.
    BTreeNode *no = read_b_tree_node(indexFile, rrn_atual);
    if (no == NULL) {
        // Se houver um erro na leitura do nó, interrompe a recursão.
        fprintf(stderr, "Erro ao ler no %d na atualizacao do PR da B-Tree.\n", rrn_atual);
        return;
    }

    // Procura a chave no nó atual.
    int i = 0;
    // Percorre as chaves no nó até encontrar uma chave maior ou igual à chave de busca,
    // ou até o final das chaves do nó.
    while (i < no->nroChaves && chave > no->C[i]) {
        i++;
    }

    // Verifica se a chave foi encontrada no nó atual.
    // 'i' é o índice onde a chave *deveria* estar, ou onde o próximo passo recursivo deve ir.
    if (i < no->nroChaves && chave == no->C[i]) {
        // Chave encontrada! Atualiza o Ponteiro de Referência (PR).
        no->PR[i] = novo_ref_dados;
        // Reescreve o nó atualizado de volta no disco.
        write_b_tree_node(indexFile, rrn_atual, no);
        // Libera a memória alocada para o nó.
        free(no);
        return; // A chave foi encontrada e atualizada, termina a recursão.
    }

    // Se a chave não foi encontrada no nó atual, decide para qual filho descer.
    // A busca continua no filho apontado por P[i].
    b_tree_update_pr_recursive(indexFile, no->P[i], chave, novo_ref_dados);

    // Libera a memória alocada para o nó após a chamada recursiva retornar.
    free(no);
}

// Função pública para atualizar o ponteiro de referência (PR) de uma chave na Árvore-B.
// Esta função é o ponto de entrada para a atualização do PR.
void b_tree_update_pr(FILE *indexFile, int chave, long novo_ref_dados) {
    BTreeHeader header; // Struct para armazenar o cabeçalho da Árvore-B.

    // Posiciona o ponteiro do arquivo de índice no início para ler o cabeçalho.
    fseek(indexFile, 0, SEEK_SET);
    // Lê o status do cabeçalho.
    fread(&header.status, sizeof(char), 1, indexFile);
    // Lê o RRN do nó raiz do cabeçalho.
    fread(&header.noRaiz, sizeof(int), 1, indexFile);

    // Verifica a consistência da Árvore-B e se ela não está vazia.
    if (header.status == '0' || header.noRaiz == -1) {
        // Árvore inconsistente ou vazia, não há chaves para atualizar.
        return;
    }

    // Inicia a busca recursiva a partir do nó raiz para encontrar e atualizar o PR.
    b_tree_update_pr_recursive(indexFile, header.noRaiz, chave, novo_ref_dados);
}