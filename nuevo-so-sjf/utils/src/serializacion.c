#include "serializacion.h"

void crear_buffer(t_paquete *paquete) {
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = 0;
    paquete->buffer->stream = NULL;
}

t_paquete *crear_paquete(op_code code) {
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = code;
    crear_buffer(paquete);
    return paquete;
}

void eliminar_paquete(t_paquete *paquete) {
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}

void *serializar_paquete(t_paquete *paquete, size_t bytes) {
    void *payload = malloc(bytes);
    int offset = 0;

    memcpy(payload + offset, &(paquete->codigo_operacion), sizeof(int32_t));
    offset += sizeof(int32_t);
    memcpy(payload + offset, &(paquete->buffer->size), sizeof(int32_t));
    offset += sizeof(int32_t);
    memcpy(payload + offset, paquete->buffer->stream, paquete->buffer->size);
    offset += paquete->buffer->size;

    return payload;
}

void serializar_mate_sem(mate_sem_t *mate_sem, t_buffer *mate_sem_buffer) {
    serializar_string(mate_sem->name, mate_sem_buffer);
    serializar_int(mate_sem->value, mate_sem_buffer);
}

void serializar_int(int32_t valor, t_buffer *b) {
    reservar_espacio(b, sizeof(int32_t));
    memcpy(((char *)b->stream) + b->size, &valor, sizeof(int32_t));
    b->size += sizeof(int32_t);
}

void serializar_string(char *str, t_buffer *b) {
    int str_size = strlen(str) + 1;
    serializar_int(str_size, b);
    reservar_espacio(b, str_size);
    memcpy(((char *)b->stream) + b->size, str, str_size);
    b->size += str_size;
}

void reservar_espacio(t_buffer *b, int bytes) {
    b->stream = realloc(b->stream, b->size + bytes);
}

void serializar_memwrite(t_buffer *buffer, void *origin, mate_pointer dest, int size) {
    buffer->size = sizeof(mate_pointer) + sizeof(int) + size;
    void *stream = malloc(buffer->size);
    int offset = 0;

    memcpy(stream + offset, (void *)dest, sizeof(int32_t));
    offset += sizeof(int32_t);
    memcpy(stream + offset, (void *)size, sizeof(int));
    offset += sizeof(int);
    memcpy(stream + offset, (void *)origin, size);

    buffer->stream = stream;
}

void serializar_memread(t_buffer *buffer, mate_pointer origin, int size) {
    serializar_int(origin, buffer);
    serializar_int(size, buffer);
}