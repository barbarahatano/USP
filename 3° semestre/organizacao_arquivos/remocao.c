#include <stdio.h>
#include <stdlib.h>
#include "arquivo.h"
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "exibicao.h"
#include "busca.h"
#include <sys/types.h>
#include "funcionalidades.h"

void removerRegistro(FILE *entrada, RegistroDados *registro, Cabecalho *cabecalho, long offset) {
    // Validação básica para evitar erros com ponteiros nulos.
    if (registro == NULL || cabecalho == NULL) {
        fprintf(stderr, "Erro: registro ou cabecalho nulo em removerRegistro.\n");
        return;
    }

    // 1. Marcar o registro como logicamente removido ('1') no arquivo.
    // Posiciona o ponteiro no início do registro a ser removido.
    fseek(entrada, offset, SEEK_SET);
    char removido_flag = '1'; // Define a flag de removido.
    // Escreve a flag de removido no arquivo.
    if (fwrite(&removido_flag, sizeof(char), 1, entrada) != 1) {
        printf("Erro ao escrever o campo 'removido' no arquivo.\n");
        return;
    }

    // 2. Atualizar o ponteiro 'prox' do registro removido.
    // O ponteiro 'prox' está localizado após o 'removido' (1 char) e 'tamanhoRegistro' (1 int).
    // O valor desse 'prox' deve ser o RRN do antigo 'topo' da lista de livres.
    fseek(entrada, offset + sizeof(char) + sizeof(int), SEEK_SET);
    // Escreve o valor do 'topo' atual do cabeçalho como o próximo da lista.
    if (fwrite(&cabecalho->topo, sizeof(long int), 1, entrada) != 1) {
        printf("Erro ao escrever o campo 'prox' no arquivo.\n");
        return;
    }

    // 3. Atualizar as informações no cabeçalho em memória (Cabecalho *cabecalho).
    cabecalho->nroRegArq--;  // Um registro ativo a menos.
    cabecalho->nroRegRem++;  // Um registro removido a mais.
    // O registro recém-removido (cujo offset é 'offset') se torna o novo 'topo' da lista de livres.
    cabecalho->topo = offset;

    // 4. Reescrever o cabeçalho completo no arquivo para persistir as mudanças.
    // Volta para o início do arquivo para reescrever o cabeçalho.
    fseek(entrada, 0, SEEK_SET);
    // Chama sua função existente para escrever o cabeçalho atualizado.
    // Esta função deve reescrever TODOS os campos do cabeçalho.
    escreverCabecalho(entrada, cabecalho);

    // Garante que todas as operações de escrita são descarregadas para o disco.
    fflush(entrada);
}