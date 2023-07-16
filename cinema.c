#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>

#define NUM_SALAS 3
#define FILEIRAS 40
#define ASSENTOS 10

typedef struct _LUGAR_OCUPADO{
    int sala, fileira, assento, codigoEstudante;
    unsigned int codigoItasil;
} LUGAR_OCUPADO;

typedef struct _ITASIL{
    unsigned int codigo;
    struct _ITASIL *pNext;
} ITASIL;

typedef struct _ESTUDANTE{
    int codigo;
    struct _ESTUDANTE *pNext;
} ESTUDANTE;

void limparTela();
void escolhaSala();
void preencherLugares();
void comprarIngressos(int sala);
void printaLugares(int sala);
void printaLugaresDisponiveis(int sala);
int retornaNumLugaresDisponiveis(int sala);
int retornaNumeroColuna(char caractereColuna);
int verificaCodigoEstudante(int codigoEstudante);
int verificaCodigoItasil(unsigned int codigoItasil);
int adicionarCodigoEstudante(ESTUDANTE **pHead, int codigoEstudante);
int adicionarCodigoItasil(ITASIL **pHead, int codigoItasil);
int removerCodigoEstudante(ESTUDANTE **pHead);
unsigned int removerCodigoItasil(ITASIL **pHead);
int existeCodigoEstudante(ESTUDANTE *pHead, int codigoEstudante);
int existeCodigoItasil(ITASIL *pHead, int codigoItasil);

char lugares[NUM_SALAS][FILEIRAS][ASSENTOS];
char vetorLetrasMaiusculas[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
char vetorLetrasMinusculas[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};

ITASIL *pHeadItasil, *pHeadItasilAux;
ESTUDANTE *pHeadEstudante, *pHeadEstudanteAux;

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
            printf("Selecione um dos filmes em cartaz para realizar a compra do(s) ingresso(s):\n1 - Velozes e Furiosos 137\n2 - The Flash\n3 - Transformers\n0 - Encerrar programa\nEscolha uma opção: ");
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
    int qntIngressos;

    do{
        printf("Digite o número de ingressos a serem comprados (digite -1 para voltar) -> ");
        scan = scanf("%d", &qntIngressos);
        while(getchar() != '\n');
        if(scan != 1 || qntIngressos < -1 || qntIngressos == 0)
            printf("Quantidade inválida. Tente novamente.\n");
        else if(qntIngressos > numLugaresDisponiveis)
            printf("Não há tantos lugares disponíveis na sala %d\n", sala+1);
    } while(scan != 1 || qntIngressos < -1 || qntIngressos == 0 || qntIngressos > numLugaresDisponiveis);

    if(qntIngressos != -1){
        int i, codigoEstudante, qntIngressosMeiaEntrada, qntIngressosItasil, qntIngressosNormais=qntIngressos, valorTotal, copiaQntIngressosMeiaEntrada, copiaQntIngressosItasil;
        unsigned int codigoItasil;
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
                do{
                    printf("Digite o código de estudante %d (digite 0 para cancelar este desconto) -> ", i+1);
                    scan = scanf("%d", &codigoEstudante);
                    while(getchar() != '\n');
                    if(scan != 1 || codigoEstudante < 0)
                        printf("Código inválido. Tente novamente.\n");
                } while(scan != 1 || codigoEstudante < 0);
                if(codigoEstudante == 0){
                    printf("Desconto cancelado.\n");
                    qntIngressosNormais++;
                    qntIngressosMeiaEntrada--;
                    break;
                }
                else if(codigoEstudante < 10000 || codigoEstudante > 99999)    /*Se tiver menos ou mais de 5 dígitos*/
                    printf("Formatação incorreta. O código de estudante possui 5 dígitos. Tente novamente\n");
                else{     /*Se tiver 5 dígitos*/
                    int validacaoCodigoEstudante = verificaCodigoEstudante(codigoEstudante);
                    if(validacaoCodigoEstudante == 1){
                        /*adicionar o código de estudante na lista encadeada*/
                        int adicao1, adicao2;
                        adicao1 = adicionarCodigoEstudante(&pHeadEstudante, codigoEstudante);      /*Adicionando na lista encadeada principal*/
                        adicao2 = adicionarCodigoEstudante(&pHeadEstudanteAux, codigoEstudante);   /*Adicionando na lista encadeada temporária*/
                        if(adicao1 && adicao2){ 
                            printf("Desconto aplicado com sucesso.\n");
                            break;
                        }
                        else{     /*Se der algum erro, é necessário remover os códigos que tentamos adicionar previamente*/
                            if(adicao1)
                                removerCodigoEstudante(&pHeadEstudante);
                            if(adicao2)
                                removerCodigoEstudante(&pHeadEstudanteAux);
                        }
                    }
                    else if(validacaoCodigoEstudante == -1)
                        printf("Código inválido. Tente novamente.\n");
                    else
                        printf("Este código já foi utilizado no cinema. Tente novamente.\n");
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
                            /*adicionar o código do cliente ITASIL na lista encadeada*/
                            unsigned int adicao1, adicao2;
                            adicao1 = adicionarCodigoItasil(&pHeadItasil, codigoItasil);     /*Adicionando na lista encadeada principal*/
                            adicao2 = adicionarCodigoItasil(&pHeadItasil, codigoItasil);     /*Adicionando na lista encadeada temporária*/
                            if(adicao1 && adicao2){ 
                                printf("Desconto aplicado com sucesso.\n");
                                break;
                            }
                            else{     /*Se der algum erro, é necessário remover os códigos que tentamos adicionar previamente*/
                                if(adicao1)
                                    removerCodigoItasil(&pHeadItasil);
                                if(adicao2)
                                    removerCodigoItasil(&pHeadItasilAux);
                            }
                        }
                        else if(validacaoClienteItasil == -1)
                            printf("Código inválido. Tente novamente.\n");
                        else
                            printf("Este código já foi utilizado no cinema. Tente novamente.\n");
                    }
                }
            }
        }
        limparTela();
        finalizarPedido(sala, qntIngressosNormais, qntIngressosItasil, qntIngressosMeiaEntrada);
    }
    else
        limparTela();
}

