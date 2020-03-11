#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <strings.h>
#include <locale.h>

typedef struct linha {
    int linha;
    char conteudo[200];
} Linha;

typedef struct elemento {
    Linha dados;
    struct elemento *ant;
    struct elemento *prox;
} Elem;

typedef struct elemento* Lista;

void mostraMensagemErro(int nuLinha, int tipoErro, char *palavra);
bool validaCaractereValido(int valorTabelaAscii);
void imprimeConsumoMemoria();
void analisarArquivo(Lista* lista);
bool isCondicaoParada(int valorTabelaAscii);
void limparConteudoVetor(char vetor[]);
FILE* carregarArquivo(char *nome);
Lista* getConteudoArquivo(FILE* arquivo);
Lista* criaLista();
void removePalavrasComAspas(char *palavra, char *palavraComAspas);
int insereListaFinal(Lista* lista, Linha lin);
void liberaLista(Lista* lista);
void memoriaConsumida(int memoria, int situacao);

/**
 * Armazena o total de memoria utilizada no compilador.
 */
int TOTAL_CONSUMO_MEMORIA = 0;

int MAX_TOTAL_CONSUMO_MEMORIA = 256000;

main() {
	setlocale(LC_ALL, "PORTUGUESE");
	
	printf("####################################################################\n");
	printf("# Aluno: Rafael Ferreira Barros\n");
	printf("# Compilador 2020/1\n");
	printf("####################################################################\n\n");
	
	FILE *arquivo;
	arquivo = carregarArquivo("arquivo.txt");
	memoriaConsumida(sizeof(arquivo), 1);

	Lista *linhasArquivo;
	linhasArquivo = getConteudoArquivo(arquivo);
	memoriaConsumida(sizeof(linhasArquivo), 1);
	
	analisarArquivo(linhasArquivo);
	
	imprimeConsumoMemoria();
	liberaLista(linhasArquivo);

	system("pause");
}

/**
 * Imprime em tela o consumo de memoria utilizada.
 */
void imprimeConsumoMemoria() {
	printf("\nCONSUMO DE MEMORIA: %d bytes\n\n", TOTAL_CONSUMO_MEMORIA);
    
    float porcentagem = 0;

	if (MAX_TOTAL_CONSUMO_MEMORIA > 0) {
		porcentagem = (TOTAL_CONSUMO_MEMORIA * 100) / MAX_TOTAL_CONSUMO_MEMORIA;
	}	 

	printf("Porcentagem consumida => %.2f%% de %i bytes\n\n" , porcentagem, MAX_TOTAL_CONSUMO_MEMORIA);
}

/**
 * Analisa o conteudo do arquivo lido.
 */
void analisarArquivo(Lista* lista) {
	int incremento, valorTabelaAscii, numeroLinha;
	char conteudoLinha[UCHAR_MAX], conteudoLinhaComAspas[UCHAR_MAX];

	limparConteudoVetor(conteudoLinha);
	limparConteudoVetor(conteudoLinhaComAspas);

	Elem* no = *lista;
	
	if (lista == NULL) {
        exit(1);
    }

	while (no != NULL) {
		numeroLinha = no->dados.linha;
		strcpy(conteudoLinha, no->dados.conteudo);
		
		removePalavrasComAspas(conteudoLinha, conteudoLinhaComAspas);

		for (incremento = 0; incremento < strlen(conteudoLinha); incremento++) {
			valorTabelaAscii = (int) conteudoLinha[incremento];

			if (!validaCaractereValido(valorTabelaAscii)) {
				printf("O caractere (%c) nao pertence a tabela de literais. [linha - %d].\n", conteudoLinha[incremento], numeroLinha);
				exit(1);
			}

			/*if (isCondicaoParada(valorTabelaAscii)) {
				printf("condicao de parada %s\n", conteudoLinha);
			}*/
		}

		no = no->prox;

		limparConteudoVetor(conteudoLinha);
		limparConteudoVetor(conteudoLinhaComAspas);
	}
}

/**
 * Analisa o conteudo do arquivo lido.
 */
