#include <stdio.h>
#include <pthread.h>

// Os seguintes valores devem ser definidos antes da execução do programa conforme a necessidade do usuário
#define numLinhas 9 // Número de linhas do mapa
#define numColunas 20 // Número de colunas do mapa
int numThreads = 5; // Número de threads a serem usadadas
int mapa[numLinhas][numColunas] = {
    {0,0,0,1,0,0,0,0,1,0,0,1,0,0,0,1,0,0,1,0},
    {0,1,0,0,0,0,1,1,1,0,1,1,0,0,0,1,0,1,0,0},
    {0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0},
    {1,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0}
};
int posicoesVisitadas[numLinhas][numColunas] = {0}; // Matriz de posições visitadas inicializada com 0

int numIlhas = 0; // Contador de ilhas, iniciado com 0

pthread_mutex_t numIlhasMutex = PTHREAD_MUTEX_INITIALIZER; // Mutex para proteger a numIlhas

// Função que verifica se uma posição (i, j) está dentro dos limites do mapa
int posicaoValida(int linha, int coluna) {
    return (linha >= 0 && linha < numLinhas && coluna >= 0 && coluna < numColunas);
}

// Função para explorar a ilha a partir de uma posição (i, j)
void analisarIlha(int linha, int coluna) {

    // Caso a posição do mapa a ser analisada não seja válida ou se a posição já tenha sido visitada e analisada ou a posição seja um 0 a função é encerrada
    if (!posicaoValida(linha, coluna) || posicoesVisitadas[linha][coluna] || mapa[linha][coluna] == 0)
        return;

    // Marcando que a posição foi visitada
    posicoesVisitadas[linha][coluna] = 1;

    // Explorando todas as 8 posições diferentes adjacentes a posição sendo analisada (horizontal, vertical e diagonal)
    int coordenadasLinhas[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int coordenadasColunas[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    for (int i = 0; i < 8; i++) {
        analisarIlha(linha + coordenadasLinhas[i], coluna + coordenadasColunas[i]);
    }
}

// Função executada por cada thread
void *threadFunction(void *arg) {
    int threadId = *(int *)arg;

    // Variável que define a difença entre a primeira linha a ser analisada e a linha a ser analisada pela thread
    int jump = numLinhas / numThreads;

    int linhaInicial = threadId * jump;
    int linhaFinal = (threadId == numThreads - 1) ? numLinhas : linhaInicial + jump;


    for (int i = linhaInicial; i < linhaFinal; i++) {
        for (int j = 0; j < numColunas; j++) {
            if (mapa[i][j] == 1 && !posicoesVisitadas[i][j]) {
                analisarIlha(i, j);
                pthread_mutex_lock(&numIlhasMutex);
                numIlhas++;
                pthread_mutex_unlock(&numIlhasMutex);
            }
        }
    }

    pthread_exit(NULL);
}

int main() {

    pthread_t threads[numThreads];
    int threadIds[numThreads];

    for (int i = 0; i < numThreads; i++) {
        threadIds[i] = i;
        pthread_create(&threads[i], NULL, threadFunction, &threadIds[i]);
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Numero de ilhas: %d\n", numIlhas);

    return 0;
}

