/***************************************************************/
/**                                                           **/
/**   Marcos Kazuya Yamazaki                     7577622      **/
/**   Exerc�cio-Programa 02                                   **/
/**                                                           **/
/***************************************************************/

/*
 * Este EP foi compilado e executado no windows, com as mesmas op��es marcadas
 * na op��o Compiler and Debugger, [-ansi] e outros, mas nao sei pelo qual motivo no linux a
 * execucao nao � estav�l.
 *
 * Tamb�m tem este caso que DE VEZ EM QUANDO nao pega:
 * 1 10
 * 0 0 1 1 0 0 0 1 0 1
 * na maioria das vezes o programa executa esse exemplo normalmente
 * mas alguns casos o programa para de funcionar ><
 */

#include <stdio.h>
#include <stdlib.h>

int **vetor; /* pixel da imagem onde os valores serao o ou 1 */
int instancia = 1, linha, coluna,  numReg = 0, numerosDaRegiao = 0;
/* 'linha' e 'coluna' � o tamanho da matriz
 * numReg, quantidade de regioes que a imagem tem
 * numerosDaRegiao, quantidade de pixel de cada regiao conexa.
 */

typedef struct STcord *STClink;
typedef struct STreg *STRlink;

struct STcord{
	int cordLin, cordCol;
	STClink proximoSTC;
}; /* celula das coordenadas de cada pixel */

struct STreg{
	int regiao, quantidade;
	STClink primeiraSTC;
	STRlink proximoSTR;
};
/*
 * Celula de cada regiao, possui um ponteiro
 * que aponta para a lista que contem as
 * coordenadas de cada pixel dela.
 * (inicio da lista das coordenadas)
 */

STClink finalSTC = NULL, auxSTC = NULL, deletaSTC = NULL;
STRlink inicioSTR = NULL, finalSTR = NULL, auxSTR = NULL, deletaSTR = NULL;
/* Ponteiros auxiliares das listas */

int criaMatriz();
int contaRegioes ();
int detectaRegiaoConexa(int i,int j);
void imprime();
void liberaEspacoMemoria();

/************************************************************************************/

int main (int argc, char *argv[])
{
	while ((scanf("%i %i", &linha, &coluna) != EOF)
            && (linha > 0 && coluna > 0))
	{
		int i, j;

		/* Criacao da matriz */
		criaMatriz();

		/* Le os valores de cada pixel */
		for(i = 0; i < linha; i++)
            for(j = 0; j < coluna; j++) scanf("%i", &vetor[i][j]);

		contaRegioes(); /* coracao do programa */
		imprime(); /* imprime os pixel e a quantidade de regioes */
		liberaEspacoMemoria(); /* libera o espaco alocado pelas listas/vetor */

		numReg = 0; instancia++; /* Atualiza valores para a proxima instancia, se tiver */
	}
	return 0;
}

/************************************************************************************/