void removePalavrasComAspas(char *palavra, char *palavraComAspas) {
	int 
		i,
		valorAscii,
		countPalavra = 0,
		isPossuiAspas = 0,
		countPalavraValida = 0;

	char palavraComConteudoAspas[UCHAR_MAX];
	char palavraSemConteudoAspas[UCHAR_MAX];

	memoriaConsumida(sizeof(i), 1);
	memoriaConsumida(sizeof(valorAscii), 1);
	memoriaConsumida(sizeof(countPalavra), 1);
	memoriaConsumida(sizeof(isPossuiAspas), 1);
	memoriaConsumida(sizeof(countPalavraValida), 1);
	memoriaConsumida(sizeof(palavraComConteudoAspas), 1);
	memoriaConsumida(sizeof(palavraSemConteudoAspas), 1);

	limparConteudoVetor(palavraComAspas);
	limparConteudoVetor(palavraComConteudoAspas);
	limparConteudoVetor(palavraSemConteudoAspas);

	for (i = 0; i < strlen(palavra); i++) {
		valorAscii = (int) palavra[i];

		if (isPossuiAspas == 0 && valorAscii == 34) {
			isPossuiAspas = 1;
			palavraComConteudoAspas[countPalavra] = palavra[i];
			countPalavra++;
			continue;
		}

		if (isPossuiAspas > 0 && valorAscii != 34) {
			palavraComConteudoAspas[countPalavra] = palavra[i];
			countPalavra++;
			continue;
		}

		if (isPossuiAspas == 1 && valorAscii == 34) {
			palavraComConteudoAspas[countPalavra] = palavra[i];
			countPalavra++;
			isPossuiAspas = 0;
			continue;
		}

		if ((isPossuiAspas == 0)) {
			palavraSemConteudoAspas[countPalavraValida] = palavra[i];
			countPalavraValida++;
		}
	}

	strcpy(palavra, palavraSemConteudoAspas);
	strcpy(palavraComAspas, palavraComConteudoAspas);
}

/**
 * Verifica se o caractere informado é uma condição de parada.
 * As condicoes de parada sao os caracterers : \0, espaco, (, ), virgula, ponto virgula, #, tabs, <, =, >, {, }, [, ]
 *
 * @param valorTabelaAscii
 */
bool isCondicaoParada(int valorTabelaAscii) {
	bool valido = false;
	
	if (
		(valorTabelaAscii == 10)
		|| (valorTabelaAscii == 32)
		|| (valorTabelaAscii == 40)
		|| (valorTabelaAscii == 41)
		|| (valorTabelaAscii == 44)
		|| (valorTabelaAscii == 59)
		|| (valorTabelaAscii == 35)
		|| (valorTabelaAscii == 9)
		|| (valorTabelaAscii == 60)
		|| (valorTabelaAscii == 61)
		|| (valorTabelaAscii == 62)
		|| (valorTabelaAscii == 123)
		|| (valorTabelaAscii == 125)
		|| (valorTabelaAscii == 91)
		|| (valorTabelaAscii == 93)
	) {
		valido = true;
	}
	
	return valido;
}

/**
 * Limpa o lixo contido no vetor.
 *
 * @param char vetor
 */
void limparConteudoVetor(char vetor[]) {
	int i = 0;

	while (i < UCHAR_MAX) {
		vetor[i] = '\0';
		i ++;
	}
}

/**
 * Retorna true se o caracter informado é valido pela tabela de literais.
 *
 * @param valorTabelaAscii
 */
bool validaCaractereValido(int valorTabelaAscii) {
	bool valido = true;
	
	if (
		valorTabelaAscii == 36
		|| valorTabelaAscii == 37
		|| valorTabelaAscii == 38
		|| valorTabelaAscii == 58
		|| valorTabelaAscii == 63
		|| valorTabelaAscii == 64
		|| valorTabelaAscii == 92
		|| valorTabelaAscii == 94
		|| valorTabelaAscii == 96
		|| valorTabelaAscii >= 126
	) {
		valido = false;
	}
	
	return valido;
}

/**
 * Exibe a mensagem de erro encontrada ao validar as linhas do arquivo.
 * 
 * @param int nuLinha
 * @param int tipoErro
 * @param char palavra
 */
