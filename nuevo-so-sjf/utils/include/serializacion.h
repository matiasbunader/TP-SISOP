#ifndef SERIALIZACION_H
#define SERIALIZACION_H

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <matelib.h>

/*
 * Comandos posibles:
 * sem init
 * sem wait
 * call io
 * sem post
 * mem alloc
 * mem free
 * mem read
 * mem write
 */
typedef enum op_code {
    ACK,
    SEM_INIT,
    SEM_WAIT,
    SEM_DESTROY,
    CALL_IO,
    SEM_POST,
    MEM_ALLOC,
    MEM_FREE,
    MEM_READ,
    MEM_WRITE,
    MATE_CLOSE
} op_code;

typedef struct t_buffer {
    int size;
    void* stream;
} t_buffer;

typedef struct t_paquete {
    op_code codigo_operacion;
    t_buffer* buffer;
} t_paquete;

t_paquete* crear_paquete(op_code code);

void reservar_espacio(t_buffer *b, int bytes);
void serializar_string(char *str, t_buffer *b);
void serializar_int(int32_t valor, t_buffer *b);
void serializar_mate_sem(mate_sem_t *mate_sem, t_buffer *b);
void serializar_memwrite(t_buffer *buffer, void *origin, mate_pointer dest, int size);
void serializar_memread(t_buffer *buffer, mate_pointer origin, int size);
void *serializar_paquete(t_paquete *paquete, size_t bytes);

#endif