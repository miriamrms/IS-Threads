#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define tamMax 10 //tamanho máximo das senhas e dos números das threads

int numThreads;
char senha[tamMax+1];
char senhaEncontrada[tamMax +1] = {0};

pthread_mutex_t mutex;

//Função executada por cada thread
void *encontrarSenha(void *threadId) {
    int id = *((int *)threadId);
    
    //Itera sobre os caracteres da senha em posições separadas por numThreads
    for (int i=id; i<tamMax;i+=numThreads) {
        //Testa todos os caracteres ASCII de 32 a 126
        for (int c=32; c <=126;c++) {
            if (senha[i]==c) {
                pthread_mutex_lock(&mutex);  //Bloqueia o acesso concorrente à senhaEncontrada
                senhaEncontrada[i] = c;      //Atualiza a senha encontrada na posição i
                pthread_mutex_unlock(&mutex);  //Libera o acesso à senhaEncontrada
                break;  //Encerra o loop se encontrar o caractere correspondente
            }
        }
    }
    free(threadId); //Libera memória alocada para threadId
    pthread_exit(NULL);//Encerra thread
}

int main() {
    printf("Digite a senha: \n");
    scanf("%s", senha);
    
    if (strlen(senha)> tamMax) {
        printf("Senha tem mais de 10 caracteres.\n");
        return 1;
    }
    
    printf("Digite a quantidade de threads: \n");
    scanf("%d", &numThreads);

    if (numThreads>tamMax) {
        printf("Número máximo de threads excedido.\n");
        return 1;
    }
    
    //Inicia o temporizador
    clock_t start = clock();
    
    pthread_t threads[numThreads];//Array com as threads
    pthread_mutex_init(&mutex, NULL);  //Inicializa o mutex para controle de acesso

    // Cria threads para buscar a senha
    for (int i =0; i<numThreads; i++) {
        int *threadId = (int *)malloc(sizeof(int));
        *threadId = i;
        pthread_create(&threads[i], NULL, encontrarSenha, (void *)threadId);
    }

    //Aguarda o término de todas as threads
    for (int i =0; i<numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);//Libera o mutex após o uso

    //Finaliza o relógio e calcula o tempo
    clock_t end = clock();
    double tempoTotal = (double)(end-start)/CLOCKS_PER_SEC;
    printf("Tempo de execução: %lf para %d thread(s)\n", tempoTotal, numThreads);
    
    printf("A senha encontrada foi: %s\n", senhaEncontrada);

    return 0;
}

//Escolhendo algumas senhas arbitrárias: 

//-Para a senha mypass1234:
// Tempo de execução: 0.000219 para 1 thread(s)
// Tempo de execução: 0.000857 para 5 thread(s)
// Tempo de execução: 0.001166 para 10 thread(s)

//-Para a senha Is56@#%6
// Tempo de execução: 0.000224 para 1 thread(s)
// Tempo de execução: 0.001416 para 5 thread(s)
// Tempo de execução: 0.001029 para 10 thread(s)

//-Para a senha *s90ddi$&E
// Tempo de execução: 0.000502 para 1 thread(s)
// Tempo de execução: 0.000783 para 5 thread(s)
// Tempo de execução: 0.001411 para 10 thread(s)

//É perceptível que na maioria dos casos mais threads resultou em mais tempo 
//de execução, o que deve fazer sentido para nossa quantidade pequena de dados
//considerando que acrescentamos mais custo ao inicializar mais threads

