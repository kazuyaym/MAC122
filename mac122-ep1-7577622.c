/***************************************************************/
/**                                                           **/
/**   Marcos Kazuya Yamazaki                     7577622      **/
/**   Exercício-Programa 01                                   **/
/**                                                           **/
/***************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <wctype.h>
#include <ctype.h>

#define TRUE 1
#define FALSE 0

char palavra[2][2048];
int digitos[2][128];

int command_delete();
int command_squeeze(int numArg, char *argv2, char *argv3);
int command_complement(char *caracter);
int trocaCaracteres();
int arrumaTabela(int tab, int trunca, char *car_, char *comandoChar_); /* mexe com a string1 */
int proximoCaracter(int trunca, char *comandoChar2_, int *comando2_, int *cont_);
/*mexe com a strin2*/
int executa_help();

int erro = FALSE;

/************************************************************************************/

int main (int argc, char *argv[])
{
    int i,j;
    /* executa --help */
    if(!strcmp(argv[1], "--help")){
        executa_help();
        return EXIT_SUCCESS;
    }

    for(j = 0; j < 2; j++) for(i = 0; i < 128; i++) digitos[j][i] = -1;

    /*
    Tabela de caracteres ISO 8859-1
    Seu endereco significa o caracter e o valor que esta armazenado nele é sao os comandos abaixo:
    -1 => nao deve ser fazer nada com o caracter, exceto no -c -C --complement
    -2 => deve ser deletado
    -3 => deve tirar os repetidos
    -4 => no caso -c -C --complement, os caracters que estiver com esse comando nao deve ser trocado
          caso contrario sim.
    numeros (>= 0) => deverá ser substituido pela sua respectiva notacao decimal
    */

    /* pega a funcao de entrada -c, -C, -d, --delete, etc */
	/* no caso desse programa, ele nao consegue pegar dois comando numa execucao */
    char c = getopt(argc, argv, ":c:C:d:s:t:-:");
    if(c == '-'){
        if(!strcmp(argv[1], "--delete")) c = 'd';
        else if(!strcmp(argv[1], "--complement")) c = 'c';
        else if(!strcmp(argv[1], "--squeeze-repeats")) c = 's';
        else if(!strcmp(argv[1], "--truncate-set1")) c = 't';
        else {
            printf("tr: unrecognized option '%s'\n", argv[1]);
            printf("Try 'tr --help' for more information.\n");
            return EXIT_FAILURE;
        }
    }
    switch(c){
        case 'd':
            arrumaTabela(0, 0, argv[2], "~comando\n--delete\\0");
            if(erro) return 1;
            command_delete();
            return EXIT_SUCCESS;
        break;
        case 's':
            arrumaTabela(0, 0, argv[2], "~comando\n--squeeze-repeats\\0");
            if(erro) return 1;
            command_squeeze(argc, argv[2], argv[3]);
            return EXIT_SUCCESS;
        break;
        case 'c': case 'C': /* dois parametros x1 e x2 , na palavra tudo que nao é x1 vira x2 */
            arrumaTabela(0, 0, argv[2], "~comando\n--complement\\0");
            if(erro) return 1;
            command_complement(argv[3]);
			return EXIT_SUCCESS;
        break;
        case 't':
            arrumaTabela(1, 1, argv[2], argv[3]);
            if(erro) return 1;
        break;
        default:
            arrumaTabela(1, 0, argv[1], argv[2]);
            if(erro) return 1;
        break;
    }

    while(!0){ /* caso nao tenha comando ou -t */
        fflush(stdin); /* limpa buffer do teclado */
        fgets(palavra[1], 1024, stdin);
		/*scanf("%[!-~ ]", palavra[1]);*/
		trocaCaracteres();
        printf("%s", palavra[1]);
        for(i = 0; palavra[1][i] != '\0'; i++) palavra[1][i] = '\0';
    }
    return EXIT_SUCCESS;
	
	/* Este programa nao executa quando passa parametros com duas funcoes -cs, entre outras.
	 *
	 *  -------------------- Alguns do que esse programa faz ---------------------
	 *   
	 *   tr x[:xdigit:]2 [a*2][b*3][c*4][D*]
	 *   Entrada: x0123456789ABCEDFGabcdefg 
	 *   Saida:   aabDbccccDDDDDDDDGDDDDDDg
	 *
	 *   tr [:lower:]x0123 9[:upper:]z
	 *   Entrada: abcdxefgx0123		
	 *   Saida:   ABCDzEFGzzzzz
	 *
	 *   tr ant4-9z '\a''\n''\t'D-I
	 *	 Entrada: 04at5n6at7n8a9nz
	 *	 Saida:   0D*bip*	E
	 *			  F*bip*	G
	 *			  H*bip*I
	 *			  I
	 *
	 *	 tr -t [=a=][:punct:] Y[0*2][x*]
	 *	 Entrada: a!@#$%&*()~[=a=]
	 *	 Saida:   Y0xxxxxxxxxxxYxx
	 *
	 *	 tr -s x	
	 *	 Entrada: xx0xx0xxx
	 *   Saida:   x0x0x	
	 *
	 *	 tr -s x 0
	 *	 Entrada: xx0xx0xxx
	 *	 Saida:   00000 
	 *
	 *	 Alem das funcoes...
	 */
}


