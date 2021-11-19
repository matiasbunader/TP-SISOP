#ifndef KERNEL_H
#define KERNEL_H

#include <commons/collections/queue.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/temporal.h>
#include <string.h>
#include <config.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <uuid/uuid.h>
#include "serializacion.h"
#include "utils.h"

typedef enum {
    NEW,
    READY,
    EXEC,
    BLOCKED,
    SUSPENDED_BLOCKED,
    SUSPENDED_READY,
    EXIT
} process_state;

typedef struct pcb_str {
    uuid_t pid;
    process_state state;
    int client_fd;
    char* blocked_at;
    char* unblocked_at;
    int ultima_estimacion;
    // semaforos tomados // PREGUNTAR
} pcb;

typedef struct t_conexiones_fds {
    int server_fd;
    int client_fd;
    int memoria_fd;
} conexiones_fds;

typedef struct mate_io_command {
    mate_io_resource key;
    char* msg;
} mate_io_command;

typedef struct mate_io_dispositivo {
    mate_io_resource key;
    int delay_ms;
    t_queue* queue;
    sem_t* sem_queue_instance;
    sem_t* sem_delay_instance;
} mate_io_dispositivo;

// double alpha_value = 0;

t_queue* cola_new;
t_list* cola_ready;

t_list* carpinchos;
t_list* dispositivos_io;

void kernel_init(void);

// semaforos
sem_t proceso_en_new;
sem_t proceso_en_ready;
sem_t multiprogramacion;

void server_init();
void planificadores_init();
void io_init();

#endif