#ifndef EXIBICAO_H
#define EXIBICAO_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "arquivo.h"

int exibirRegistro(RegistroDados registro, Cabecalho cabecalho);
void scan_quote_string(char *str);
void binarioNaTela(char *nomeArquivoBinario);

#endif // EXIBICAO_H

