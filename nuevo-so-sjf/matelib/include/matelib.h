#ifndef MATELIB_H
#define MATELIB_H

#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <netdb.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"

//-------------------Type Definitions----------------------/
typedef struct mate_inner_structure {
    char *ip;
    char *port;
    int backend_code;
    int backend_fd;
    void *memory;
    sem_t *sem_instance;
} mate_inner_structure;

typedef struct mate_instance {
    mate_inner_structure *group_info;
} mate_instance;

typedef char *mate_io_resource;

typedef char *mate_sem_name;

typedef int32_t mate_pointer;

typedef struct mate_sem_t {
    mate_sem_name name;
    int32_t value;
} mate_sem_t;

// TODO: Docstrings

//------------------General Functions---------------------/
int mate_init(mate_instance *lib_ref, char *config);

int mate_close(mate_instance *lib_ref);

//-----------------Semaphore Functions---------------------/
int mate_sem_init(mate_instance *lib_ref, mate_sem_name sem, unsigned int value);

int mate_sem_wait(mate_instance *lib_ref, mate_sem_name sem);

int mate_sem_post(mate_instance *lib_ref, mate_sem_name sem);

int mate_sem_destroy(mate_instance *lib_ref, mate_sem_name sem);

//--------------------IO Functions------------------------/

int mate_call_io(mate_instance *lib_ref, mate_io_resource io, void *msg);

//--------------Memory Module Functions-------------------/

mate_pointer mate_memalloc(mate_instance *lib_ref, int size);

int mate_memfree(mate_instance *lib_ref, mate_pointer addr);

int mate_memread(mate_instance *lib_ref, mate_pointer origin, void *dest, int size);

int mate_memwrite(mate_instance *lib_ref, void *origin, mate_pointer dest, int size);

#endif