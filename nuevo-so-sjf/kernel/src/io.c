#include "io.h"
#include "planificadores.h"

void io_init(void) {
    char** io_keys_config = config_get_array_value(config_kernel, "DISPOSITIVOS_IO");
    char** io_delays_config = config_get_array_value(config_kernel, "DURACIONES_IO");

    dispositivos_io = list_create();

    // contar dispositivos io
    int dispositivos_count = 0;
    char** io_keys_ptr = io_keys_config;
    char** io_delays_ptr = io_delays_config;
    while (*io_keys_ptr != 0) {
        mate_io_dispositivo* parsed = malloc(sizeof(mate_io_dispositivo));
        size_t key_length = strlen(*io_keys_ptr);
        parsed->key = malloc(key_length);
        strcpy(parsed->key, *io_keys_ptr);
        parsed->delay_ms = atoi(*io_delays_ptr);

        list_add(dispositivos_io, parsed);
        dispositivos_count++;
        ++io_keys_ptr;
        ++io_delays_ptr;
    }

    // lanzar m threads io
    pthread_t mocked_io[dispositivos_count];
    for (int i = 0; i < dispositivos_count; i++) {
        mate_io_dispositivo* dispositivo = list_get(dispositivos_io, i);
        pthread_create(&mocked_io[i], NULL, (void*)&simular_io, (void*)dispositivo);
        pthread_detach(mocked_io[i]);
    }
}

void* simular_io(mate_io_dispositivo* io) {
    log_info(log_kernel, "\nSOY UNA IO ((%s))", io->key);

    io->queue = queue_create();

    sem_t sem_io_queue;
    io->sem_queue_instance = malloc(sizeof(sem_t));
    io->sem_queue_instance = &sem_io_queue;
    sem_init(&sem_io_queue, 0, 0);

    sem_t sem_io_delay;
    io->sem_delay_instance = malloc(sizeof(sem_t));
    io->sem_delay_instance = &sem_io_delay;
    sem_init(&sem_io_delay, 0, 1);

    while (1) {
        sem_wait(&sem_io_queue);
        sem_wait(&sem_io_delay);
        pthread_t mocked_delay;
        pthread_create(&mocked_delay, NULL, (void*) &simular_espera, io);
        pthread_detach(mocked_delay);
    }
}

void* simular_espera(mate_io_dispositivo* io) {
    pcb* carpincho;
    carpincho = queue_pop(io->queue);
    usleep(io->delay_ms * 1000);
    sem_post(io->sem_delay_instance);
    pasar_a_ready(carpincho);
    return NULL;
}

mate_io_command* deserializar_io_command(t_paquete* paquete) {
    mate_io_command* io_cmd = malloc(sizeof(mate_io_command));
    size_t key_length = 0;
    size_t msg_length = 0;
    void* stream = paquete->buffer->stream;

    memcpy(&(key_length), stream, sizeof(int32_t));
    stream += sizeof(int32_t);
    io_cmd->key = malloc(key_length);
    memcpy(io_cmd->key, stream, key_length);
    stream += key_length;

    memcpy(&(msg_length), stream, sizeof(int32_t));
    stream += sizeof(int32_t);
    io_cmd->msg = malloc(msg_length);
    memcpy(io_cmd->msg, stream, msg_length);
    stream += msg_length;

    return io_cmd;
}

mate_io_dispositivo* get_io_by_key(char* key) {
    int _matches_key(mate_io_dispositivo * io) {
        return strcmp(io->key, key) == 0;
    }

    return list_find(dispositivos_io, (void*)_matches_key);
}

void manejar_io(mate_io_command* io_command, pcb* carpincho) {
    pasar_a_blocked(carpincho);
    mate_io_dispositivo* io = get_io_by_key(io_command->key);
    queue_push(io->queue, carpincho);
    sem_post(io->sem_queue_instance);
}