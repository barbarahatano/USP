#ifndef ARVOREB_H
#define ARVOREB_H

#include <stdio.h>

// A ordem da árvore, conforme especificado no PDF 
#define M_ORDER 3

// Tamanho em bytes de uma página/nó da árvore-B no disco 
#define B_TREE_NODE_SIZE 44

// Estrutura do Cabeçalho do arquivo de índice da Árvore-B 
typedef struct {
    char status;    // 1 byte
    int noRaiz;     // 4 bytes, RRN do nó raiz
    int proxRRN;    // 4 bytes, RRN do próximo nó a ser alocado
    int nroNos;     // 4 bytes, Número total de nós na árvore
} BTreeHeader;

// Estrutura do Nó (página) da Árvore-B 
typedef struct {
    int tipoNo;     // 4 bytes, -1: Folha, 0: Raiz, 1: Intermediário
    int nroChaves;  // 4 bytes, Número de chaves presentes no nó
    int C[M_ORDER - 1]; // 2 chaves (idAttack) de 4 bytes cada
    long PR[M_ORDER - 1]; // 2 ponteiros de registro (byte offset) de 8 bytes cada
    int P[M_ORDER]; // 3 ponteiros para os filhos (RRNs) de 4 bytes cada
} BTreeNode;

BTreeNode* create_b_tree_node();
BTreeNode* read_b_tree_node(FILE *indexFile, int rrn);
int ler_b_tree_header(FILE *f_indice, BTreeHeader *header);

void b_tree_insert(FILE *f_indice, BTreeHeader *header, int chave, long ref_dados);
long b_tree_search(FILE *indexFile, int chave);
void b_tree_update_pr(FILE *indexFile, int chave, long novo_ref_dados);
void atualizarNoRaiz(FILE *indexFile, BTreeHeader *header);


#endif // ARVOREB_H