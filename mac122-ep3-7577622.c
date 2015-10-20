/***************************************************************/
/**                                                           **/
/**   Marcos Kazuya Yamazaki                     7577622      **/
/**   Exercício-Programa 03                                   **/
/**                                                           **/
/***************************************************************/


/*
 *  1. Qual a estrutura de dados que você usa para representar um tabuleiro?
 *   No primeiro momento, uso uma matriz de inteiro, pois para cada posicao que um bispo
 *   eh colocado, todas as suas diagonais sao diminuidas de um valor (inicial 0, para
 *   casa vazia). Ja para guardar cada tabuleiro distinto, é usado uma matriz de char,
 *   pois o tabuleiro é representado apenas por valores 0 (casa vazia) e 1 (bispo).
 *
 *  2. Como são gerados os tabuleiros? 
 *   O tabuleiro é gerado por Backtracking ou Busca em profundidade, a cada chamada da
 *   recursao, enquanto o numero de bispos é menor que o desejado, coloca a posicao no
 *   lugar livre achado e subtrai (para marcar os lugares que nao se poderiam colocar 
 *   os proximos bispos) o valor em -1 para todas as suas diagonais.
 *
 *  3. Como você detecta se um tabuleiro gerado é equivalente a outro que já gerou?
 *   Quando é achado um novo tabuleiro, eh calculada seu valor para a tabela de hash,
 *   se nessa posicao houver um  outro tabuleiro, eh comparada um por um todas as posicoes.
 *   Se for equivalente, as proximas 7 simetricas a ela nem deverá ser calculada.
 */
 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int primos[] = {
    53, 97, 193, 389, 769, 1543, 3079, 6151, 12289,
    24593, 49157, 98317, 196613, 393241, 786433,
    1572869, 3145739, 6291469, 12582917, 25165843,
    50331653, 100663319, 201326611, 402653189,
    805306457, 1610612741
};

/*****************************************************************************/
/**                                                                         **/
/**   tabela - tamanho lateral da tabela de serao colocado os bispos        **/
/**   bispos - numero de bispos que deverá ter na tabela                    **/
/**	  imprime_tab - imprime os tabuleiros nao simetricos                    **/
/**	  imprime_inf - imprime as informacoes da tabela de Hashing utilizada	**/
/**	  **tabuleiro - tabuleiro que contem as informacoes dos bispos e de		**/
/**					onde pode ou nao ser colocados outros					**/
/**	  p - indice do vetor primos											**/
/**	  maxCol - aqui e guardado o valor da colisao onde foi maxima,			**/
/**			   caso ache algum outro maior, o valor é substituido			**/
/**	  hashOcupado - numero que a tabela hash foi ocupada					**/
/**	  DISTINTOS - numero de tabelas distintas encontradas					**/
/**	  NAOSIME - numeros de tabela que nao sao simetricas distintas			**/
/**	  **t - tabela auxiliar, que guarda apenas os valores de onde 			**/
/**			estao os bispos													**/
/**                                                           				**/
/*****************************************************************************/

int tabela = 0, bispos = 0, imprime_tab = 0, imprime_inf = 0, **tabuleiro,
	p = 0, maxCol = 0, hashOcupado = 0, DISTINTOS = 0, NAOSIME = 0;
char **t;

/*****************************************************************/
/** 															**/	
/**		tabNum - n-esimo tabuleiro nao simetrico encontrado		**/
/**		*m - tabueiro correspondente							**/
/**																**/
/*****************************************************************/ 

typedef struct infTab{
    int tabNum;
    char** m;
	struct infTab *prox;
} *strIT;
strIT *vetorHash;

/*********************************************************************/

void montaTabuleiro(int numBispo, int x, int y);
int guardaComparaTabuleiro();
void ajeitaTabuleiro(int cordX, int cordY, int valor);
int guardaTabuleiro(int simetria, strIT aux);
int arrumaTabelaHash();
int* hash(int mod);
int verificaIgualdade(char **m, char **n);

/*********************************************************************/