/************************************************************************************/


int arrumaTabela(int tab, int trunca, char *car_, char *comandoChar_){
    /*   trunca => [valor: 0 == false] [valor: 1 == true]   */

    /*   car_ => sao os caracteres que serao modificados    */
    /*   manipula o vetor digitos                           */

    int comando_ = 0, cont = 0;
    int *Ac_, *Acont;

    Ac_ = &comando_; Acont = &cont;

	/* atribui o valor do comando 
	 * -2 caso seja para deletar
	 * -3 squeeze
	 * -4 complement, nesse caso sao os caracteres que nao devem ser mudadas
	 */
    if(!strcmp(comandoChar_, "~comando\n--delete\\0")) comando_ = -2;
    else if(!strcmp(comandoChar_, "~comando\n--squeeze-repeats\\0")) comando_ = -3;
    else if(!strcmp(comandoChar_, "~comando\n--complement\\0")) comando_ = -4;
    else proximoCaracter(trunca, comandoChar_, Ac_, Acont);

    int i, j;
    for(i = 0; car_[i] != '\0'; i++){
	/* verifica cada caracter ou caso seja um comando do tipo [:???:], '\?' */
		if(erro) return EXIT_FAILURE;
        /* Pega comando do tipo '\?' */
        if(car_[i] == '\'' && car_[i+1] == '\\' && car_[i+3] == '\''){
            switch(car_[i+2]){
				case 'a': digitos[tab][7] = comando_;  break;
                case 'b': digitos[tab][8] = comando_;  break;
                case 't': digitos[tab][9] = comando_;  break;
                case 'n': digitos[tab][10] = comando_; break;
                case 'v': digitos[tab][11] = comando_; break;
                case 'f': digitos[tab][12] = comando_; break;
                case 'r': digitos[tab][13] = comando_; break;
                default:
                    printf("tr: invalid caracter class '\\%c'\n", car_[i+2]);
                    printf("Try 'tr --help' for more information.\n");
                    erro = TRUE;
                    return EXIT_FAILURE;
            }
            i+=3;
            proximoCaracter(trunca, comandoChar_, Ac_, Acont);
        }
        /* pega comando do tipo [=?=] */
        else if(car_[i] == '[' && car_[i+1] == '='
           && car_[i+3] == '=' && car_[i+4] == ']'){
               digitos[tab][(int)car_[i+2]] = comando_;
               proximoCaracter(trunca, comandoChar_, Ac_, Acont);
               i+=4;
        }

        /* Pega comandos do tipo [:?????:] */
        else if(car_[i] == '[' && car_[i+1] == ':'
           && car_[i+7] == ':' && car_[i+8] == ']'){

            char *novoComando;
            novoComando = malloc (sizeof (6));

            novoComando[0] = car_[i+2]; novoComando[1] = car_[i+3]; novoComando[2] = car_[i+4];
            novoComando[3] = car_[i+5]; novoComando[4] = car_[i+6]; novoComando[5] = '\0';

            if(!strcmp(novoComando, "alnum")){
                for(j = 0; j < 128; j++) if(iswalnum(j)){
                    digitos[tab][j] = comando_;
                    proximoCaracter(trunca, comandoChar_, Ac_, Acont);
                }
            }
            else if(!strcmp(novoComando, "alpha")){
                for(j = 0; j < 128; j++) if(iswalpha(j)){
                    digitos[tab][j] = comando_;
                    proximoCaracter(trunca, comandoChar_, Ac_, Acont);
                }
            }
            else if(!strcmp(novoComando, "blank")){
                digitos[tab][9] = comando_;
                proximoCaracter(trunca, comandoChar_, Ac_, Acont);
                digitos[tab][32] = comando_;
                proximoCaracter(trunca, comandoChar_, Ac_, Acont);
            }
            else if(!strcmp(novoComando, "cntrl")){
                for(j = 0; j < 128; j++) if(iswcntrl(j)){
                    digitos[tab][j] = comando_;
                    proximoCaracter(trunca, comandoChar_, Ac_, Acont);
                }
            }
            else if(!strcmp(novoComando, "space")){
                for(j = 0; j < 128; j++) if(iswspace(j)){
                    digitos[tab][j] = comando_;
                    proximoCaracter(trunca, comandoChar_, Ac_, Acont);
                }
            }
            else if(!strcmp(novoComando, "digit")){
                for(j = 0; j < 128; j++) if(iswdigit(j)) {
                    digitos[tab][j] = comando_;
                    proximoCaracter(trunca, comandoChar_, Ac_, Acont);
            }
            }
            else if(!strcmp(novoComando, "graph")){
                for(j = 0; j < 128; j++) if(iswgraph(j)) {
                    digitos[tab][j] = comando_;
                    proximoCaracter(trunca, comandoChar_, Ac_, Acont);
                }
            }
            else if(!strcmp(novoComando, "lower")){
                for(j = 0; j < 128; j++) if(iswlower(j)){
                    if(comando_ == -10) digitos[tab][j] = j - 32;
                    else if(comando_ == -11) digitos[tab][j] = j;
                    else {
                        digitos[tab][j] = comando_;
                        proximoCaracter(trunca, comandoChar_, Ac_, Acont);
                        if(comando_ == -11 || comando_ == -10) j = 0;
                    }
                }
                if(comando_ == -11 || comando_ == -10)
                    proximoCaracter(trunca, comandoChar_, Ac_, Acont);
            }
            else if(!strcmp(novoComando, "upper")){
                for(j = 0; j < 128; j++) if(iswupper(j)){
                    if(comando_ == -11) digitos[tab][j] = j + 32;
                    else if(comando_ == -10) digitos[tab][j] = j;
                    else {
                        digitos[tab][j] = comando_;
                        proximoCaracter(trunca, comandoChar_, Ac_, Acont);
                        if(comando_ == -11 || comando_ == -10) j = 0;
                    }
                }
                if(comando_ == -11 || comando_ == -10)
                    proximoCaracter(trunca, comandoChar_, Ac_, Acont);
            }
            else if(!strcmp(novoComando, "print")){
                for(j = 0; j < 128; j++) if(iswprint(j)) {
                    digitos[tab][j] = comando_;
                    proximoCaracter(trunca, comandoChar_, Ac_, Acont);
                }
            }
            else if(!strcmp(novoComando, "punct")){
                for(j = 0; j < 128; j++) if(iswpunct(j)) {
                    digitos[tab][j] = comando_;
                    proximoCaracter(trunca, comandoChar_, Ac_, Acont);
                }
            }
            else{
                printf("tr: invalid caracter class '%s'\n", novoComando);
                printf("Try 'tr --help' for more information.\n");
                erro = TRUE;
                return EXIT_FAILURE;
            }
            i+=8;
        }
        /* digitos hexadecimal */
        else if(car_[i] == '[' && car_[i+1] == ':'
           && car_[i+8] == ':' && car_[i+9] == ']'){

            char *novoComando;
            novoComando = malloc (sizeof (7));

            novoComando[0] = car_[i+2]; novoComando[1] = car_[i+3]; novoComando[2] = car_[i+4];
            novoComando[3] = car_[i+5]; novoComando[4] = car_[i+6]; novoComando[5] = car_[i+7];
            novoComando[6] = '\0';

            if(!strcmp(novoComando, "xdigit")){
                for(j = 0; j < 128; j++)
                    if(iswxdigit(j)){
                        digitos[tab][j] = comando_;
                        proximoCaracter(trunca, comandoChar_, Ac_, Acont);
                    }
            }
            else{
                printf("tr: invalid caracter class '%s'\n", novoComando);
                printf("Try 'tr --help' for more information.\n");
                erro = TRUE;
                return EXIT_FAILURE;
            }
            i+=9;
        }
        else {
            digitos[tab][(int)car_[i]] = comando_;
            proximoCaracter(trunca, comandoChar_, Ac_, Acont);
        }

        /* mexe nos comandos a-z A-Z 0-9*/
        if(car_[i+1] == '-' && (car_[i+2] > car_[i])){
            for(j = 1; j <= (car_[i+2] - car_[i]); j++){
                digitos[tab][car_[i]+j] = comando_;
                proximoCaracter(trunca, comandoChar_, Ac_, Acont);
            }
            i+=2;
        }
        if(comando_ <= -9) comando_ = -1; /* evita alguns problemas, nao muito util */
    }


    /*!!!!!debugger!!!!!*/

    /*for(i = 0; i < 128; i+=4){
        printf("%i %i %c => %c    ", i, digitos[tab][i], i, digitos[tab][i]);
        printf("%i %i %c => %c    ", i+1, digitos[tab][i+1], i+1, digitos[tab][i+1]);
        printf("%i %i %c => %c    ", i+2, digitos[tab][i+2], i+2, digitos[tab][i+2]);
        printf("%i %i %c => %c\n", i+3, digitos[tab][i+3], i+3, digitos[tab][i+3]);
    }*/


    return EXIT_SUCCESS;
}

