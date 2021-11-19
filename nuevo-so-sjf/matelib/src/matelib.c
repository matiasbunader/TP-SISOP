#include <matelib.h>
#include "serializacion.h"

#define LOG_FILE_PATH "./log/matelib.log"

// TODO: reemplazar placeholders por implementacion propia

//------------------General Functions---------------------/

int mate_init(mate_instance *lib_ref, char *config) {
    lib_ref->group_info = malloc(sizeof(mate_inner_structure));
    t_config *config_ref = config_create(config);

    t_log *log_matelib = log_create(LOG_FILE_PATH, "MATELIB", true, LOG_LEVEL_INFO);
    log_info(log_matelib, "Hello from Matelib! \n");

    char *backend = config_get_string_value(config_ref, "BACKEND");
    lib_ref->group_info->backend_code = obtener_backend_code(backend);

    if (lib_ref->group_info->backend_code == UNKNOWN)
        throw_error(UNKNOWN_BACKEND_ERROR, log_matelib, 1);

    char *backendIpConfigKey = string_new();
    char *backendPortConfigKey = string_new();
    string_append(&backendIpConfigKey, "IP_");
    string_append(&backendIpConfigKey, backend);

    string_append(&backendPortConfigKey, "PORT_");
    string_append(&backendPortConfigKey, backend);

    lib_ref->group_info->ip =
        config_get_string_value(config_ref, backendIpConfigKey);
    lib_ref->group_info->port =
        config_get_string_value(config_ref, backendPortConfigKey);

    if (!lib_ref->group_info->ip || !lib_ref->group_info->port)
        throw_error(CONFIG_ERROR, log_matelib, 1);

    // conectar con KERNEL || MEMORIA
    int conexion =
        crear_conexion(lib_ref->group_info->ip, lib_ref->group_info->port, log_matelib);

    lib_ref->group_info->backend_fd = conexion;

    int32_t res;
    recv(conexion, &res, sizeof(int32_t), MSG_WAITALL);  // espero señal de PROCEDA
    if (res != PROCEDA) return COMMUNICATION_ERROR;

    printf("EXECUTING!");
    return SUCCESS;
}

int mate_close(mate_instance *lib_ref) {
    // obtener conexion existente (a partir de lib_ref->group_info->socket)
    int backend_fd = lib_ref->group_info->backend_fd;
    // inicializar paquete de comando SEM INIT
    t_paquete *paquete = crear_paquete(MATE_CLOSE);

    // calcular tamaño de paquete serializado: op_code + stream_size + stream
    size_t payload_size = paquete->buffer->size + 2 * sizeof(int32_t);
    void *payload = serializar_paquete(paquete, payload_size);
    send(backend_fd, payload, payload_size, 0);

    // TODO: check memory leaks
    // free(payload);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);

    liberar_conexion(lib_ref->group_info->backend_fd);
    free(lib_ref->group_info);
    return 0;
}

//-----------------Semaphore Functions---------------------/

int mate_sem_init(mate_instance *lib_ref, mate_sem_name sem, unsigned int value) {
    // obtener conexion existente (a partir de lib_ref->group_info->socket)
    int backend_fd = lib_ref->group_info->backend_fd;

    // declarar semáforo virtual TAD
    mate_sem_t *mate_sem = malloc(sizeof(mate_sem_t));
    mate_sem->name = sem;
    mate_sem->value = value;

    // inicializar paquete de comando SEM INIT
    t_paquete *paquete = crear_paquete(SEM_INIT);

    // serializar data de semáforo virtual
    serializar_mate_sem(mate_sem, paquete->buffer);

    // calcular tamaño de paquete serializado: op_code + stream_size + stream
    size_t payload_size = paquete->buffer->size + 2 * sizeof(int32_t);
    void *payload = serializar_paquete(paquete, payload_size);
    send(backend_fd, payload, payload_size, 0);

    // TODO: check memory leaks
    // free(payload);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);

    int32_t res;
    recv(backend_fd, &res, sizeof(int32_t), MSG_WAITALL);  // espero señal de PROCEDA
    if (res != PROCEDA) printf("Error al iniciar semaforo");

    return res;
}