void mostraMensagemErro(int nuLinha, int tipoErro, char *palavra) {
	switch(tipoErro) {
		case 1:
			printf("O caractere (%c) nao pertence a tabela de literais. [linha - %d].\n", palavra, nuLinha);
		break;
		default:
			printf("Foi selecionando um tipo de erro nao definido no case.\n");
		break;
	}

	exit(1);
}

/**
 * Método responsável por abrir um arquivo externo.
 *
 * @param nome
 */
FILE* carregarArquivo(char *nome) {

    FILE *arquivo;
    
    arquivo = fopen(nome, "r");
    
    if (arquivo == NULL) {
		printf("\nErro, nao foi possivel abrir o arquivo\n");
	}

	return arquivo;
}

/**
 * Retorna o conteudo da lista.
 *
 * @param FILE* arquivo
 * @return Lista* linhas
 */
Lista* getConteudoArquivo(FILE* arquivo) {
	int nuLinha = 1;
	char conteudo[200];
	Lista* linhas = criaLista();

	while ((fgets(conteudo, sizeof(conteudo), arquivo)) != NULL) {

		if (strlen(conteudo) != 1) {
			Linha item;
			item.linha = nuLinha;
			strcpy(item.conteudo, conteudo);
			insereListaFinal(linhas, item);
		}
					
		nuLinha ++;
	}

	fclose(arquivo);
	return linhas;
}


/**
 * Cria a lista.
 *
 * @return Lista* lista
 */
Lista* criaLista() {
    Lista* lista = (Lista*) malloc(sizeof(Lista));

    if (lista != NULL) {
        *lista = NULL;
    }

    return lista;
}

/**
 * Insere no final da lista de acordo com parametros informados.
 *
 * @param Lista* lista
 * @param Linha lin
 * @return int
 */
int insereListaFinal(Lista* lista, Linha lin) {
    if (lista == NULL) {
        return 0;
    }

    Elem *no;
    no = (Elem*) malloc(sizeof(Elem));
    if (no == NULL) {
        return 0;
    }

    no->dados = lin;
    no->prox = NULL;

    // lista vazia: insere inicio
    if ((*lista) == NULL) {
        no->ant = NULL;
        *lista = no;
    } else {
        Elem *aux;
        aux = *lista;

        while (aux->prox != NULL) {
            aux = aux->prox;
        }

        aux->prox = no;
        no->ant = aux;
    }
    return 1;
}

/**
 * Remove todos os elementos da lista.
 *
 * @param Lista* lista
 */
void liberaLista(Lista* lista) {
    if (lista != NULL) {
        Elem* no;

        while ((*lista) != NULL) {
            no = *lista;
            *lista = (*lista)->prox;
            free(no);
        }

        free(lista);
    }
}

/**
 * Conta a quantidade de memoria consumida, incrementado e subtraindo da constante TOTAL_CONSUMO_MEMORIA
 *
 * http://www.cprogressivo.net/2013/03/A-funcao-sizeof-e-blocos-vizinhos-de-memoria-em-C.html
 *
 * @param int memoria
 * @param int situacao: 1 - incrementa, 2 - subtrai
 */
void memoriaConsumida(int memoria, int situacao) {
	float percentMemoriaConsumida = 0;

	if (situacao == 1) {
		TOTAL_CONSUMO_MEMORIA += memoria;	
	} else {
		TOTAL_CONSUMO_MEMORIA -= memoria;
	}

	if (MAX_TOTAL_CONSUMO_MEMORIA > 0) {
		percentMemoriaConsumida = (TOTAL_CONSUMO_MEMORIA * 100) / MAX_TOTAL_CONSUMO_MEMORIA;
	}

	if (percentMemoriaConsumida > 90 && percentMemoriaConsumida < 99) {
		printf("Sua memoria esta entre 90%% a 99%% do total disponivel, memoria atual: %.2f%\n\n", percentMemoriaConsumida);
	}
	
	if (TOTAL_CONSUMO_MEMORIA > MAX_TOTAL_CONSUMO_MEMORIA) {
		printf("Memoria Insuficiente");
	}
}
