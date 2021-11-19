#include "semaforos.h"
#include "planificadores.h"

static void mate_sem_free(mate_sem_t* s) {
    // TODO check memory leaks
    queue_destroy(s->queue);
    free(s->name);
    free(s);
}

mate_sem_t* deserializar_mate_sem(t_paquete* paquete) {
    mate_sem_t* sem = malloc(sizeof(mate_sem_t));
    size_t name_length = 0;
    void* stream = paquete->buffer->stream;

    memcpy(&(name_length), stream, sizeof(int32_t));
    stream += sizeof(int32_t);
    sem->name = malloc(name_length);
    memcpy(sem->name, stream, name_length);
    stream += name_length;
    memcpy(&(sem->value), stream, sizeof(int32_t));

    return sem;
}

char* deserializar_mate_sem_name(t_paquete* paquete) {
    char* sem_name = malloc(sizeof(char));
    size_t name_length = 0;
    void* stream = paquete->buffer->stream;

    memcpy(&(name_length), stream, sizeof(int32_t));
    stream += sizeof(int32_t);
    sem_name = malloc(name_length);
    memcpy(sem_name, stream, name_length);

    return sem_name;
}

mate_sem_t* get_mate_sem_by_name(char* sem_name) {
    int _mate_sem_name_matches(mate_sem_t * vs) {
        return strcmp(vs->name, sem_name) == 0;
    }

    return list_find(mate_sem_list, (void*)_mate_sem_name_matches);
}

void manejar_sem_init(mate_sem_t* mate_sem, pcb* carpincho) {
    mate_sem->queue = queue_create();
    list_add(mate_sem_list, mate_sem);
    pasar_a_ready(carpincho);
}

void manejar_sem_wait(mate_sem_t* mate_sem, pcb* carpincho) {
    mate_sem->value--;

    if (mate_sem->value < 0) {
        pasar_a_blocked(carpincho);
        queue_push(mate_sem->queue, carpincho);
    } else {
        pasar_a_ready(carpincho);
    }
}

void manejar_sem_post(mate_sem_t* mate_sem, pcb* carpincho) {
    mate_sem->value++;

    if (!queue_is_empty(mate_sem->queue)) {
        pcb* carpincho_en_espera = queue_pop(mate_sem->queue);
        pasar_a_ready(carpincho_en_espera);
    }
    pasar_a_ready(carpincho);
}

void manejar_sem_destroy(mate_sem_t* mate_sem, pcb* carpincho) {
    if (!queue_is_empty(mate_sem->queue)) {
        // PREGUNTAR
        // what to do?
        // pcb* carpincho_en_espera = queue_pop(mate_sem->queue);
        // pasar_a_ready(carpincho_en_espera);
    }

    int _mate_sem_name_matches(mate_sem_t * vs) {
        return strcmp(vs->name, mate_sem->name) == 0;
    }

    list_remove_and_destroy_by_condition(mate_sem_list, (void*)_mate_sem_name_matches, (void*)mate_sem_free);

    pasar_a_ready(carpincho);
}