int main (int argc, char *argv[])
{
    int i, j;
    vetorHash = (strIT *) calloc(primos[p], sizeof(strIT));
    if(!vetorHash) {
        printf("Memoria insuficiente\n"); return EXIT_FAILURE;
    }
	for(i = 0; i < primos[p]; vetorHash[i++] = NULL);
	tabela = atoi(argv[1]);
	bispos = atoi(argv[2]);
    
	for(i = 3; i < argc; ++i){
        if(!strcmp(argv[i], "v")) imprime_inf = 1;
        else if(!strcmp(argv[i], "i")) imprime_tab = 1;
    }

	tabuleiro = (int **) calloc(tabela, sizeof(int*));
	if(!tabuleiro) {printf("Memoria insuficiente\n"); return EXIT_FAILURE;}
	for(i = 0; i < tabela; ++i) {
          tabuleiro[i] = (int *) calloc(tabela, sizeof(int));
          if(!tabuleiro[i]) {printf("Memoria insuficiente\n"); return EXIT_FAILURE;}
    }

	t = (char **) malloc(tabela*sizeof(char*));
	if(!t) {printf("Memoria insuficiente\n"); return EXIT_FAILURE;}
	for(i = 0; i < tabela; ++i){
          t[i] = (char *) malloc(tabela*sizeof(char));
	      if(!t[i]) {printf("Memoria insuficiente\n"); return EXIT_FAILURE;}
    }

	for(i = 0; i <= tabela/2; ++i){
        for(j = 0; j <= tabela/2; ++j){
            montaTabuleiro(1, i, j);
            tabuleiro[i][j] = 0;
            ajeitaTabuleiro(i, j, 1);
        }
	}

	free(vetorHash); free(tabuleiro); free(t);
	printf("%i Tabuleiros, %i nao equivalentes\n\n",DISTINTOS, NAOSIME);
	if(imprime_inf)
		printf("Tabela Hash: %i, Carga: %.6f\nTabela Ocupado: %i, Max Colisoes: %i \n", primos[p], (float)DISTINTOS/(float)primos[p],hashOcupado, maxCol);
	
    return EXIT_SUCCESS;
}

/******************************************************************************************/
/**                                                                                      **/
/**       Funcao recursiva que a partir de onde o ultimo bispo foi colocado percorre     **/
/**   o resto do tabuleiro procurando por espaco onde podem ser colocados mais           **/
/**   bispos.                                                                            **/
/**                                                                                      **/
/**   numBispo - numero de bispos que ja foi colocado no tabuleiro, caso ele seja        **/
/**	  		     igual ao numero de bispos que se deseja por, ela guardo o tabuleiro.    **/
/**   x, y - coordenadas de onde sera colocado o bispo                                   **/
/**                                                                                      **/
/******************************************************************************************/

void montaTabuleiro(int numBispo, int x, int y)
{
    int i, j;
	/* coloca o bispo na coordenada (x,y), marcando com o valor '1' */
    tabuleiro[x][y] = 1;

	/* marca todos os lugares onde nao se poderiam colocar os proximos bispos */
	ajeitaTabuleiro(x, y, -1);

    if(numBispo >= bispos){
        guardaComparaTabuleiro();
        return;
    }

    for(i = x; i < tabela; ++i){
        for(j = y; j < tabela; ++j){
            if(tabuleiro[i][j] == 0){
				montaTabuleiro(numBispo+1, i, j);
				tabuleiro[i][j] = 0;
                ajeitaTabuleiro(i, j, 1);
			}
		y = 0;
        }
    }
}

/************************************************************************************/
/**                                                                                **/
/**       Apos achar um tabuleiro com o numeros de bispos desejado, o tabuleiro    **/
/**   é mandado para essa funcao. Daqui eh mandado para se calcular o valor para   **/
/**	  a tabela de hashing e depois guarda seu tabuleiro no seu respectivo lugar.   **/
/**                                                                                **/
/************************************************************************************/

