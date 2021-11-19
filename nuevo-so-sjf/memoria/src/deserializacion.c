#include "deserializacion.h"

vmalloc_t* deserializar_vmalloc(t_paquete* paquete) {
    vmalloc_t* memalloc = malloc(sizeof(vmalloc_t));
    size_t name_length = 0;
    void* stream = paquete->buffer->stream;

    memcpy(&(name_length), stream, sizeof(int32_t));
    stream += sizeof(int32_t);
    
    memcpy(&(memalloc->value), stream, sizeof(int32_t));

    return memalloc;
}

