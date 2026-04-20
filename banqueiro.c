#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

pthread_mutex_t lock;

int request_resources(int customer_num, int request[]);
int release_resources(int customer_num, int release[]);
bool is_safe();
void* customer_thread(void* customer_id);

int main(int argc, char *argv[]) {
    if (argc != NUMBER_OF_RESOURCES + 1) {
        printf("Uso: %s <rec1> <rec2> <rec3>\n", argv[0]);
        return -1;
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] = atoi(argv[i + 1]);
    }

    pthread_mutex_init(&lock, NULL);
    srand(time(NULL));

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            maximum[i][j] = (rand() % (available[j] + 1)); 
            allocation[i][j] = 0;
            need[i][j] = maximum[i][j];
        }
    }

    pthread_t threads[NUMBER_OF_CUSTOMERS];
    int customer_ids[NUMBER_OF_CUSTOMERS];

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        customer_ids[i] = i;
        pthread_create(&threads[i], NULL, customer_thread, &customer_ids[i]);
    }

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    return 0;
}

bool is_safe() {
    int work[NUMBER_OF_RESOURCES];
    bool finish[NUMBER_OF_CUSTOMERS] = {false};

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        work[i] = available[i];
    }

    int count = 0;
    while (count < NUMBER_OF_CUSTOMERS) {
        bool found = false;
        for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
            if (!finish[i]) {
                bool can_allocate = true;
                for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                    if (need[i][j] > work[j]) {
                        can_allocate = false;
                        break;
                    }
                }

                if (can_allocate) {
                    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                        work[j] += allocation[i][j];
                    }
                    finish[i] = true;
                    found = true;
                    count++;
                }
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

int request_resources(int customer_num, int request[]) {
    pthread_mutex_lock(&lock);

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (request[i] > need[customer_num][i]) {
            pthread_mutex_unlock(&lock);
            return -1;
        }
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (request[i] > available[i]) {
            pthread_mutex_unlock(&lock);
            return -1;
        }
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] -= request[i];
        allocation[customer_num][i] += request[i];
        need[customer_num][i] -= request[i];
    }

    if (is_safe()) {
        printf("Cliente %d: Pedido APROVADO.\n", customer_num);
        pthread_mutex_unlock(&lock);
        return 0;
    } else {
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            available[i] += request[i];
            allocation[customer_num][i] -= request[i];
            need[customer_num][i] += request[i];
        }
        printf("Cliente %d: Pedido NEGADO (Estado Inseguro).\n", customer_num);
        pthread_mutex_unlock(&lock);
        return -1;
    }
}

int release_resources(int customer_num, int release[]) {
    pthread_mutex_lock(&lock);

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (release[i] > allocation[customer_num][i]) {
            release[i] = allocation[customer_num][i];
        }
        allocation[customer_num][i] -= release[i];
        need[customer_num][i] += release[i];
        available[i] += release[i];
    }
    
    printf("Cliente %d: Recursos LIBERADOS.\n", customer_num);
    pthread_mutex_unlock(&lock);
    return 0;
}

void* customer_thread(void* arg) {
    int customer_num = *(int*)arg;
    int request[NUMBER_OF_RESOURCES];
    int release[NUMBER_OF_RESOURCES];

    while (1) {
        sleep(rand() % 3 + 1);

        bool has_need = false;
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            if (need[customer_num][i] > 0) {
                request[i] = rand() % (need[customer_num][i] + 1);
                has_need = true;
            } else {
                request[i] = 0;
            }
        }

        if (has_need) {
            request_resources(customer_num, request);
        }

        sleep(rand() % 2 + 1);

        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            if (allocation[customer_num][i] > 0) {
                release[i] = rand() % (allocation[customer_num][i] + 1);
            } else {
                release[i] = 0;
            }
        }
        
        release_resources(customer_num, release);
    }
    return NULL;
}
