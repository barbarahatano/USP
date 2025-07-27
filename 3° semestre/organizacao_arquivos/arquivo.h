#ifndef ARQUIVO_H
#define ARQUIVO_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char status;
    long int topo;
    long int proxByteOffset;
    int nroRegArq;
    int nroRegRem;
    char descreveIdentificador[23];
    char descreveYear[27];
    char descreveFinancialLoss[28];
    char codDescreveContry[1];
    char descreveCountry[26];
    unsigned char codDescreveType[1];
    char descreveType[38];
    unsigned char codDescreveTargetIndustry[1];
    char descreveTargetIndustry[38];
    unsigned char codDescreveDefense[1];
    char descreveDefense[67];
} Cabecalho;

// Definição da estrutura do registro de dados (conforme especificado anteriormente)
typedef struct {
    int idAttack;
    int year;
    float financialLoss;
    char removido;
    int tamanhoRegistro;
    long int prox;
    char *country;
    char *attackType;
    char *targetIndustry;
    char *defenseMechanism;
} RegistroDados;

int escreverCabecalho(FILE *arquivo, Cabecalho *cabecalho);
int escreverRegistro(FILE *arquivo, RegistroDados *registro, Cabecalho *cabecalho);
RegistroDados* lerRegistroCSV(FILE *csvFile);
int inicializarCabecalhoStruct(Cabecalho *cabecalho, FILE *arquivo);
int escreverCabecalho(FILE *arquivo, Cabecalho *cabecalho);
char* lerCampoVariavel(FILE *arquivo);  
int lerCabecalho(FILE *arquivo, Cabecalho *cabecalho);

#endif // ARQUIVO_H