int mate_sem_wait(mate_instance *lib_ref, mate_sem_name sem_name) {
    // obtener conexion existente (a partir de lib_ref->group_info->socket)
    int backend_fd = lib_ref->group_info->backend_fd;

    // inicializar paquete de comando SEM INIT
    t_paquete *paquete = crear_paquete(SEM_WAIT);

    // serializar data de semáforo virtual
    serializar_string(sem_name, paquete->buffer);

    // calcular tamaño de paquete serializado: op_code + stream_size + stream
    size_t payload_size = paquete->buffer->size + 2 * sizeof(int32_t);
    void *payload = serializar_paquete(paquete, payload_size);
    send(backend_fd, payload, payload_size, 0);

    // TODO: check memory leaks
    free(payload);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);

    int32_t res;
    recv(backend_fd, &res, sizeof(int32_t), MSG_WAITALL);  // espero señal de PROCEDA
    if (res != PROCEDA) printf("Error al utilizar semaforo");

    return res;
}

int mate_sem_post(mate_instance *lib_ref, mate_sem_name sem_name) {
    // obtener conexion existente (a partir de lib_ref->group_info->socket)
    int backend_fd = lib_ref->group_info->backend_fd;

    // inicializar paquete de comando SEM INIT
    t_paquete *paquete = crear_paquete(SEM_POST);

    // serializar data de semáforo virtual
    serializar_string(sem_name, paquete->buffer);

    // calcular tamaño de paquete serializado: op_code + stream_size + stream
    size_t payload_size = paquete->buffer->size + 2 * sizeof(int32_t);
    void *payload = serializar_paquete(paquete, payload_size);
    send(backend_fd, payload, payload_size, 0);

    // TODO: check memory leaks
    free(payload);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);

    int32_t res;
    recv(backend_fd, &res, sizeof(int32_t), MSG_WAITALL);  // espero señal de PROCEDA
    if (res != PROCEDA) printf("Error al utilizar semaforo");

    return res;
}

int mate_sem_destroy(mate_instance *lib_ref, mate_sem_name sem_name) {
    // obtener conexion existente (a partir de lib_ref->group_info->socket)
    int backend_fd = lib_ref->group_info->backend_fd;

    // inicializar paquete de comando SEM INIT
    t_paquete *paquete = crear_paquete(SEM_DESTROY);

    // serializar data de semáforo virtual
    serializar_string(sem_name, paquete->buffer);

    // calcular tamaño de paquete serializado: op_code + stream_size + stream
    size_t payload_size = paquete->buffer->size + 2 * sizeof(int32_t);
    void *payload = serializar_paquete(paquete, payload_size);
    send(backend_fd, payload, payload_size, 0);

    // TODO: check memory leaks
    free(payload);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);

    int32_t res;
    recv(backend_fd, &res, sizeof(int32_t), MSG_WAITALL);  // espero señal de PROCEDA
    if (res != PROCEDA) printf("Error al destruir semaforo");

    return res;
}

//--------------------IO Functions------------------------/

int mate_call_io(mate_instance *lib_ref, mate_io_resource io, void *msg) {
    // obtener conexion existente (a partir de lib_ref->group_info->socket)
    int backend_fd = lib_ref->group_info->backend_fd;

    // inicializar paquete de comando SEM INIT
    t_paquete *paquete = crear_paquete(CALL_IO);

    // serializar data de semáforo virtual
    serializar_string(io, paquete->buffer);
    serializar_string(msg, paquete->buffer);

    // calcular tamaño de paquete serializado: op_code + stream_size + stream
    size_t payload_size = paquete->buffer->size + 2 * sizeof(int32_t);
    void *payload = serializar_paquete(paquete, payload_size);
    send(backend_fd, payload, payload_size, 0);

    // TODO: check memory leaks
    // free(payload);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);

    int32_t res;
    recv(backend_fd, &res, sizeof(int32_t), MSG_WAITALL);  // espero señal de PROCEDA
    if (res != PROCEDA) printf("Error al utilizar io");

    return res;
}