/************************************************************************************/

int repeat = 0, sequencia = 0;

int proximoCaracter(int trunca, char *comandoChar2_, int *comando2_, int *cont_)
{
    if(*comando2_ == -2 || *comando2_ == -3 || *comando2_ == -4)
        return EXIT_SUCCESS;

    else if(repeat > 1){
		repeat--;
		return EXIT_SUCCESS;
	}
	else if(repeat == -1) return EXIT_SUCCESS;
	/* [CHAR*] vai até o final */

	else if(comandoChar2_[*cont_] == '\0'){
        if(trunca){
            *comando2_ = -1;
            return EXIT_SUCCESS;
        }
        if(*comando2_ == 32 || *comando2_ == -32) *comando2_ = -1;
        return EXIT_SUCCESS;
    }

	else if(sequencia > 0){
		sequencia--;
		*comando2_ = *comando2_ + 1;
		if(sequencia == 0) *cont_ = *cont_ + 2;
		return EXIT_SUCCESS;
	}

    /* Pega comando do tipo '\?' */
    if(comandoChar2_[*cont_] == '\'' && comandoChar2_[*cont_+1] == '\\'
                                                    && comandoChar2_[*cont_+3] == '\''){
        switch(comandoChar2_[*cont_+2]){
                case 'a': *comando2_ = 7;  break;
                case 'b': *comando2_ = 8;  break;
                case 't': *comando2_ = 9;  break;
                case 'n': *comando2_ = 10; break;
                case 'v': *comando2_ = 11; break;
                case 'f': *comando2_ = 12; break;
                case 'r': *comando2_ = 13; break;
                default:
                    printf("tr: invalid caracter class '\\%c'\n", comandoChar2_[*cont_+2]);
                    printf("Try 'tr --help' for more information.\n");
                    erro = TRUE;
                    return 1;
        }
        *cont_ += 3;
    }
    else if(comandoChar2_[*cont_] == '[' && comandoChar2_[*cont_+1] == ':'
                    && comandoChar2_[*cont_+7] == ':' && comandoChar2_[*cont_+8] == ']'){
		/* na string2 só podem ter dois tipos do comando: [:?????:] upper e lower */
		if(comandoChar2_[*cont_+2] == 'u' && comandoChar2_[*cont_+3] == 'p'
                        && comandoChar2_[*cont_+4] == 'p' && comandoChar2_[*cont_+5] == 'e'
                        && comandoChar2_[*cont_+6] == 'r') *comando2_ = -10;
						/* upper NESTE CASE O COMANDO PARA IDENTIFICAR EH -10 */
        else if(comandoChar2_[*cont_+2] == 'l' && comandoChar2_[*cont_+3] == 'o'
                        && comandoChar2_[*cont_+4] == 'w' && comandoChar2_[*cont_+5] == 'e'
                        && comandoChar2_[*cont_+6] == 'r') *comando2_ = -11;
						/* lower NESTE CASO O COMANDO PARA IDENTIFICAR EH -11 */
		else {
            printf("tr: when translating the only character classes that may appear in\n");
            printf("string2 are 'upper' and 'lower'\n");
            erro = TRUE;
			return 1;
        }
        *cont_ += 8;
    }
	else if(comandoChar2_[*cont_] == '[' && comandoChar2_[*cont_+2] == '*'){
		/* comando [CHAR*] ou [CHAR*NUMBER] */
		*comando2_ = (int)comandoChar2_[*cont_+1];
		if(comandoChar2_[*cont_+3] == ']') repeat = -1; /* repete até o final */
		else{
			int w;
			for(w = 3; comandoChar2_[*cont_+w] != ']'; w++){
				if(comandoChar2_[*cont_+w] == '\0' || comandoChar2_[*cont_+w] < '0' || comandoChar2_[*cont_+w] > '9'){
					erro = TRUE;
					printf("Try 'tr --help' for more information.\n");
					return 1;
				}
			}/* conta quantas vezes o CHAR deve ser repetida */
			int quantaRepe = 0, potencia10 = 1, z = w;
			while(z > 3){
				z--;
				quantaRepe += (((int)comandoChar2_[*cont_ + z]) - 48) * potencia10;
				potencia10 *= 10;
			}

			*cont_ = *cont_ + w + 1;
			repeat = quantaRepe;
			return EXIT_SUCCESS;
		}
	}
    else *comando2_ = (int)comandoChar2_[*cont_];

	/* Reconhece o comando do tipo a-z */
	if(comandoChar2_[*cont_+1] == '-' && (comandoChar2_[*cont_+2] > comandoChar2_[*cont_])){
		sequencia = comandoChar2_[*cont_+2] - comandoChar2_[*cont_];
	}

    *cont_ += 1;
    return EXIT_SUCCESS;
}

