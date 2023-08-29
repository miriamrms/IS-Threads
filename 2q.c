#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_RECURSOS 3
#define NUM_PROCESSOS 3
#define NUM_THREADS 5
#define MAX_ITERACOES 10

// Estrutura para representar um recurso com um identificador e um mutex
typedef struct Recurso {
    int id;
    pthread_mutex_t lock;
} Recurso;

// Estrutura para representar um processo com um identificador, recursos máximos e recursos alocados
typedef struct Processo {
    int id;
    int max_recursos[NUM_RECURSOS];
    int recursos_alocados[NUM_RECURSOS];
    pthread_mutex_t lock;
} Processo;

Recurso recursos[NUM_RECURSOS];
Processo processos[NUM_PROCESSOS];

bool visitados[NUM_PROCESSOS];
bool pilha[NUM_PROCESSOS];

// Função para realizar uma busca em profundidade para detectar ciclo em um grafo
bool dfs_detectar_ciclo(int no) {
    visitados[no] = true;
    pilha[no] = true;

    for (int i = 0; i < NUM_PROCESSOS; ++i) {
        if (processos[no].recursos_alocados[i] > 0) {
            if (!visitados[i]) {
                if (dfs_detectar_ciclo(i)) {
                    return true;
                }
            } else if (pilha[i]) {
                return true;
            }
        }
    }

    pilha[no] = false;
    return false;
}

// Função para detectar deadlock percorrendo os processos e executando a busca em profundidade
bool detectar_deadlock() {
    for (int i = 0; i < NUM_PROCESSOS; ++i) {
        visitados[i] = false;
        pilha[i] = false;
    }

    for (int i = 0; i < NUM_PROCESSOS; ++i) {
        if (!visitados[i]) {
            if (dfs_detectar_ciclo(i)) {
                return true;
            }
        }
    }

    return false;
}

// Função para alocar um recurso para um processo
bool alocacao_recurso(Processo *processo, int indice_recurso) {
    pthread_mutex_lock(&processo->lock);
    pthread_mutex_lock(&recursos[indice_recurso].lock);

    if (processo->recursos_alocados[indice_recurso] < processo->max_recursos[indice_recurso]) {
        processo->recursos_alocados[indice_recurso]++;
        pthread_mutex_unlock(&recursos[indice_recurso].lock);
        pthread_mutex_unlock(&processo->lock);
        return true;
    }

    pthread_mutex_unlock(&recursos[indice_recurso].lock);
    pthread_mutex_unlock(&processo->lock);
    return false;
}

// Função para liberar um recurso alocado por um processo
void liberar_recurso(Processo *processo, int indice_recurso) {
    pthread_mutex_lock(&processo->lock);
    pthread_mutex_lock(&recursos[indice_recurso].lock);

    if (processo->recursos_alocados[indice_recurso] > 0) {
        processo->recursos_alocados[indice_recurso]--;
    }

    pthread_mutex_unlock(&recursos[indice_recurso].lock);
    pthread_mutex_unlock(&processo->lock);
}

// Função executada pelas threads para simular as alocações e detectar deadlock
void *simular(void *arg) {
    int iteracao = 0;
    while (iteracao < MAX_ITERACOES) {
        for (int i = 0; i < NUM_PROCESSOS; ++i) {
            for (int j = 0; j < NUM_RECURSOS; ++j) {
                if (alocacao_recurso(&processos[i], j)) {
                    printf("Processo %d alocou recurso %d\n", i, j);
                    if (detectar_deadlock()) {
                        printf("Deadlock detectado!\n");
                    }
                    liberar_recurso(&processos[i], j);
                    printf("Processo %d liberou recurso %d\n", i, j);
                }
            }
        }
        iteracao++;
    }
}

int main() {
    // Inicialização dos recursos e processos
    for (int i = 0; i < NUM_RECURSOS; ++i) {
        recursos[i].id = i;
        pthread_mutex_init(&recursos[i].lock, NULL);
    }

    for (int i = 0; i < NUM_PROCESSOS; ++i) {
        processos[i].id = i;
        pthread_mutex_init(&processos[i].lock, NULL);
        for (int j = 0; j < NUM_RECURSOS; ++j) {
            processos[i].max_recursos[j] = 1;
            processos[i].recursos_alocados[j] = 0;
        }
    }

    // Criação e execução das threads para simulação
    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_create(&threads[i], NULL, simular, NULL);
    }

    // Espera todas as threads terminarem
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
