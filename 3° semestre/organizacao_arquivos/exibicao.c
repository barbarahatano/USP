#include <stdio.h>
#include <stdlib.h>
#include "arquivo.h"
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


void scan_quote_string(char *str) {

	/*
	*	Use essa função para ler um campo string delimitado entre aspas (").
	*	Chame ela na hora que for ler tal campo. Por exemplo:
	*
	*	A entrada está da seguinte forma:
	*		nomeDoCampo "MARIA DA SILVA"
	*
	*	Para ler isso para as strings já alocadas str1 e str2 do seu programa, você faz:
	*		scanf("%s", str1); // Vai salvar nomeDoCampo em str1
	*		scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2 (sem as aspas)
	*
	*/

	char R;

	while((R = getchar()) != EOF && isspace(R)); // ignorar espaços, \r, \n...

	if(R == 'N' || R == 'n') { // campo NULO
		getchar(); getchar(); getchar(); // ignorar o "ULO" de NULO.
		strcpy(str, ""); // copia string vazia
	} else if(R == '\"') {
		if(scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
			strcpy(str, "");
		}
		getchar(); // ignorar aspas fechando
	} else if(R != EOF){ // vc tá tentando ler uma string que não tá entre aspas! Fazer leitura normal %s então, pois deve ser algum inteiro ou algo assim...
		str[0] = R;
		scanf("%s", &str[1]);
	} else { // EOF
		strcpy(str, "");
	}
}

void binarioNaTela(char *nomeArquivoBinario) { 
    // Função para imprimir o arquivo binário na tela
	unsigned long i, cs;
	unsigned char *mb;
	size_t fl;
	FILE *fs;
	if(nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
		fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
		return;
	}
	fseek(fs, 0, SEEK_END);
	fl = ftell(fs);
	fseek(fs, 0, SEEK_SET);
	mb = (unsigned char *) malloc(fl);
	fread(mb, 1, fl, fs);

	cs = 0;
	for(i = 0; i < fl; i++) {
		cs += (unsigned long) mb[i];
	}
	printf("%lf\n", (cs / (double) 100));
	free(mb);
	fclose(fs);
}

int exibirRegistro(RegistroDados registro, Cabecalho cabecalho) {
    
    if (strcmp(&registro.removido, "1") == 0) {
        printf("Registro inexistente\n");
        return 1;
    }
    
    else {
        //Printa o identificador
        printf("%.*s", 23, cabecalho.descreveIdentificador);
        printf(": %d\n", registro.idAttack);
        
        
    //Printa Ano
        printf("%.*s", 27, cabecalho.descreveYear);
        if (registro.year == -1) {
            printf(": NADA CONSTA\n");
        } else {
            printf(": %d\n", registro.year);
        }
        //Printa país
        printf("%.*s", 26, cabecalho.descreveCountry);
        if (registro.country != NULL) {
            printf(": %s\n", registro.country);
        } else {
            printf(": NADA CONSTA\n");
        }
        //Printa setor da indústria
        printf("%.*s", 38, cabecalho.descreveTargetIndustry);
        if (registro.targetIndustry != NULL) {
            printf(": %s\n", registro.targetIndustry);
        } else {
            printf(": NADA CONSTA\n");
        }
        //Printa tipo de ameaça
        printf("%.*s", 38, cabecalho.descreveType);
        if (registro.attackType != NULL) {
            printf(": %s\n", registro.attackType);
        } else {
            printf(": NADA CONSTA\n");
        }
    //Printa prejuízo financeiro
        printf("%.*s", 28, cabecalho.descreveFinancialLoss);
        if (registro.financialLoss == -1.0) {
            printf(": NADA CONSTA\n");
        } else {
            printf(": %.2lf\n", registro.financialLoss);
        }
    //Printa estratégia de defesa
        printf("%.*s", 67, cabecalho.descreveDefense);
        if (registro.defenseMechanism != NULL) {
            printf(": %s\n", registro.defenseMechanism);
        } else {
            printf(": NADA CONSTA\n");
        }
    // Imprime uma linha em branco para separar os registros
        printf("\n");
        return 1;
        }
    }