#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>

/*
 * Macro que retorna o maior parametro.
 */
#define MAX(a, b) (((a) > (b))? (a): (b))

/*
 * Struct ilustrativa
 */
typedef struct _task_t {
    int id;
    int length;
    int due;
    int weight;
} task_t;

/*
 * Lê as  tarefas de +stdin+, retorna count e sets de +tasks_ptr+.
 *
 * task_ptr - Ponteiro que retorna o array.
 */
int read_tasks(task_t **tasks_ptr) {
    task_t *tasks, t;
    int i, limit;

    tasks = NULL;
    limit = INT_MAX;
    i = 0;
    while (limit-- && scanf("%d %d %d", &t.length, &t.due, &t.weight) != EOF) {
        tasks = realloc(tasks, ++i * sizeof(task_t));
        t.id = i;
        tasks[i-1] = t;
    }

    *tasks_ptr = tasks;
    return i;
}

/*
 * Imprime a lista de tarefas e as melhores permutacoes para +stdout+.
 *
 * n     - Numero de tarefas.
 * tasks - Array de tarefas.
 * best  - Array com as melhores permutacoes.
 */
void print_tasks(int n, task_t *tasks, task_t *best) {
    int i;
    task_t t;

    printf(" Nr | p_j | d_j | w_j \n");
    printf("----+-----+-----+-----\n");
    for (i = 0; i < n; i++) {
        t = tasks[i];
        printf("%2d. |%4d |%4d |%4d \n", t.id, t.length, t.due, t.weight);
    }

    printf("\nBest solution is: ");
    for (i = 0; i < n; i++) printf("%d ", best[i].id);
    printf("\n");
}

/*
 * Computa o valor da funcao desejada.
 *
 * n        - Numero de tarefas.
 * solution - Array de tarefas.
 */
int target(task_t *solution, int n) {
    register int sum, i, time, tmp;

    for (i = 0, time = 0, sum = 0; i < n; i++) {
        time += solution[i].length;
        /* Avalia a expressao uma vez */
        tmp = time - solution[i].due;
        sum += solution[i].weight * MAX(0, tmp);
    }

    return sum;
}

/*
 * Segundo procedimento de eliminação.
 *
 * array    - Array de tarefas contendo permutações parciais.
 * n        - Numero de elementos.
 * original - Valor do +target+ funcao pa +array+.
 */
char swap_skip(task_t *array, int n, int original) {
    task_t *copy, tmp;
    char result;
    int i;

    /* Creating defensive copy */
    copy = malloc(n * sizeof(task_t));
    memcpy(copy, array, n*sizeof(task_t));

    result = 0;
    for (i = n - 1; i > 0; i--) {
        /* Trocando valores */
        tmp = copy[i];
        copy[i] = copy[i - 1];
        copy[i - 1] = tmp;

        if (target(copy, n) < original) {
            result = 1;
            break;
        }
    }

    free(copy);
    return result;
}

/*
 * Terceiro procedimento de eliminacao.
 *
 * array    - Array de tarefas contendo permutacoes parciais.
 * n        - Numero de elementos.
 * original - Valor do +target+ funcao para +array+.
 */
char length_skip(task_t *array, int n, int original) {
    task_t *copy, tmp;
    char result;
    int i;

    /* Criando copia defensiva */
    copy = malloc(n * sizeof(task_t));
    memcpy(copy, array, n * sizeof(task_t));

    result = 0;
    for (i = 0; i < n; i++) {
        if (array[i].length > array[n-1].length) {
            tmp = copy[i];
            copy[i] = copy[n-1];
            copy[n-1] = tmp;

            if (target(copy, n) < original) {
                result = 1;
                break;
            }
        }
    }

    free(copy);
    return result;
}

/*
 * Gera um valor aleatorio inicial.
 *
 * n     - Numero de elementos.
 * tasks - Array de tarefas.
 */