void finalizarPedido(sala, qntIngressosNormais, qntIngressosItasil, qntIngressosMeiaEntrada){
    int i, scan, fileira, coluna, valorTotal;
    int qntIngressos=qntIngressosNormais+qntIngressosItasil+qntIngressosMeiaEntrada;
    char colunaChar;
    /*Escolhendo os assentos*/
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
            printf("%d %d\n", fileira, coluna);
            if(fileira < 0 || fileira > 39 || coluna == -1 )
                printf("Assento inválido. Tente novamente.\n");
            else if(lugares[sala][fileira][coluna] == 'X')
                printf("Lugar já ocupado. Tente novamente.\n");
            else{
                LUGAR_OCUPADO *lugar_ocup = (LUGAR_OCUPADO *)malloc(sizeof(LUGAR_OCUPADO));
                lugar_ocup->sala = sala;
                lugar_ocup->fileira = fileira;
                lugar_ocup->assento = coluna;
                if(pHeadEstudanteAux != NULL){
                    lugar_ocup->codigoEstudante = removerCodigoEstudante(&pHeadEstudanteAux);
                    lugar_ocup->codigoItasil = 0;
                }
                else if(pHeadItasilAux != NULL){
                    lugar_ocup->codigoItasil = removerCodigoItasil(&pHeadItasilAux);
                    lugar_ocup->codigoEstudante = 0;
                }
                else{
                    lugar_ocup->codigoEstudante = 0;
                    lugar_ocup->codigoItasil = 0;
                }
                printf("Assento escolhido com sucesso.\n");
                lugares[sala][fileira][coluna] = 'X';
                break;
            }
        }
        
    }

    valorTotal = (qntIngressosNormais * 20) + (qntIngressosMeiaEntrada * 10) + (qntIngressosItasil * 14);
    printf("Preço total: R$%d,00\nAgradecemos pelo pedido. Bom filme!\n\n", valorTotal);
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
    int i;
    for(i = 0; i < ASSENTOS; i++){
        if(caractereColuna == vetorLetrasMaiusculas[i] || caractereColuna == vetorLetrasMinusculas[i])
            return i;
    }
    return -1;
}

