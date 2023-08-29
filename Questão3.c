#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <pthread.h>

/*REGRAS
1. THREADS LEITORAS PODEM SIMULTANEAMENTE ACESSAR A REGIÃO CRÍTICA
2. THREADS LEITORAS PRECISAM SER PROIBIDAS DE REALIZAR LEITURA ENQUANTO A THREAD ESCRITORA ESTÁ SENDO EXECUTADA
3. THREADS ESCRITORAS AO ACESSAR A REGIÃO CRÍTICA BLOQUEIAM TODAS AS THREADS ESCRITORAS E LEITORAS.
USAR EXCLUSÃO MÚTUA.
*/

/*INFOS
N RECEBE O NUM DE THREADS DE LEITURA
M RECEBE O NUM DE THREADS DE ESCRITA
A BASE DE DADO COMPARTILHADA É UM ARRAY
AS THREADS ESCRITORAS VAO ESCREVER EM UMA REGIÃO ALEATÓRIA

*/

#define TAM_ARRAY 100

//DECLARANDO VARIÁVEIS
int N;
int M;
int *base_dados;            //array que vai ser nossa base de dados
int writer_ativa = 0;      // variavel utilizada como controle da variavel de condição
int id_writer_ativa;      // checa espaço a thread está escrevendo (TAVEZ NAO PRECISE)

//INICIALIZAR MUTEX

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex para controle das threads

//INICIALIZAR VARIAVEL CONDICIONAL

pthread_cond_t writer_finished_condition = PTHREAD_COND_INITIALIZER; //variavel condicional
pthread_cond_t reader_finished_condition = PTHREAD_COND_INITIALIZER; //variavel condicional

//IMPLEMENTAR FUNÇÕES DE ROTINA DA THREAD

/*
Na read Function utilizamos um mutex antes de acessar a região crítica para checar com a varivel
condicional se a thread de escrita está ativa
ela espera
e roda as threads leitoras normalmente, podendo elas estar na região crítica no mesmo tempo
*/

void *read_function(void *id_read_thread){
    int read_id = (*(int *)id_read_thread);

    while (1) {
        pthread_mutex_lock(&mutex);     // trava o mutex

        // Espera enquanto uma escritora está ativa
        while (writer_ativa) {
            pthread_cond_wait(&writer_finished_condition, &mutex); // variavel condicional, checa se a thread escritora já finalizou
        }

        pthread_mutex_unlock(&mutex);

        int id = rand() % (TAM_ARRAY - 1); //ler o valor do endereço aleatório, dentro das posições do array

        printf("Thread leitora %d leu o valor %d na posicao %d\n", read_id, base_dados[id], id);

        sleep(2);

    }

}

//---------------------------------------------------------------------------------------

/*
Na write function a região crítica é acessada dentro do mutex, para evitar que outra thread acesse ela enquanto uma
está escrevendo
Utilizamos a variavel condicional para bloquear todas as threads enquanto a escritora estiver ativa.
esperamos ela finalizar, alterar o valor de write_ativa para 0 e avisa as demais threads
*/

void *write_function(void *id_write_thread){

    int write_id = (*(int *)id_write_thread); // nº da thread
    
    while (1) {
        int valor = rand() % 100; // valor a ser armazenado 
        int id = rand() % (TAM_ARRAY - 1); // posição aleatória do array

        pthread_mutex_lock(&mutex);

        // Bloqueia todas as threads enquanto uma escritora está ativa
        while (writer_ativa) {
            pthread_cond_wait(&writer_finished_condition, &mutex);
        }

        writer_ativa = 1;   // sinal da variavel condicional (1 = ativa)

        base_dados[id] = valor;     // modifica o valor na base de dados;
        printf("Thread escritora %d escreveu o valor %d na posicao %d\n", write_id, valor, id); // print o que a thread realizou

        writer_ativa = 0;       // modifica o sinal para 0 = thread conclui a escrita
        pthread_cond_broadcast(&writer_finished_condition); // Notifica as threads que a escrita foi finalizada
        pthread_mutex_unlock(&mutex);

        sleep(2);
    }

}

//--------------------------------------------------------------------------------------

int main(){

    //VARIAVEIS
    int i,j;

    //ENTRADAS

    printf("Digite o número de threads leitoras N: ");
    scanf("%d",&N);

    printf("Digite o número de threads escritoras M: ");
    scanf("%d",&M);

    //ALOCAR MEMÓRIA PARA O ARRAY (REGIÃO COMPARTILHADA)

    base_dados = (int *)malloc(TAM_ARRAY * sizeof(int));
    if (base_dados == NULL) {
        printf("Memória não alocada!\n");
    }

    //CRIAR THREADS

    pthread_t reader_threads[N];
    int id_read[N];
    pthread_t writer_threads[M];
    int id_write[M];

    //THREADS ESCRITORAS
    for(i=0;i<M;i++){
        id_write[i] = i+1;
        pthread_create(&writer_threads[i], NULL, write_function,(void *)&id_write[i]);
        
    }

    //THREADS LEITORAS
    
    for(i=0;i<N;i++){
        id_read[i] = i+1;
        pthread_create(&reader_threads[i], NULL, read_function,(void *)&id_read[i]);
        
    }

    //ENCERRAR THREADS

    for(j=0;j<N;j++){
        pthread_join(reader_threads[j],NULL);
        pthread_join(writer_threads[j],NULL);
    }

    //LIBERA A MEMÓRIA USADA PELO ARRAY DA MEMÓRIA CRITICA

    free(base_dados);

    return 0;
}