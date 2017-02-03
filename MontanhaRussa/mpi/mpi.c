#include <mpi.h>
#include <stdlib.h> 
#include <stdio.h> 
#include <time.h> 
#include <unistd.h>
#include <getopt.h>
#include <limits.h>

#define CARRINHO 0

int numPassageiros;
int capacidadeCarrinho;
long tempoPasseio;
int numPasseios;

int lugaresOcupados;
long tempoEntrarCarrinho = 2;

int ultimoPassageiro = 1;

void subirCarrinho() {
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

void logicaMontanhaRussa(int rank) {
	if (rank == CARRINHO) {
		while (numPasseios--) {
			int passageiroAtual = ultimoPassageiro;
			while (!podeComecarPasseio()) {
				int passageiro;
				MPI_Recv(&passageiro, 1, MPI_INT, passageiroAtual, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				passageiroAtual++;
				lugaresOcupados++;
				printf("\nPassageiro %d subiu no carrinho - Capacidade do Carrinho %d\n", passageiro, capacidadeCarrinho - lugaresOcupados);
				if (passageiroAtual > numPassageiros)
					passageiroAtual = 1;
			}
			comecarPasseio();
			ultimoPassageiro = passageiroAtual;
		}
	} else {
		int carrinhoAberto = 1;
		while (carrinhoAberto) {
			printf("Passageiro %d na fila para entrar no carrinho..\n", rank);
			subirCarrinho();
			MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		}
	}
}

void imprimeUsage() {
    printf("Help\n\n");
    printf("Uso: ./mpi <parametro> <valor>\n\n");
    printf("  -c <capacidade>     capacidade do carrinho\n");
    printf("  -v <voltas>         numero de passeios do carrinho\n\n");
    printf("  -t <tempo>          tempo de passeio\n\n");
}

int main(int argc, char **argv) {
	char c;
    while ((c = getopt (argc, argv, "c:v:t:h")) != -1){
	    switch (c){
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
	
	int totalRanks;
	int rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &totalRanks);

	lugaresOcupados = 0;
	numPassageiros = totalRanks - 1;

	if (capacidadeCarrinho > numPassageiros) {
		printf("Capacidade do Carrinho é maior do que o número de passageiros\n");
		exit(0);
	}

	if (rank == CARRINHO) {
		printf("Numero de Passageiros %d\n", numPassageiros);
		printf("Capacidade do Carrinho %d\n", capacidadeCarrinho);
		printf("Numero de Passeios %d\n", numPasseios);
		printf("Tempo de Passeio %lus\n\n", tempoPasseio);
	}

	logicaMontanhaRussa(rank);

	if (rank == CARRINHO) {
		printf("\n\nTodos os passeios foram realizados, Montanha Russa fechada\n");
		printf("Tempo Execucao %llus\n", time(NULL) - tempoExecucao);
		MPI_Abort(MPI_COMM_WORLD, 0);
	}
	MPI_Finalize();
	return 0;
}