/************************************************************************************/

int command_delete() {
    int i, j = 0;
    while (!0){ /* assim como no comando original ela roda em looging sem fim */
        fflush(stdin); /* limpa buffer do teclado */
		fgets(palavra[0], 1024, stdin);
        /*scanf("%[!-~ ]", palavra[0]);*/
		/*o caracter '!' é igual ao inteiro 33 que é o primeiro
		caracter 'digitavel' e o '~' é o inteiro 126*/

        j = 0;
        for(i = 0; palavra[0][i] != '\0'; i++){
            if(digitos[0][(int)palavra[0][i]] == -1){ /* verifica o comando de cada caracter     */
                palavra[1][j] = palavra[0][i];     /* caso o comando dela for diferente de -1 */
                j++;                               /* significa que ela será ignorada         */
            }
        }
        printf("%s", palavra[1]); /* Sai a nova String manipulada */
        for(i = 0; palavra[0][i] != '\0'; i++){
            palavra[1][i] = '\0'; /* limpa as casas da string */
            palavra[0][i] = '\0';
        }
    }
    return EXIT_SUCCESS;
}

/************************************************************************************/

int command_squeeze(int numArg, char *argv2, char *argv3)
{
    int i, j = 0;
    if(numArg == 4) arrumaTabela(1, 0, argv2, argv3);
    /* depois de tirar as repetidas, vamos troca-las */

    while (!0){
        fflush(stdin);
		fgets(palavra[0], 1024, stdin);
        /*scanf("%[!-~ ]", palavra[0]);*/

        j = 0;
        for(i = 0; palavra[0][i] != '\0'; i++){
            if(digitos[0][(int)palavra[0][i]] == -3){ /* se o caracter tiver o comando -3 */
                palavra[1][j] = palavra[0][i];
                 /* devera ter uma copia dela e o resto todas deletadas*/
                while(palavra[1][j] == palavra[0][i+1]) i++;
                j++;
            }
            else {
                palavra[1][j] = palavra[0][i];
                j++;
            }
        }

        trocaCaracteres();
        printf("%s", palavra[1]);

        for(i = 0; palavra[0][i] != '\0'; i++){
            palavra[1][i] = '\0';
            palavra[0][i] = '\0';
        }
    }
    return EXIT_SUCCESS;
}

