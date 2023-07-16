#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>

#define NUM_SALAS 3
#define FILEIRAS 40
#define ASSENTOS 10

void limparTela();
void escolhaSala();
void preencherLugares();
void comprarIngressos(int sala);
void printaLugares(int sala);
void printaLugaresDisponiveis(int sala);
int retornaNumLugaresDisponiveis(int sala);
int retornaNumeroColuna(char caractereColuna);
int verificaCodigoEstudante(char *codigoEstudanteString, int *codigoEstudante);
int verificaCodigoItasil(unsigned int codigoItasil);

char lugares[NUM_SALAS][FILEIRAS][ASSENTOS];
char vetorLetrasMaiusculas[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
char vetorLetrasMinusculas[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};

typedef struct _SALA{
    int sala, fileira, assento, codigoEstudante;
    unsigned int codigoItasil;
} SALA;

typedef struct _ITASIL{
    unsigned int codigo;
    struct _ITASIL *pNext;
} ITASIL;

typedef struct _ESTUDANTE{
    int codigo;
    struct _ESTUDANTE *pNext;
} ESTUDANTE;

int main(){
    setlocale(LC_ALL, "Portuguese.Brazil");
    limparTela();
    preencherLugares();    /*Aqui, os lugares de todas as salas serão preenchidos*/
    
    escolhaSala();
    printf("\nPrograma encerrado.\n");
    return 0;
}

void limparTela(void){   /*Função para limpar a tela do console, ainda fazendo a verificacao do sistema operacional*/
    #ifdef _linux_
        system("clear");
    #elif _WIN32
        system("cls");
    #endif
}

void escolhaSala(void){
    int sair=0, scanInteiro, esc;
    while(sair != 1){
        do{
            printf("Selecione um dos filmes em cartaz para realizar a compra do(s) ingresso(s):\n1 - Velozes e Furiosos 137\n2 - The Flash\n3 - Transformers\nEscolha uma opção: ");
            scanInteiro = scanf("%d", &esc);;        /*Fazendo a verificação para valores inteiros usando o retorno da função scanf*/
            while(getchar() != '\n');
            if(scanInteiro != 1){
                limparTela();
                printf("Valor inválido. Tente novamente.\n");
            }
        } while(scanInteiro != 1);
        esc -= 1;   /*Para o usuário, as salas vão de 1 a 3. Mas para a lógica so programa, elas vão de 0 a 2. Por isso é necessário diminuir 1 unidade*/
        switch(esc){
            case -1:   /*Se o usuário digitar a sala 0, o programa deve ser finalizado*/
                sair = 1;
                break;
            case 0:
                 comprarIngressos(esc);    /*sala 1*/
                break;
            case 1:
                comprarIngressos(esc);     /*sala 2*/
                break;
            case 2:
                comprarIngressos(esc);     /*sala 3*/
                break;
            default:
                limparTela();
                printf("Opção inválida. Tente novamente.\n");
                break;
        }
    }
}

void comprarIngressos(int sala){
    int scan;
    int numLugaresDisponiveis = retornaNumLugaresDisponiveis(sala);    /*Verificando o número de lugares ainda disponíveis na sala*/
    int qntIngressos, fileira, coluna;
    char colunaChar;

    do{
        printf("Digite o número de ingressos a serem comprados (digite -1 para voltar) -> ");
        scan = scanf("%d", &qntIngressos);
        while(getchar() != '\n');
        if(scan != 1 || qntIngressos < -1 || qntIngressos == 0)
            printf("Quantidade inválida. Tente novamente.\n");
        else if(qntIngressos > numLugaresDisponiveis)
            printf("Não há %d lugares disponíveis na sala %d", qntIngressos, sala);
    } while(scan != 1 || qntIngressos < -1 || qntIngressos == 0 || qntIngressos > numLugaresDisponiveis);

    if(qntIngressos != -1){
        int i, qntIngressosMeiaEntrada, qntIngressosItasil, qntIngressosNormais=qntIngressos, valorTotal, copiaQntIngressosMeiaEntrada, copiaQntIngressosItasil;
        unsigned int codigoItasil;
        char codigoEstudante[6];
        do{
            printf("Digite o número de meias-entradas (ingressos restantes: %d) -> ", qntIngressosNormais);
            scan = scanf("%d", &qntIngressosMeiaEntrada);
            while(getchar() != '\n');
            if(scan != 1 || qntIngressosMeiaEntrada < 0 || qntIngressosMeiaEntrada > qntIngressosNormais)
                printf("Quantidade inválida. Tente novamente.\n");
        } while(scan != 1 || qntIngressosMeiaEntrada < 0 || qntIngressosMeiaEntrada > qntIngressosNormais);
        qntIngressosNormais -= qntIngressosMeiaEntrada;
        copiaQntIngressosMeiaEntrada = qntIngressosMeiaEntrada;

        for(i=0; i<copiaQntIngressosMeiaEntrada; i++){
            while(1){
                printf("Digite o código de estudante %d (digite 0 para cancelar este desconto) -> ", i+1);
                fgets(codigoEstudante, 6, stdin);
                if(codigoEstudante[strlen(codigoEstudante)-1] == '\n'){
                    codigoEstudante[strlen(codigoEstudante)-1] = '\0';
                }
                /*DEPOIS ANALISAR ISSO MELHOR (STRING NÃO TEM \n)*/
                /*printf("%d\n", strlen(codigoEstudante));*/
                
                if(strlen(codigoEstudante) == 5)
                    while (getchar() != '\n');   /*Limpando o buffer*/
                
                if(strlen(codigoEstudante) != 5 && strcmp(codigoEstudante, "0") != 0)
                    printf("Formatação incorreta.\n");
                else{
                    if(strcmp(codigoEstudante, "0") == 0){
                        printf("Desconto cancelado.\n");
                        qntIngressosNormais++;
                        qntIngressosMeiaEntrada--;
                        break;
                    }
                    else{
                        int codigoEstudanteInt;
                        int validacaoCodigoEstudante = verificaCodigoEstudante(codigoEstudante, &codigoEstudanteInt);
                        if(validacaoCodigoEstudante == 1){
                            /*adicionar o código de estudante na lista encadeada*/
                            printf("Desconto aplicado com sucesso.\n");
                            break;
                        }
                        else if(validacaoCodigoEstudante == -1)
                            printf("Formatação incorreta. Tente novamente.\n");
                        else
                            printf("Código já foi utilizado no cinema. Tente novamente.\n");
                    }
                }
            }
        }

        if(qntIngressosNormais > 0){    /*Se ainda tiver ingressos disponíveis*/
            do{
                printf("Digite o número de clientes Itasil (ingressos restantes: %d) -> ", qntIngressosNormais);
                scan = scanf("%d", &qntIngressosItasil);
                while(getchar() != '\n');
                if(scan != 1 || qntIngressosItasil < 0 || qntIngressosItasil > qntIngressosNormais)
                    printf("Quantidade inválida. Tente novamente.\n");
            } while(scan != 1 || qntIngressosItasil < 0 || qntIngressosItasil > qntIngressosNormais);
            qntIngressosNormais -= qntIngressosItasil;
            copiaQntIngressosItasil = qntIngressosItasil;

            for(i=0; i<copiaQntIngressosItasil; i++){
                while(1){
                    do{
                        printf("Digite o código do cliente Itasil %d (digite 0 para cancelar este desconto) -> ", i+1);
                        scan = scanf("%u", &codigoItasil);
                        while(getchar() != '\n');
                        if(scan != 1)
                            printf("Código inválido. Tente novamente.\n");
                    } while(scan != 1);

                    if(codigoItasil == 0){
                        qntIngressosNormais++;
                        qntIngressosItasil--;
                        break;
                    }
                    else{
                        int validacaoClienteItasil = verificaCodigoItasil(codigoItasil);
                        if(validacaoClienteItasil == 1){
                            /*adicionar o código de cliente Itasil na lista encadeada*/
                            printf("Desconto aplicado com sucesso.\n");
                            break;
                        }
                        else if(validacaoClienteItasil == -1)
                            printf("Formatação incorreta. Tente novamente.\n");
                        else
                            printf("Código já foi utilizado no cinema. Tente novamente.\n");
                    }
                }
            }
        }

        /*escolher assentos*/
        printaLugares(sala);   /*Mostrando o mapa da sala*/
        for(i=0; i<qntIngressos; i++){
            while(1){
                do{
                    printf("Selecione o assento para o ingresso (%d/%d) -> ", i+1, qntIngressos);
                    scan = scanf("%d %c", &fileira, &colunaChar);
                    while(getchar() != '\n');
                    if(scan != 2)
                        printf("Assento inválido. Tente novamente.\n");
                } while(scan != 2);

                fileira = fileira-1;
                coluna = retornaNumeroColuna(colunaChar);
                if(fileira < 0 || fileira > 39 || coluna == -1 )
                    printf("Assento inválido. Tente novamente.\n");
                else if(lugares[sala][fileira][coluna] == 'X')
                    printf("Lugar já ocupado. Tente novamente.\n");
                else{
                    printf("Assento escolhido com sucesso.\n");
                    lugares[sala][fileira][coluna] = 'X';
                    break;
                }
            }
            
        }


        valorTotal = (qntIngressosNormais * 20) + (qntIngressosMeiaEntrada * 10) + (qntIngressosItasil * 14);
        printf("Preço total: R$%d,00\nAgradecemos pelo pedido. Bom filme!\n\n", valorTotal);
    }
    else
        limparTela();
}

/*Printa o mapa da sala, informando lugares ocupados e vazios*/
void printaLugares(int sala){
    int i, j;
    printf("==================SALA %d==================\n", sala);
    printf("    ");
    for(i=0; i<ASSENTOS; i++){
        printf("%c", vetorLetrasMaiusculas[i]);  
        if(i < ASSENTOS-1)
            printf("   ");  
    }
    printf("\n");
    for(i=0; i<FILEIRAS; i++){
        printf("%02d  ", i+1);
        for(j=0; j<ASSENTOS; j++){
            if(lugares[sala][i][j] == '0')
                printf("0");
            else
                printf("X");
            if(j < ASSENTOS-1)
                printf(" - ");
        }
        printf("\n");
    }
    printf("==============TELA DO CINEMA==============\n");
}

/*Preenche a matriz de lugares de todas as salas assim que o programa é iniciado*/
void preencherLugares(){
    int i, j, k;
    for(i=0; i<NUM_SALAS; i++){
        for(j=0; j<FILEIRAS; j++){
            for(k=0; k<ASSENTOS; k++){
                lugares[i][j][k] = '0';
            }
        }
    }
}

/*Retorna o índice correspondente à letra digitada para a poltrona*/
int retornaNumeroColuna(char caractereColuna){
    int achou=0, i, indice;
    for(i = 0; i < ASSENTOS; i++){
        if(caractereColuna == vetorLetrasMaiusculas[i])
            achou = 1;
        else{
            if(caractereColuna == vetorLetrasMinusculas[i])
                achou = 1;
        }
    }
    if(achou == 1)
        indice = i;
    else
        indice = -1;
    return indice;
}

/*Retorna -1 (código no formato inválido), -2 (código já usado) ou 1 (código válido). A função verifica se o código de estudante já foi usado na sala e se está no fromato válido*/
int verificaCodigoEstudante(char *codigoEstudanteString, int *codigoEstudante){
    int i, codigoRetorno, somaAlgarismos=0;
    for(i=0; i<5; i++){
        if(codigoEstudanteString[i] > '9' || codigoEstudanteString[i] < '0'){    /*Verificando, por meio da tabela ASCII, se o caractere é um algarismo*/
            codigoRetorno = -1;
            break;
        }
        if(i < 4)
            somaAlgarismos += (codigoEstudanteString[i] - '0');    /*Subtraindo o caractere '0', eu consigo transformar o valor do caractere para o inteiro correspondente*/
        else{
            if(somaAlgarismos % 10 == (codigoEstudanteString[i] - '0'))    /*Checando se a soma dos 4 primeiros algarismos mod10 é igual ao último algarismo*/
                codigoRetorno = 1;
            else
                codigoRetorno = -1;
        }
    }

    if(codigoRetorno == 1)
        *(codigoEstudante) = atoi(codigoEstudanteString);    /*codigoEstudante receberá o valor do código como inteiro*/

    return codigoRetorno;
}

/*Retorna 1 (codigo valido), -1 (codigo invalido) ou -2(codigo ja usado no cinema)*/
int verificaCodigoItasil(unsigned int codigoItasil){
    int validacaoCodigo;    
    if(codigoItasil % 341 == 0)
        validacaoCodigo = 1;
    else{
        if(codigoItasil % 341 != 0)
            validacaoCodigo = -1;
    }
    return validacaoCodigo;
}

/*Retorna o número de lugares disponíveis em determinada sala*/
int retornaNumLugaresDisponiveis(int sala){
    int i, j, numLugaresDisponiveis=0;
    for(i=0; i<FILEIRAS; i++){
        for(j=0; j<ASSENTOS; j++){
            if(lugares[sala][i][j] == '0')
                numLugaresDisponiveis++;
        }
    }    
    
    return numLugaresDisponiveis;
}