task_t *generate_initial(int n, task_t *tasks) {
    task_t *initial, *best, *ptr;
    int *fill, i, r, iterations, best_result;
    char *used;

    fill = malloc(n * sizeof(int));
    used = malloc(n * sizeof(char));
    iterations = n*n;
    best = NULL;
    initial = NULL;

    srand(time(NULL));
    while (iterations--) {
        memset(used, 0, n*sizeof(char));
        for (i = 0; i < n; i++) {
            r = rand() % n;
            if (used[r] == 0) {
                fill[i] = r;
                used[r] = 1;
            } else {
                /* Valor usado, gerando novamente */
                i--;
            }
        }

        initial = malloc(n * sizeof(task_t));
        for (i = 0; i < n; i++) initial[i] = tasks[fill[i]];

        if (best == NULL || best_result > target(initial, n)) {
            ptr = best;
            best = initial;
            initial = ptr;
            best_result = target(best, n);
        }
        free(initial);
    }

    free(used);
    free(fill);
    return best;
}

/*
 * FUncao recursiva que gera permutacoes.
 *
 * n           - Numero de elementos.
 * tasks       - Array de tarefas.
 * fill        - Array que armazena as permutacoes.
 * index       - Local onde a permutacao esta no array.
 * used        - Array contendo informacoes, quando elas sao usadas ou nao.
 * best        - Array contendo melhor resultado.
 * best_result - Valor do +target+ funcao para +best+ array.
 */
void permute(int n, task_t *tasks, int *fill, int index,
             char *used, task_t **best, int *best_result) {
    int i, j, array_result;
    char cont;
    task_t *array, *ptr;

    for (i = 0; i < n; i++) {
        if (used[i] == 0) {
            fill[index] = i;

            /* Remontando array de tarefas */
            array = malloc((index + 1) * sizeof(task_t));
            for (j = 0; j < index + 1; j++) array[j] = tasks[fill[j]];

            if (index < n-1) {
                /* Substiruir ponto para eliminacao */
                cont = 1;

                /* Computando esse valor uma vez, sera usado varias vezes */
                array_result = target(array, index + 1);

                /* Pula se a permutacao incompleta e pior que a melhor */
                if (array_result > *best_result) cont = 0;

                /* Pula se existe uma solucao melhor em um subconjunto permutado*/
                if (cont && swap_skip(array, index + 1, array_result)) cont = 0;

                /* Pula se existe uma solucao melhor depois de trocar a maior
                 * tarefa com a ultima tarefa */
                if (cont && length_skip(array, index + 1, array_result)) cont = 0;

                if (cont) {
                    used[i] = 1;
                    permute(n, tasks, fill, index + 1, used, best, best_result);
                    used[i] = 0;
                }
            } else {
                /* Terminamos a permutacao, chamando a funcao desejada */
                if (target(array, n) < *best_result) {
                    ptr = *best;
                    *best = array;
                    array = ptr;

                    /* Pegando funcao desejada para melhor resultado */
                    *best_result = target(*best, n);
                }
            }
            free(array);
        }
    }
}

/*
 * Pepara buffers e chama a funcao +permute+.
 *
 * n     - Numero de elementos.
 * tasks - Array de tarefas.
 */
task_t *compute(int n, task_t *tasks) {
    task_t *best;
    int best_result, *fill;
    char *used;

    /* Criando buffers de ajuda necessarios */
    used = malloc(n * sizeof(char));
    memset(used, 0, n * sizeof(char));
    fill = malloc(n * sizeof(int));

    /* Gerando resultados iniciais */
    best = generate_initial(n, tasks);
    best_result = target(best, n);

    /* Computacao acontece aqui */
    permute(n, tasks, fill, 0, used, &best, &best_result);

    /*  Liberando buffers temporarios */
    free(fill);
    free(used);

    return best;
}

int main(void) {
    task_t *tasks, *best;
    int n;

    n = read_tasks(&tasks);
    best = compute(n, tasks);
    print_tasks(n, tasks, best);

    free(tasks);
    free(best);

    return 0;
}