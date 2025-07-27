#ifndef BUSCA_H
#define BUSCA_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "arquivo.h"


typedef struct {
    char **campos;
    char **valores;
    int qtdPares;
} Filtro; 


int registroCorresponde(RegistroDados *registro, Cabecalho *cabecalho, Filtro *filtro);
Filtro *lerFiltroEntrada();
void liberarFiltro(Filtro *filtro);

#endif // BUSCA_H

