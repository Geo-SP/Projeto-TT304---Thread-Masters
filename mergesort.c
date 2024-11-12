#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#define MAX_INT 2147483647

struct ThreadData {
    int *data;
    int esquerda;
    int direita;
    int thread_id;
    double tempo_execucao;
};

int num_threads;
pthread_mutex_t mutex_thread = PTHREAD_MUTEX_INITIALIZER;
double *thread_tempo_execucao;

void merge(int *data, int esquerda, int meio, int direita) {
    int n1 = meio - esquerda + 1;
    int n2 = direita - meio;
    int *E = malloc(sizeof(int) * n1);
    int *D = malloc(sizeof(int) * n2);

    if (E == NULL || D == NULL) {
        fprintf(stderr, "Erro ao alocar memória para mesclagem.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n1; i++)
        E[i] = data[esquerda + i];
    for (int j = 0; j < n2; j++)
        D[j] = data[meio + 1 + j];

    int i = 0, j = 0, k = esquerda;
    while (i < n1 && j < n2) {
        if (E[i] <= D[j]) {
            data[k++] = E[i++];
        } else {
            data[k++] = D[j++];
        }
    }

    while (i < n1) {
        data[k++] = E[i++];
    }

    while (j < n2) {
        data[k++] = D[j++];
    }

    free(E);
    free(D);
}

void merge_sort(int *data, int esquerda, int direita) {
    if (esquerda < direita) {
        int meio = esquerda + (direita - esquerda) / 2;
        merge_sort(data, esquerda, meio);
        merge_sort(data, meio + 1, direita);
        merge(data, esquerda, meio, direita);
    }
}

void *thread_sort(void *arg) {
    struct ThreadData *t_data = (struct ThreadData *)arg;
    struct timespec inicio, fim;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    merge_sort(t_data->data, t_data->esquerda, t_data->direita);

    clock_gettime(CLOCK_MONOTONIC, &fim);
    t_data->tempo_execucao = (fim.tv_sec - inicio.tv_sec);
    t_data->tempo_execucao += (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    pthread_mutex_lock(&mutex_thread);
    thread_tempo_execucao[t_data->thread_id] = t_data->tempo_execucao;
    pthread_mutex_unlock(&mutex_thread);

    pthread_exit(NULL);
}

void ler_arquivos(int **data, int *tamanho_total, int argc, char *argv[], int index_inicial) {
    int *all_data = NULL;
    int total = 0;

    for (int i = index_inicial; i < argc - 2; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            fprintf(stderr, "Erro ao abrir o arquivo %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }

        int num;
        int tamanho = 0;
        int capacidade = 1000;
        int *dados_do_arquivo = malloc(sizeof(int) * capacidade);
        if (dados_do_arquivo == NULL) {
            fprintf(stderr, "Erro ao alocar memória para leitura do arquivo.\n");
            exit(EXIT_FAILURE);
        }

        while (fscanf(fp, "%d", &num) != EOF) {
            if (tamanho >= capacidade) {
                capacidade *= 2;
                dados_do_arquivo = realloc(dados_do_arquivo, sizeof(int) * capacidade);
                if (dados_do_arquivo == NULL) {
                    fprintf(stderr, "Erro ao realocar memória para leitura do arquivo.\n");
                    exit(EXIT_FAILURE);
                }
            }
            dados_do_arquivo[tamanho++] = num;
        }
        fclose(fp);

        all_data = realloc(all_data, sizeof(int) * (total + tamanho));
        if (all_data == NULL) {
            fprintf(stderr, "Erro ao realocar memória para todos os dados.\n");
            exit(EXIT_FAILURE);
        }
        memcpy(all_data + total, dados_do_arquivo, sizeof(int) * tamanho);
        total += tamanho;
        free(dados_do_arquivo);
    }

    *data = all_data;
    *tamanho_total = total;
}

int main (int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Uso incorreto. Exemplo:\n");
        fprintf(stderr, "./mergesort 4 arq1.dat arq2.dat arq3.dat -o saida.dat\n");
        exit(EXIT_FAILURE);
    }

    num_threads = atoi(argv[1]);
    if (num_threads != 1 && num_threads != 2 && num_threads != 4 && num_threads != 8) {
        fprintf(stderr, "Número de threads deve ser 1, 2, 4 ou 8.\n");
        exit(EXIT_FAILURE);
    }

    char *arquivo_de_saida = NULL;
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            arquivo_de_saida = argv[i + 1];
            break;
        }
    }

    if (arquivo_de_saida == NULL) {
        fprintf(stderr, "Arquivo de saída não especificado.\n");
        exit(EXIT_FAILURE);
    }

    int *data = NULL;
    int tamanho_total = 0;

    ler_arquivos(&data, &tamanho_total, argc, argv, 2);

    pthread_mutex_init(&mutex_thread, NULL);

    thread_tempo_execucao = malloc(sizeof(double) * num_threads);
    if (thread_tempo_execucao == NULL) {
        fprintf(stderr, "Erro ao alocar memória para tempos de execução das threads.\n");
        exit(EXIT_FAILURE);
    }

    pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);
    struct ThreadData *thread_data_array = malloc(sizeof(struct ThreadData) * num_threads);
    int segment_size = (tamanho_total + num_threads - 1) / num_threads;

    for (int i = 0; i < num_threads; i++) {
        thread_data_array[i].data = data;
        thread_data_array[i].esquerda = i * segment_size;
        thread_data_array[i].direita = (i + 1) * segment_size - 1;
        if (thread_data_array[i].direita >= tamanho_total) {
            thread_data_array[i].direita = tamanho_total - 1;
        }
        thread_data_array[i].thread_id = i;
        thread_data_array[i].tempo_execucao = 0;

        if (pthread_create(&threads[i], NULL, thread_sort, &thread_data_array[i]) != 0) {
            fprintf(stderr, "Erro ao criar thread %d.\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    int current_segments = num_threads;
    int *indices = malloc(sizeof(int) * current_segments);
    if (indices == NULL) {
        fprintf(stderr, "Erro ao alocar memória para índices de mesclagem.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < current_segments; i++) {
        indices[i] = thread_data_array[i].esquerda;
    }

    int *dados_ordenados = malloc(sizeof(int) * tamanho_total);
    if (dados_ordenados == NULL) {
        fprintf(stderr, "Erro ao alocar memória para dados ordenados.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < tamanho_total; i++) {
        int valor_minimo = MAX_INT;
        int segmento_min = -1;
        for (int j = 0; j < current_segments; j++) {
            if (indices[j] <= thread_data_array[j].direita) {
                if (data[indices[j]] < valor_minimo) {
                    valor_minimo = data[indices[j]];
                    segmento_min = j;
                }
            }
        }
        if (segmento_min != -1) {
            dados_ordenados[i] = data[indices[segmento_min]];
            indices[segmento_min]++;
        } else {
            fprintf(stderr, "Erro durante a mesclagem dos segmentos.\n");
            exit(EXIT_FAILURE);
        }
    }

    memcpy(data, dados_ordenados, sizeof(int) * tamanho_total);

    free(dados_ordenados);
    free(indices);

    double soma_thread_exec_time = 0.0;
    for (int i = 0; i < num_threads; i++) {
        printf("Tempo de execução da Thread %d: %f segundos.\n", i, thread_tempo_execucao[i]);
        soma_thread_exec_time += thread_tempo_execucao[i];
    }
    printf("Tempo total de execução das threads: %f segundos.\n", soma_thread_exec_time);

    FILE *fp_out = fopen(arquivo_de_saida, "w");
    if (fp_out == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo de saída %s\n", arquivo_de_saida);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < tamanho_total; i++) {
        fprintf(fp_out, "%d\n", data[i]);
    }
    fclose(fp_out);

    free(data);
    free(thread_tempo_execucao);
    free(threads);
    free(thread_data_array);
    pthread_mutex_destroy(&mutex_thread);

    return 0;
}