/************************************************************************************/

int command_complement(char *caracter)
{
    int i,j, k;
    char charOficial;
    for(k = 0; caracter[k] != '\0'; k++) {}
    charOficial = caracter[k-1];

    if(charOficial == '\'' && k >= 4){
        if(caracter[k-4] == '\'' && caracter[k-3] == '\\'){
            switch(caracter[k-2]){
                case 'a': charOficial = 7; break;
                case 'b': charOficial = 8; break;
                case 't': charOficial = 9; break;
                case 'n': charOficial = 10; break;
                case 'f': charOficial = 11; break;
                case 'v': charOficial = 12; break;
                case 'r': charOficial = 13; break;
                default:
                    printf("tr: invalid caracter class '\\%c'\n", caracter[k-2]);
                    printf("Try 'tr --help' for more information.\n");
                    erro = TRUE;
					return 1;
            }
        }
    }
    while(!0){
        fflush(stdin);
		fgets(palavra[0], 1024, stdin);
        /*scanf("%[!-~ ]", palavra[0]);*/

        j = 0;
        for(i = 0; palavra[0][i] != '\0'; i++){
            if(digitos[0][(int)palavra[0][i]] == -4) palavra[1][j] = palavra[0][i];
            else palavra[1][j] = charOficial;
            j++;
        }
        printf("%s", palavra[1]);

        for(i = 0; palavra[0][i] != '\0'; i++){
            palavra[1][i] = '\0';
            palavra[0][i] = '\0';
        }
    }
    return EXIT_SUCCESS;
}