//--------------Memory Module Functions-------------------/

mate_pointer mate_memalloc(mate_instance *lib_ref, int size) {
    // obtener conexion existente (a partir de lib_ref->group_info->socket)
    int backend_fd = lib_ref->group_info->backend_fd;

    t_paquete *paquete = crear_paquete(MEM_ALLOC);
    serializar_int(size, paquete->buffer);

    // calcular tamaño de paquete serializado: op_code + stream_size + stream
    size_t payload_size = paquete->buffer->size + 2 * sizeof(int32_t);
    void *payload = serializar_paquete(paquete, payload_size);
    send(backend_fd, payload, payload_size, 0);

    // TODO: check memory leaks
    free(payload);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);

    // recv()  espero respuesta de backend
    int32_t res;
    recv(backend_fd, &res, sizeof(int32_t), MSG_WAITALL);

    return res;
}

int mate_memfree(mate_instance *lib_ref, mate_pointer addr) {
    // obtener conexion existente (a partir de lib_ref->group_info->socket)
    int backend_fd = lib_ref->group_info->backend_fd;

    t_paquete *paquete = crear_paquete(MEM_FREE);
    serializar_int(addr, paquete->buffer);

    // calcular tamaño de paquete serializado: op_code + stream_size + stream
    size_t payload_size = paquete->buffer->size + 2 * sizeof(int32_t);
    void *payload = serializar_paquete(paquete, payload_size);
    send(backend_fd, payload, payload_size, 0);

    // TODO: check memory leaks
    free(payload);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);

    // recv()  espero respuesta de backend
    int32_t res;
    recv(backend_fd, &res, sizeof(int32_t), MSG_WAITALL);

    return res;
}

//mate_pointer origin es de dónde tenés que leer
int mate_memread(mate_instance *lib_ref, mate_pointer origin, void *dest, int size) {
    // obtener conexion existente (a partir de lib_ref->group_info->socket)
    int backend_fd = lib_ref->group_info->backend_fd;

    t_paquete *paquete = crear_paquete(MEM_READ);
    serializar_memread(paquete->buffer, origin, size);

    // calcular tamaño de paquete serializado: op_code + stream_size + stream
    size_t payload_size = paquete->buffer->size + 2 * sizeof(int32_t);
    void *payload = serializar_paquete(paquete, payload_size);
    send(backend_fd, payload, payload_size, 0);

    // TODO: check memory leaks
    free(payload);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);

    // recv()  espero respuesta de backend
    int content_size;
    recv(backend_fd, &content_size, sizeof(int), 0);

    //leer la respuesta de memoria y guardarla en el destino
    void *content = malloc(content_size);
    recv(backend_fd, content, content_size, 0);

    memcpy(dest, content, content_size);

    // recv()  espero respuesta de backend
    int32_t res;
    recv(backend_fd, &res, sizeof(int32_t), MSG_WAITALL);

    return res;
}

int mate_memwrite(mate_instance *lib_ref, void *origin, mate_pointer dest, int size) {
    // obtener conexion existente (a partir de lib_ref->group_info->socket)
    int backend_fd = lib_ref->group_info->backend_fd;

    t_paquete *paquete = crear_paquete(MEM_WRITE);
    serializar_memwrite(paquete->buffer, origin, dest, size);

    // calcular tamaño de paquete serializado: op_code + stream_size + stream
    size_t payload_size = paquete->buffer->size + 2 * sizeof(int32_t);
    void *payload = serializar_paquete(paquete, payload_size);
    send(backend_fd, payload, payload_size, 0);

    // TODO: check memory leaks
    free(payload);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);

    // recv()  espero respuesta de backend
    int32_t res;
    recv(backend_fd, &res, sizeof(int32_t), MSG_WAITALL);

    return res;
}