int criaMatriz()
{
    int i;
    /* Cria a primeira coluna de n linhas */
    vetor = (int**) malloc(linha*sizeof(int*));
    if(vetor == NULL){ /* Se o malloc retornar NULL ao inves de um
                 ponteiro, nao ha memoria suficiente */
        printf("Memoria esgotada!\n");
        return EXIT_FAILURE;
    }
    for(i = 0; i < linha; i++)
    { /* Para cada linha da coluna criada
        cria as outras colunas */
        vetor[i] = (int*) malloc(coluna*sizeof(int));

        if(!vetor[i]){
            printf("Memoria esgotada!\n");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

/************************************************************************************/

/*
 * Procura por regi�es, e quando acha uma, procura
 * todas as vizinhas numa outra funcao
 */

int contaRegioes ()
{
	int i, j;
	for(i = 0; i < linha; i++){
		for(j = 0; j < coluna; j++){
			if(vetor[i][j] == 1){
				numReg++;

				/* cria lista das regioes */
				auxSTR = (STRlink) malloc(sizeof *auxSTR);
				if(auxSTR == NULL) {
				    printf("Memoria esgotada!\n"); return EXIT_FAILURE;
                }
				auxSTR -> regiao = numReg;

				/* inicio da lista */
				if(inicioSTR == NULL) inicioSTR = auxSTR;
				/* caso a lista ja tenha sido inicializada */
				else finalSTR -> proximoSTR = auxSTR;

				finalSTR = auxSTR;
				finalSTR -> primeiraSTC = NULL;
				finalSTR -> proximoSTR = NULL;

				/* verifica todos os vizinhos enquanto a ocorrencia do pixel
				 i = linha da matriz / j = coluna da matriz */
				detectaRegiaoConexa(i,j);

				/* guarda a quantidade de ocorrencia do pixel na regiao
				 * e depois zera esse valor para a proxima regiao.
				 */
				finalSTR -> quantidade = numerosDaRegiao;
				numerosDaRegiao = 0;
			}
		}
	}
	return 0;
}

/************************************************************************************/

/*
 * Procura usando recursao as regioes conexas. Parametros:
 * i -> linha do vetor
 * j -> coluna do vetor
 */

int detectaRegiaoConexa(int i,int j)
{
	/* Faz a celula das cordenadas do pixel */
	auxSTC = (STClink) malloc(sizeof *auxSTC);
    if(auxSTC == NULL) {
        printf("Memoria esgotada!\n"); return EXIT_FAILURE;
    }
	/* Guarda as coordenadas na celula da lista */
	auxSTC -> cordLin = i; auxSTC -> cordCol = j;

	/* inicio da lista */
	if((finalSTR -> primeiraSTC) == NULL) finalSTC = (finalSTR -> primeiraSTC = auxSTC);
	/* caso a lista ja tenha sido inicializada */
	else finalSTC = (finalSTC -> proximoSTC = auxSTC);

	finalSTC -> proximoSTC = NULL;

	/* diferencia os pixel que ja foram verificados */
	vetor[i][j] = numReg*(-1);

	/* verifica se esta na borda, se nao esta, verifica os 4 vizinhos */
	if(j < coluna-1 && vetor[i][j+1] == 1) detectaRegiaoConexa(i,j+1);
	if(i < linha-1 && vetor[i+1][j] == 1) detectaRegiaoConexa(i+1,j);
	if(j > 0 && vetor[i][j-1] == 1) detectaRegiaoConexa(i,j-1);
	if(i > 0 && vetor[i-1][j] == 1) detectaRegiaoConexa(i-1,j);

	/* quantidade de pixel da regiao conexa */
	numerosDaRegiao++;
	return 0;
}

/************************************************************************************/

void imprime()
{
    int k,l; /* Variaveis para auxiliar a percurs�o da matriz */
	printf("\nInstancia %i\n", instancia);

	/* Caso o numero de regiao � menor que o numero de letras do alfabeto */
	if(numReg <= 'z'-'a'+1){
		/* imprime o desenho */
		for(k = 0; k < linha; k++){
			for(l = 0; l < coluna; l++){
				if(vetor[k][l] == 0) printf("  ");
				else{
					/* Como para identificar, multiplicamos o numero de regioes
					 * por (-1), aqui tranformaremos esse numero no caracter
					 * de 'a' at� 'z'.
					 */
					printf(" %c", (-1)*vetor[k][l]+('a'-1));
				}
			}
			printf("\n");
		}
		printf("\nNumero de regioes: %i\n", numReg);
		/* Percorre a lista das regioes, pois la
		 * estao guardadas as quantidade de cada uma
		 */
		for(auxSTR = inicioSTR; auxSTR; auxSTR = auxSTR->proximoSTR){
			printf("Regiao %c: %i\n", auxSTR->regiao+('a'-1), auxSTR->quantidade);
		}
	}
	/* Caso contrario s� imprime a quantidade de pixel de cada regiao */
	else{
        printf("\nNumero de regioes: %i\n", numReg);
		for(auxSTR = inicioSTR; auxSTR; auxSTR = auxSTR->proximoSTR){
			printf("Regiao %i: %i\n", auxSTR->regiao, auxSTR->quantidade);
		}
	}
    printf("\n");
}

/************************************************************************************/

void liberaEspacoMemoria()
{    /*
     * Para nao dar erro, essa fun��o libera com o free a celula anterior
     * da qual o ponteiro auxiliar aponta no momento, pois ela usa o valor do
     * ponteiro que tem dentro da celula para ir para a proxima
     */
    for(auxSTR = inicioSTR; auxSTR != NULL; auxSTR = auxSTR->proximoSTR){
        if(deletaSTR != NULL) free(deletaSTR);
        for(auxSTC = auxSTR -> primeiraSTC; auxSTC != NULL; auxSTC = auxSTC->proximoSTC){
            if(deletaSTC != NULL) free(deletaSTC);
            deletaSTC = auxSTC;
        }
        free(deletaSTC);
        deletaSTR = auxSTR;
    }
    free(deletaSTC);
    free(deletaSTR);
    free(vetor);

    finalSTC = NULL; deletaSTC = NULL;
    inicioSTR = NULL; finalSTR = NULL; deletaSTR = NULL;
}

/********************************************************************************
FeedBack

FUNCIONALIDADES PRINCIPAIS
[+++] detec��o de regi�es conexas OK.
[+++] constru��o das listas de coordenadas OK.
Total -0

ENTRADA
[+++] entrada OK.
Total -0

SA�DA
[+++] sa�da OK.
Total -0

ERROS GERAIS
[ -4] segfault e double free:
    No ponteiro 'deletaSTC' na fun��o 'liberaEspacoMemoria'. O que
    acontece � que n�o havia necessidade dos free da linha 253 e que
    ficou faltando fazer o 'deletaSTC' ficar NULL antes da pr�xima
    itera��o do for das linhas 244-253, para que ele n�o fosse
    liberado de novo na linha 247.
    No Windows, o programa provavelmente teve sua mem�ria protegida, e
    isso evitava os crashs. No linux, isso n�o acontecia e o problema
    emergia.
[ -1] vazamento de mem�ria:
    Tirando os problemas com o liberamento das listas ligadas, ainda
    assim ficou faltando liberar as linhas da matriz 'vector'. Note
    que a linha 256 s� libera o vetor de colunas dessa matriz, mas n�o
    as linhas em si.
Total -5

C�DIGO
[+++] c�digo OK.
Total -0

OBSERVA��ES
[!!!] vari�veis auxiliares globais:
    Evite sempre fazer vari�veis AUXILIARES globais, como a 'auxSTC' e
    a 'auxSTR'. Desse jeito, com uma pequena distra��o, seu programa
    j� fica todo inconsistente, pois cada fun��o precisa terminar
    deixando elas de um jeito que n�o atrapalhe as outras fun��es, ou
    sen�o toda fun��o precisa lembrar quando chamada que a vari�vel
    pode estar com algum valor misterioso.
    Usar vari�veis globais para elementos globais faz sentido, como a
    'inicioSTR'. Mas vari�veis auxiliares n�o, de verdade.

DESCONTOS TOTAIS: -5

NOTA FINAL: 95/100

********************************************************************************/