int guardaComparaTabuleiro()
{
    int i, j, *h, tabEqui = 0, pareado = 0, colisao = 0;
	strIT auxIT, aux;

	/*     A funcao hash(int p) devolve um vetor com 8 casas, cada um com   */
	/* o valor hash de cada simetria da tabela original                     */
    h = hash(primos[p]);

	/*     O valor -1 na primeira entrada da tabela h, significa que o tabuleiro     */
	/*  encontrado eh igual ao outro que ja foi encontrado anteriormente.            */
	if(h[0] == -1){
		free(h); return;
	}

	NAOSIME++;
	for(i = 0; i < 8; ++i){
        auxIT = (strIT) malloc(sizeof(struct infTab));
        if(!auxIT) {printf("Memoria insuficiente\n"); return EXIT_FAILURE;}
		auxIT -> tabNum = NAOSIME - 1;
		auxIT -> prox = NULL;
		/* Guarda o tabuleiro na celula, de acordo com a ordem dele */
		guardaTabuleiro(i , auxIT);
		
		/* Verifica se a casa do vetor de hashing ja esta ocupada */
		if(!vetorHash[h[i]]){
			vetorHash[h[i]] = auxIT;
			DISTINTOS++; hashOcupado++;
		}
		else{
			/*     No caso positivo, percorre todas as celulas, verificando    */
		    /*  se algum tabuleiro identico ja nao foi guardada.               */
			for(aux = vetorHash[h[i]]; aux; aux = aux->prox){
				if(aux -> tabNum == auxIT -> tabNum){
					if(pareado) tabEqui = 1;
					else if(verificaIgualdade(aux -> m, auxIT -> m)) tabEqui = (pareado = 1);
				}
				if(tabEqui) break;
			}
			if(!tabEqui){
				/* Se nao achou nenhuma equivalente, guarda a celula na tabela */
				for(colisao  = 1, aux = vetorHash[h[i]]; aux->prox; aux = aux->prox, colisao++);
				if(++colisao > maxCol) maxCol = colisao;
				aux -> prox = auxIT;
				DISTINTOS++;
			}
		}
		tabEqui = 0;
	}
	free(h);

	if(imprime_tab){
		for(i = 0; i < tabela; ++i){ for(j = 0; j < tabela; ++j) printf("%i ", t[i][j]); printf("\n");} printf("\n");
	}

	/*  Se a carga for maior que 2/3 da tabela, aumenta o numero de pposicoes da tabela */
	if((float)DISTINTOS/(float)primos[p] >= 0.66667) arrumaTabelaHash();
	return EXIT_SUCCESS;
}

/*************************************************************************************/
/**                                                                                 **/
/**     Ajeita a tabela a partir de onde foi colocado ou tirado o bispo,            **/
/**	 percorrendo todas as diagonais.                                                **/
/**  Valores negativos na tabela, significa que nao se pode colocar um bispo        **/
/**  cordX, cordY - coordenadas de onde foi tirado ou colocado o bispo              **/
/**  valor - +1, foi tirado um bispo												**/
/**		   - -1, foi colocado um bispo												**/
/**                                                                                 **/
/*************************************************************************************/

void ajeitaTabuleiro(int cordX, int cordY, int valor)
{
    int i = cordX, j = cordY;

    while(cordX < tabela-1 && cordY < tabela-1) tabuleiro[++cordX][++cordY] += valor;
    cordX = i; cordY = j;
    while(cordX < tabela-1 && cordY > 0)        tabuleiro[++cordX][--cordY] += valor;
    cordX = i; cordY = j;
    while(cordX > 0 && cordY < tabela)          tabuleiro[--cordX][++cordY] += valor;
    cordX = i; cordY = j;
    while(cordX > 0 && cordY > 0)               tabuleiro[--cordX][--cordY] += valor;
}

