#include "server.h"
#include "io.h"
#include "planificadores.h"
#include "semaforos.h"

void server_init(void) {
    conexiones_fds fds;
    int memoria_fd = conectar_con_memoria();

    char* ip = config_get_string_value(config_kernel, "IP_ESCUCHA");
    char* puerto = config_get_string_value(config_kernel, "PUERTO_ESCUCHA");

    int server_fd = iniciar_servidor(ip, puerto, log_kernel);

    fds.server_fd = server_fd;
    fds.memoria_fd = memoria_fd;

    // lanzar thread de aceptacion de clientes
    pthread_t aceptacion_de_clientes;
    pthread_create(&aceptacion_de_clientes, NULL, (void*)&aceptar_clientes, &fds);
    pthread_detach(aceptacion_de_clientes);
}

int conectar_con_memoria() {
    char* ip_memoria = config_get_string_value(config_kernel, "IP_MEMORIA");
    char* puerto_memoria = config_get_string_value(config_kernel, "PUERTO_MEMORIA");

    return crear_conexion(ip_memoria, puerto_memoria, log_kernel);
}

// inicio thread aceptacion de clientes
void* aceptar_clientes(conexiones_fds* fds) {
    while (1) {
        int client_fd = esperar_cliente(fds->server_fd, log_kernel);
        fds->client_fd = client_fd;
        // lanzar thread de escucha de mensajes por cliente
        pthread_t escucha_de_mensajes;
        pthread_create(&escucha_de_mensajes, NULL, (void*)&servir_cliente, fds);
        pthread_detach(escucha_de_mensajes);
    }
}

// inicio thread exclusivo para instancia de matelib
void* servir_cliente(conexiones_fds* fds) {
    pcb* baby_carpincho = pcb_init(fds->client_fd);
    queue_push(cola_new, baby_carpincho);
    sem_post(&proceso_en_new);

    atender_mensajes(*fds);
    return NULL;
}

pcb* pcb_init(int client_fd) {
    pcb* new_carpincho = malloc(sizeof(pcb));
    new_carpincho->client_fd = client_fd;
    uuid_generate(new_carpincho->pid);
    new_carpincho->state = NEW;
    new_carpincho->ultima_estimacion = 0;
    new_carpincho->blocked_at = "";
    new_carpincho->unblocked_at = "";

    list_add(carpinchos, new_carpincho);
    return new_carpincho;
}

void atender_mensajes(conexiones_fds fds) {
    while (1) {
        t_paquete* paquete = malloc(sizeof(t_paquete));
        paquete->buffer = malloc(sizeof(t_buffer));
        // Recibimos el codigo de operacion
        recv(fds.client_fd, &(paquete->codigo_operacion), sizeof(int32_t), MSG_WAITALL);
        // Recibimos el tamaño del buffer
        recv(fds.client_fd, &(paquete->buffer->size), sizeof(int32_t), MSG_WAITALL);
        paquete->buffer->stream = malloc(paquete->buffer->size);
        // Recibimos el contenido buffer
        recv(fds.client_fd, paquete->buffer->stream, paquete->buffer->size, MSG_WAITALL);

        process_request(paquete, fds);

        free(paquete->buffer->stream);
        free(paquete->buffer);
        free(paquete);
    }
}

void process_request(t_paquete* paquete, conexiones_fds fds) {
    pcb* carpincho = get_carpincho_by_fd(fds.client_fd);

    switch (paquete->codigo_operacion) {
        case CALL_IO: {
            log_info(log_kernel, "op: call io");
            // deserializar paquete
            mate_io_command* io_command = deserializar_io_command(paquete);
            printf("\nmensaje io[%s]: << %s >>", io_command->key, io_command->msg);

            manejar_io(io_command, carpincho);

            break;
        }
        case SEM_INIT: {
            mate_sem_t* received_mate_sem;
            log_info(log_kernel, "op: Inicializar semaforo");
            received_mate_sem = deserializar_mate_sem(paquete);

            // obtener por nombre el semaforo virtual, si ya existe lanzar error.
            // found_mate_sem = get_mate_sem_by_name(received_mate_sem->name);
            // if (found_mate_sem != NULL) throw_error(UNKNOWN, log_kernel);

            manejar_sem_init(received_mate_sem, carpincho);

            break;
        }
        case SEM_WAIT: {
            char* sem_name;
            mate_sem_t* found_mate_sem;

            log_info(log_kernel, "op: sem wait");
            sem_name = deserializar_mate_sem_name(paquete);

            // obtener por nombre el semaforo virtual, si no existe ???
            found_mate_sem = get_mate_sem_by_name(sem_name);
            // if (found_mate_sem == NULL) throw_error(UNKNOWN, log_kernel);  // ???

            manejar_sem_wait(found_mate_sem, carpincho);

            break;
        }
        case SEM_POST: {
            char* sem_name;
            mate_sem_t* found_mate_sem;

            log_info(log_kernel, "op: sem post");
            sem_name = deserializar_mate_sem_name(paquete);

            // obtener por nombre el semaforo virtual, si no existe ???
            found_mate_sem = get_mate_sem_by_name(sem_name);
            //if (found_mate_sem == NULL) throw_error(UNKNOWN, log_kernel);  // ???

            manejar_sem_post(found_mate_sem, carpincho);

            break;
        }
        case SEM_DESTROY: {
            char* sem_name;
            mate_sem_t* found_mate_sem;

            log_info(log_kernel, "op: sem destroy");
            sem_name = deserializar_mate_sem_name(paquete);

            // obtener por nombre el semaforo virtual, si no existe ???
            found_mate_sem = get_mate_sem_by_name(sem_name);
            //if (found_mate_sem == NULL) throw_error(UNKNOWN, log_kernel);  // ???

            manejar_sem_destroy(found_mate_sem, carpincho);

            break;
        }
        case MEM_ALLOC:
        case MEM_FREE:
        case MEM_READ:
        case MEM_WRITE: {
            // PASAMANOS
            size_t payload_size = paquete->buffer->size + 2 * sizeof(int32_t);
            void* payload = serializar_paquete(paquete, payload_size);
            send(fds.memoria_fd, payload, payload_size, 0);

            // TODO: check memory leaks
            free(payload);

            // recv()  espero respuesta de memoria
            int32_t res;
            recv(fds.memoria_fd, &res, sizeof(int32_t), MSG_WAITALL);
            send(fds.client_fd, (void*)res, sizeof(int32_t), 0);
        }
        case MATE_CLOSE:
            log_info(log_kernel, "Cerrando carpincho (%d)", carpincho->client_fd);
        default: {
            if (paquete->codigo_operacion != MATE_CLOSE) throw_error(CLIENT_DISCONNECTED, log_kernel, 0);
            close(carpincho->client_fd);
            pasar_a_exit(carpincho);
            pthread_exit(NULL);
        }
    }
}
