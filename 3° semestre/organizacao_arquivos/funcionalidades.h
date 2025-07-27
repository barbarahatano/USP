#ifndef FUNCIONALIDADES_H
#define FUNCIONALIDADES_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "arquivo.h"
#include "busca.h"
#include "exibicao.h"
#include "arvoreB.h"

void removerRegistro(FILE *entrada, RegistroDados *registro, Cabecalho *cabecalho, long offset);

// Funções auxiliares para atualização
int calcular_tamanho_novo_registro(RegistroDados *registro);
void atualizar_campos_do_registro(RegistroDados *registro, Filtro *novos_dados);
long inserir_registro_first_fit(FILE *binFile, RegistroDados *registro, Cabecalho *cabecalho);
void funcionalidade3(FILE *binFile, Filtro *filtro, Cabecalho cabecalho, int funcionalidade);
void funcionalidade5(char *nomeArquivoEntrada, int n);
void funcionalidade6(char *nomeArquivoEntrada, int n);
void funcionalidade4(char *nomeArquivoEntrada, int n); 
void funcionalidade7(char *nomeArquivoDados, char *nomeArquivoIndice);
void funcionalidade8(char *nomeArquivoDados, char *nomeArquivoIndice, int n, int funcionalidade);
void funcionalidade10(char *nomeArquivoDados, char *nomeArquivoIndice, int n);
void funcionalidade11(char *nomeArquivoDados, char *nomeArquivoIndice, int n_operacoes);
#endif // FUNCIONALIDADES_H