/*Retorna -1 (código no formato inválido), -2 (código já usado) ou 1 (código válido). A função verifica se o código de estudante já foi usado na sala e se está no fromato válido*/
int verificaCodigoEstudante(int codigoEstudante){    /*Aqui eu sei que o código de estudante chega com 5 dígitos*/
    int i, codigoRetorno, somaAlgarismos=0, potenciaDez=10000, copiaCodigoEstudante=codigoEstudante;
    for(i=4; i>=0; i--){
        if(i > 0)
            somaAlgarismos += (copiaCodigoEstudante/potenciaDez);
        else{
            if(somaAlgarismos % 10 == (copiaCodigoEstudante/potenciaDez))      /*Checando se a soma dos 4 primeiros algarismos mod10 é igual ao último algarismo*/
                codigoRetorno = 1;
            else
                codigoRetorno = -1;
        }

        copiaCodigoEstudante %= potenciaDez;     /*"Excluindo" o dígito mais à esquerda do código*/
        potenciaDez /= 10;
    }

    if(codigoRetorno == 1){    /*Checando se o código já foi utilizado no cinema*/
        if(existeCodigoEstudante(pHeadEstudante, codigoEstudante))
            codigoRetorno = -2;
    }

    return codigoRetorno;
}

/*Retorna 1 (codigo valido), -1 (codigo invalido) ou -2(codigo ja usado no cinema)*/
int verificaCodigoItasil(unsigned int codigoItasil){
    int validacaoCodigo;    
    if(codigoItasil % 341 == 0 && codigoItasil != 0)
        validacaoCodigo = 1;
    else
        validacaoCodigo = -1;

    if(validacaoCodigo == 1){     /*Checando se o código já foi utilizado no cinema*/
        if(existeCodigoItasil(pHeadItasil, codigoItasil))
            validacaoCodigo = -2;
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

int adicionarCodigoEstudante(ESTUDANTE **pHead, int codigoEstudante){    /*Esta função adicionára sempre no início da lista encadeada*/
    ESTUDANTE *pNovo = (ESTUDANTE *)malloc(sizeof(ESTUDANTE));
    if(!pNovo)
        return 0;
    else{
        pNovo->codigo = codigoEstudante;
        if((*pHead) == NULL)       /*Se a lista estiver vazia*/
            pNovo->pNext = NULL;
        else
            pNovo->pNext = (*pHead);
        (*pHead) = pNovo;
    }

    return 1;
}

int adicionarCodigoItasil(ITASIL **pHead, int codigoItasil){     /*Esta função adicionára sempre no início da lista encadeada*/
    ITASIL *pNovo = (ITASIL *)malloc(sizeof(ITASIL));
    if(!pNovo)
        return 0;
    else{
        pNovo->codigo = codigoItasil;
        if((*pHead) == NULL)      /*Se a lista estiver vazia*/
            pNovo->pNext = NULL;
        else
            pNovo->pNext = (*pHead);
        (*pHead) = pNovo;
    }

    return 1;
}

int removerCodigoEstudante(ESTUDANTE **pHead){     /*Esta função removerá da lista o primeiro código e retornará seu valor*/
    ESTUDANTE *pAux;
    int codigoRetorno;

    pAux = (*pHead);
    if((*pHead) != NULL){
        (*pHead) = (*pHead)->pNext;
        codigoRetorno = pAux->codigo;
        free(pAux);
    }
    else
        codigoRetorno = 0;

    return codigoRetorno;
}

unsigned int removerCodigoItasil(ITASIL **pHead){      /*Esta função removerá da lista o primeiro código e retornará seu valor*/
    ITASIL *pAux;
    unsigned int codigoRetorno;

    pAux = (*pHead);
    if((*pHead) != NULL){
        (*pHead) = (*pHead)->pNext;
        codigoRetorno = pAux->codigo;
        free(pAux);
    }
    else
        codigoRetorno = 0;

    return codigoRetorno;
}

int existeCodigoEstudante(ESTUDANTE *pHead, int codigoEstudante){
    ESTUDANTE *pAux = pHead;
    while(pAux != NULL){
        if(pAux->codigo == codigoEstudante)
            return 1;
        pAux = pAux->pNext;
    }
    return 0;
}

int existeCodigoItasil(ITASIL *pHead, int codigoItasil){
    ITASIL *pAux = pHead;
    while(pAux != NULL){
        if(pAux->codigo == codigoItasil)
            return 1;
        pAux = pAux->pNext;
    }
    return 0;
}