/************************************************************************************/
/**                                                                                **/
/**		Guarda o tabuleiro na celula, porem há 8 casos de simetrias diferente,     **/
/**  dependendo que cada cada valor da simetria, a tabela é percorrida de formas   **/
/**  diferente                                                                     **/
/**  simetria - valores de 0 a 7                                                   **/
/**  aux - apontador da celula onde será guardada o tabuleiro                      **/
/**                                                                                **/
/************************************************************************************/

int guardaTabuleiro(int simetria, strIT aux)
{
	int i, j, x = 0, y = 0;

	aux -> m = (char **) calloc(tabela, sizeof(char*));
	if(!aux->m) {printf("Memoria insuficiente\n"); return EXIT_FAILURE;}
	for(j = 0; j < tabela; j++){
           aux -> m[j] = (char *) calloc(tabela, sizeof(char));
           if(!aux -> m[j]) {printf("Memoria insuficiente\n"); return EXIT_FAILURE;}
    }
    
	switch(simetria){
	case 0: for(i = 0; i < tabela ; ++i, ++x, y = 0)   for(j = 0; j < tabela; ++j)    aux -> m[x][y++] = t[i][j]; break;
	case 1: for(i = tabela-1; i >= 0; --i, ++x, y = 0) for(j = 0; j < tabela; ++j)    aux -> m[x][y++] = t[i][j]; break;
    case 2: for(i = tabela-1; i >= 0; --i, ++x, y = 0) for(j = tabela-1; j >= 0; --j) aux -> m[x][y++] = t[i][j]; break;
    case 3: for(i = 0; i < tabela; ++i, ++x, y = 0)    for(j = tabela-1; j >= 0; --j) aux -> m[x][y++] = t[i][j]; break;
    case 4: for(i = 0; i < tabela; ++i, ++x, y = 0)    for(j = 0; j < tabela; ++j)    aux -> m[x][y++] = t[j][i]; break;
    case 5: for(i = tabela-1; i >= 0; --i, ++x, y = 0) for(j = 0; j < tabela; ++j)    aux -> m[x][y++] = t[j][i]; break;
    case 6: for(i = tabela-1; i >= 0; --i, ++x, y = 0) for(j = tabela-1; j >= 0; --j) aux -> m[x][y++] = t[j][i]; break;
    case 7: for(i = 0; i < tabela; ++i, ++x, y = 0)    for(j = tabela-1; j >= 0; --j) aux -> m[x][y++] = t[j][i]; break;
	}
	return EXIT_SUCCESS;
}

/************************************************************************************/
/**                                                                                **/
/**      Quando eh chamada essa funcao, eh porque mais de 2/3 da tabela, se nao    **/
/**  houvesse colisoes, foram ocupadas, assim cria-se outra tabela com o proximo   **/
/**  primo mais proximo do dobro do atual.                                         **/
/**   	 Percorre a tabela anterior e calcula-se o novo valor da tabela Hash       **/ 
/**                                                                                **/
/************************************************************************************/

int arrumaTabelaHash()
{
	strIT *vetorAux, auxStr, auxFor, auxNull = NULL;
	int i, j, k, hash = 0, colisao = 0;
	hashOcupado = 0;

	vetorAux = (strIT *) calloc(primos[++p], sizeof(strIT));
	if(!vetorAux) {printf("Memoria insuficiente\n"); return EXIT_FAILURE;}
	for(i = 0; i < primos[p]; vetorAux[i++] = NULL);

	for(k = 0; k < primos[p-1]; k++){
		for(auxStr = vetorHash[k]; auxStr; auxStr = auxStr -> prox){
			hash = 0;
			if(auxNull) auxNull -> prox = NULL;
			
			for(i = 0; i < tabela; ++i)	for(j = 0; j < tabela; ++j) hash = (2*hash + auxStr->m[i][j])%primos[p];

			if(!vetorAux[hash]){
				hashOcupado++;
				auxNull = (vetorAux[hash] = auxStr);
			}
			else{
				for(auxFor = vetorAux[hash], colisao = 1; auxFor->prox; auxFor = auxFor->prox, colisao++);
				if(++colisao > maxCol) maxCol = colisao;
				auxNull = (auxFor -> prox = auxStr);
			}
		}
		if(auxNull) auxNull -> prox = NULL;
	}
	free(vetorHash);
	vetorHash = vetorAux;
	return EXIT_SUCCESS;
}

