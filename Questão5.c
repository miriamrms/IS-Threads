#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

//Declarando variáveis iniciais

bool *array_booleano;         //Array boleano para números primos   
int num_threads;             //Nº de threads (entrada)
int N;                      //Nº limite para ser primo

//Inicializar uma variável mutex para controle das threads 

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex para controle das threads

//FUNÇÃO PARA ELIMINAR OS MÚLTIPLOS DO PRÓXIMO PRIMO

void *eliminar_multiplos(void *id_array) {
    int num_primo = *((int *)id_array); //o num é o índice do array

    //laço que percorre todos os múltiplos de num_primo
    int l=2;

    for (int i = l * num_primo; i < N; i = l*num_primo) {
        if (array_booleano[i]) {
            pthread_mutex_lock(&mutex);
            array_booleano[i] = 0;
            pthread_mutex_unlock(&mutex);
        }
        l++;
    }

    pthread_exit(NULL);
}

int main() {

    //DECLARAR VARIÁVEIS

    int i,j;
    int num_primo = 2;

    //Inicialziando clock para medir quanto tempo o código leva pra executar
    //Para testar com números diferentes de threads

    double time_spent = 0.0;
 
    clock_t begin = clock();

    //RECEBENDO AS ENTRADAS 

    printf("Digite o número de threads: ");
    scanf("%d", &num_threads);

    printf("Digite o valor de N: ");
    scanf("%d", &N);

    //ALOCANDO MEMÓRIA AO ARRAY DE NUM PRIMOS

    array_booleano = (bool *)malloc(N * sizeof(bool));
    if (array_booleano == NULL) {
        printf("Memória não alocada!\n");
    }

    //INICIALIZAR ARRAY_PRIMO COMO TRUE: TODOS OS NUM SERÃO CONSIDERADOS PRIMOS ATÉ SEREM "ELIMINADOS"    
    //COM EXCESSÃO DE [0] E [1]: NÃO SEI AINDA SE VAI SER RELEVANTE ADICIONAR ESSE.

    //array_booleano[0]=0;
    //array_booleano[1]=0;

    for (i = 2; i < N; i++) {
        array_booleano[i] = 1;
    }

    //Teste para verificar se o array foi preenchido
    /*
    for (int i = 2; i < N; i++) {
        printf("%d\n",array_primos[i]);
    }
    */

    //CRIANDO AS THREADS

    pthread_t array_threads[num_threads];
    int array_primos[num_threads];

    //laço para criar threads

    for (i=0; i<num_threads && num_primo<N; i++) {
        //inicializa com o número primo = 2; num_primo inicialmente é 2.
        array_primos[i] = num_primo;
        pthread_create(&array_threads[i], NULL, eliminar_multiplos, (void *)&array_primos[i]);

        //encontrando o próximo primo (laço percorre enquanto o num for menor que o limite e o valor booleano do array seja "false")
        while (num_primo < N && !array_booleano[num_primo]) {
            num_primo++;
        }
         num_primo++;
    }

    //ENCERRA AS THREADS APÓS ENCONTRAR OS NUM PRIMOS

    for(j = 0; j < num_threads; j++){
        pthread_join(array_threads[j], NULL);
    }

    //PRINTAR NA TELA OS NUMEROS PRIMOS ENCONTRADOS

    printf("\nOs números primos até %d são:\n", N);
    for(j = 2; j < N; j++){
        if(array_booleano[j]){
            printf("%d ", j);
        }
    }

    free(array_booleano); //libera a memória alocada para o array

    //FINALIZA O CLOCK COM O TEMPO DE EXECUÇÃO DO PROGRAMA

    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;  // calcula tempo de execução
    printf("\n\nTempo de execução: %f segundos\n", time_spent);

    return 0;
}

