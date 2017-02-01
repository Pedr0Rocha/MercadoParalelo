#include <stdlib.h> 
#include <stdio.h> 
#include <time.h> 
#include <unistd.h>
#include <getopt.h>
#include <limits.h>

int numPassageiros;
int capacidadeCarrinho;
long tempoPasseio;
int numPasseios;

int lugaresOcupados;
int numeroPortasCarrinho = 4;
long tempoEntrarCarrinho = 2;

int ultimoPassageiro = 1;

void subirCarrinho(int rank) {
	lugaresOcupados++;
	printf("Passageiro %d subindo no carrinho - Capacidade do Carrinho %d\n", rank, capacidadeCarrinho - lugaresOcupados);
	sleep(tempoEntrarCarrinho);
}

void comecarPasseio() {
	printf("\nCarrinho Cheio!\n");
	printf("Carrinho saindo para passeio...\n");
	sleep(tempoPasseio);
	lugaresOcupados = 0;
	printf("Carrinho de volta! Passeios restando %d\n\n", numPasseios);
}

int podeComecarPasseio() {
	if (capacidadeCarrinho - lugaresOcupados == 0) return 1;
	return 0;
}

void logicaMontanhaRussa() {
	while (numPasseios--) {
		int passageiroAtual = ultimoPassageiro;
		while (!podeComecarPasseio()) {
			subirCarrinho(passageiroAtual);
			passageiroAtual++;
			if (passageiroAtual > numPassageiros)
				passageiroAtual = 1;
		}
		comecarPasseio();
		ultimoPassageiro = passageiroAtual;
	}
}

void imprimeUsage() {
    printf("Help\n\n");
    printf("Uso: ./seq <parametro> <valor>\n\n");
    printf("  -n <passageiros>    numero de passageiros\n");
    printf("  -c <capacidade>     capacidade do carrinho\n");
    printf("  -v <voltas>         numero de passeios do carrinho\n\n");
    printf("  -t <tempo>          tempo de passeio\n\n");
}

int main(int argc, char **argv) {
	char c;
    while ((c = getopt (argc, argv, "n:c:v:t:h")) != -1){
	    switch (c){
	        case 'n':
	            numPassageiros = atoi(optarg);
	            break;
	        case 'c':
	            capacidadeCarrinho = atoi(optarg);
	        case 'v':
	            numPasseios = atoi(optarg);
	            break;
	        case 't':
	            tempoPasseio = strtoull(optarg, NULL, 10);
	            break;
	        case 'h':
	            imprimeUsage();
	            exit(0);
	        case '?':
	            printf("Erro nos Parametros.\n");
	        default:
	            abort();
	    }
	}
	unsigned long long tempoExecucao = time(NULL);

	unsigned long long seed = time(NULL);
	srand(seed);

	lugaresOcupados = 0;

	printf("Numero de Passageiros %d\n", numPassageiros);
	printf("Capacidade do Carrinho %d\n", capacidadeCarrinho);
	printf("Numero de Passeios %d\n", numPasseios);
	printf("Tempo de Passeio %lus\n\n", tempoPasseio);

	logicaMontanhaRussa();

	printf("Tempo Execucao %llus\n", time(NULL) - tempoExecucao);
	return 0;
}