/**************************************************************************************/
/**                                                                                  **/
/**      Pega o tabuleiro, como ele esta em notacao binaria, transformaremos em      **/
/**  decimal, mas a cada passo tiraremos o modulo dele com o primo da tabela Hash.   **/
/**		 Primeiro ele calcula o valor da primeira tabela, depois vefirica se ela     **/
/**  ja foi guardada antes, caso positivo a funcao nem faz os calculos para as       **/
/**  as tabelas simetricas a ela.                                                    **/
/**                                                                                  **/  
/**************************************************************************************/

int* hash(int mod)
{
    int *h, i, j, base = 2;
	strIT auxH;

    h = (int *) calloc(8, sizeof(int));

	for(i = 0; i < tabela; ++i) for(j = 0; j < tabela; ++j) {
		if(tabuleiro[i][j] == 1) t[i][j] = 1;
		else t[i][j] = 0;
	}

    for(i = 0; i < tabela; ++i)    for(j = 0; j < tabela; ++j)    h[0] = (base*h[0] + t[i][j])%mod;
    for(auxH = vetorHash[h[0]]; auxH; auxH = auxH->prox){
		if(verificaIgualdade(auxH -> m, t)){
			h[0] = -1; return h;
		}
    }
	for(i = tabela-1; i >= 0; --i) for(j = 0; j < tabela; ++j)    h[1] = (base*h[1] + t[i][j])%mod;
    for(i = tabela-1; i >= 0; --i) for(j = tabela-1; j >= 0; --j) h[2] = (base*h[2] + t[i][j])%mod;
    for(i = 0; i < tabela; ++i)    for(j = tabela-1; j >= 0; --j) h[3] = (base*h[3] + t[i][j])%mod;
    for(i = 0; i < tabela; ++i)    for(j = 0; j < tabela; ++j)    h[4] = (base*h[4] + t[j][i])%mod;
    for(i = tabela-1; i >= 0; --i) for(j = 0; j < tabela; ++j)    h[5] = (base*h[5] + t[j][i])%mod;
    for(i = tabela-1; i >= 0; --i) for(j = tabela-1; j >= 0; --j) h[6] = (base*h[6] + t[j][i])%mod;
    for(i = 0; i < tabela; ++i)    for(j = tabela-1; j >= 0; --j) h[7] = (base*h[7] + t[j][i])%mod;

	/*for(i = 0; i < 8; i++) printf("%i ", h[i]); printf("\n");*/
	return h;
}

/****************************************************************/
/**                                                            **/
/**      Dada duas tabelas, verifica se sao identicas          **/
/**                                                            **/
/****************************************************************/

int verificaIgualdade(char **m, char **n)
{
	int i, j, igual = 1;
	for(i = 0; i < tabela; i++){
		for(j = 0; j < tabela; j++){
			if(n[i][j] != m[i][j]) igual = 0;
			if(!igual) break;
		}
		if(!igual) break;
	}
	return igual;
}


/******************************************************************************

FeedBack

ASPECTOS PRINCIPAIS
[+++] implementou todas as funcionalidades essenciais.
[+++] resultados corretos.
[ -2] infeciciência dos algoritmos e estruturas de dados:
    Você acaba calculando duas vezes as rotações e reflexões.
Total -2

ENTRADA E SAÍDA
[+++] OK.
Total -0

ERROS GERAIS
[ -1] vazamento de memória:
    Você libera o vetor de linhas das matrizes e o vetor de listas (a
    tabela de hash) mas não libera as linhas em si das matrizes nem as
    listas da taebla de hash.
Total -1

CLAREZA
[+++] relatório OK.
[+++] código OK.
Total -0

OBSERVAÇÕES
[!!!] excelente código.

DESCONTOS TOTAIS: -3

NOTA FINAL: 97/100

******************************************************************************/