/************************************************************************************/

int trocaCaracteres()
{
    int i;
    for(i = 0; palavra[1][i] != '\0'; i++){
        if(digitos[1][(int)palavra[1][i]] != -1) /* Caso o CHAR tenha comando diferente de -1 */
            palavra[1][i] = digitos[1][(int)palavra[1][i]]; /* substitue */
    }
    return EXIT_SUCCESS;
}


/************************************************************************************/

int executa_help() {

     FILE *file = fopen("help.txt", "r");

     if (file == NULL) {
         puts("Arquivo help.txt not found.");
         return 1;
     }

     putchar('\n');
     while (feof(file) == FALSE) {
         putchar(fgetc(file));
     }
     fclose(file);
     return EXIT_SUCCESS;
}

/***********************************************************************************************
FeedBack

FUNCIONALIDADES BÁSICAS
- squeeze incompleto: -3
total -3

Todas as funcionalidades básicas do tr foram corretamente implementadas.
Só ficou com aquele erro do squeeze que você mesmo apontou.

PROCESSAMENTO DA LINHA DE COMANDO
- não trata opções combinadas: -2
- faltaram certas mensagens de erro: -1
total -3

Suporte a múltiplas opções não implementado.
Não imprime mensagens de erro quando falta argumentos pro programa. Mas tem help.

PROCESSSAMENTO DA ENTRADA
- não trata o EOF: -2
total -2

Não pára o programa no final da entrada.

BÔNUS
- comentários: +5
- implementação inteligente: +10
- organizado: +5
total +20

Bons comentários.
Implementação muito inteligente!
Código bem organizado.

EXTRA (não conta para a nota)
- comentários
- vazamento de memória

Os comentários estão muito bons, mas fez falta comentários explicando como cada função do seu
programa funciona! Algo do tipo "Essa função recebe ... e devolve ..." seria bom.
Além disso, você não está liberando a memória alocada. Lembre-se sempre de desalocar a
memória que você alocou com malloc.

Avaliação à parte, foi um excelente EP! Parabéns!

Descontos totais: -8
Bônus totais: +20
NOTA FINAL: 100/100

***********************************************************************************************/