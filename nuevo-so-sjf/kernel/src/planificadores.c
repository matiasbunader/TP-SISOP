#include "planificadores.h"

#include "semaforos.h"

static void pcb_destroy(pcb* c) {
    // REVISAR: "dobule free or corruption"
    // free(c->pid);
    // free(c->blocked_at);
    // free(c->unblocked_at);
    // free(c);
}

void planificadores_init(void) {
    int grado_multiprocesamiento = config_get_int_value(config_kernel, "GRADO_MULTIPROCESAMIENTO");
    int grado_multiprogramacion = config_get_int_value(config_kernel, "GRADO_MULTIPROGRAMACION");

    mate_sem_list = list_create();
    carpinchos = list_create();

    cola_new = queue_create();
    cola_ready = list_create();

    sem_init(&multiprogramacion, 0, grado_multiprogramacion);
    sem_init(&proceso_en_new, 0, 0);
    sem_init(&proceso_en_ready, 0, 0);

    lanzar_planificador_largo_plazo();
    lanzar_planificador_corto_plazo(grado_multiprocesamiento);
}

// lanzar 1 thread planificador de largo plazo
void lanzar_planificador_largo_plazo(void) {
    pthread_t planificador_largo_plazo;
    pthread_create(&planificador_largo_plazo, NULL, (void*)&manejar_multiprogramacion, NULL);
    pthread_detach(planificador_largo_plazo);
}

// lanzar n threads exec
void lanzar_planificador_corto_plazo(int grado_multiprocesamiento) {
    int i;
    pthread_t procesadores[grado_multiprocesamiento];
    for (i = 0; i < grado_multiprocesamiento; i++) {
        pthread_create(procesadores + i, NULL, (void*)&ejecutar, i);
        pthread_detach(procesadores[i]);
    }
}

// inicio thread planificador largo plazo
void* manejar_multiprogramacion(void) {
    while (1) {
        log_info(log_kernel, "Esperando proceso en new");
        sem_wait(&proceso_en_new);

        log_info(log_kernel, "Chequeo grado de multiprogramacion");
        sem_wait(&multiprogramacion);  // consumer de grado de multiprogramacion

        pcb* carpincho = malloc(sizeof(pcb));
        // PREGUNTAR
        // chequeo de 3 condiciones
        // Que haya carpinchos en BLOCKED
        // Que no haya carpinchos en READY
        // Que haya carpinchos en NEW

        // if (!queue_is_empty(cola_suspended_ready)) carpincho = queue_pop(cola_suspended_ready);
        carpincho = queue_pop(cola_new);  // else
        list_add(cola_ready, carpincho);
        sem_post(&proceso_en_ready);
    }
}

// inicio thread planificador corto plazo
void* ejecutar(int i) {
    while (1) {
        log_info(log_kernel, "Esperando proceso en ready (%d)", i);
        sem_wait(&proceso_en_ready);
        log_info(log_kernel, "EJECUTANDO! (%d)", i);

        // PREGUNTAR
        // TODO: obtener carpincho segun algoritmo de corto plazo
        pcb* carpincho = pick_next_carpincho_to_exec();

        pasar_a_exec(carpincho);

        log_info(log_kernel, "Asignado procesador %d a carpincho en socket %d", i, carpincho->client_fd);
    }
}

void pasar_a_ready(pcb* carpincho) {
    carpincho->state = READY;
    list_add(cola_ready, carpincho);
    sem_post(&proceso_en_ready);
}

void pasar_a_exec(pcb* carpincho) {
    carpincho->state = EXEC;
    int32_t* ack_buf;
    ack_buf = malloc(sizeof(int32_t));
    *(ack_buf) = PROCEDA;
    send(carpincho->client_fd, ack_buf, sizeof(int32_t), 0);

    // store unblocked timestamp
    carpincho->unblocked_at = temporal_get_string_time("hour:min:sec:milisec -- %H:%M:%S:%MS\n");
}

void pasar_a_blocked(pcb* carpincho) {
    carpincho->state = BLOCKED;
    // store blocked timestamp
    carpincho->blocked_at = temporal_get_string_time("hour:min:sec:milisec -- %H:%M:%S:%MS\n");
}

void pasar_a_exit(pcb* carpincho) {
    int _fd_matches(pcb * c) {
        return c->client_fd == carpincho->client_fd;
    }

    list_remove_and_destroy_by_condition(carpinchos, (void*)_fd_matches, (void*)pcb_destroy);

    sem_post(&multiprogramacion);
}

pcb* get_carpincho_by_fd(int client_fd) {
    int _fd_matches(pcb * carpincho) {
        return carpincho->client_fd == client_fd;
    }
    return list_find(carpinchos, (void*)_fd_matches);
}

pcb* pick_next_carpincho_to_exec() {
    char* algoritmo_planificacion = config_get_string_value(config_kernel, "ALGORITMO_PLANIFICACION");

    if (!strcmp(algoritmo_planificacion, "SJF")) return pick_next_carpincho_by_sjf();
    if (!strcmp(algoritmo_planificacion, "HRRN")) return pick_next_carpincho_by_hrrn();

    throw_error(CONFIG_ERROR, log_kernel, false);
    return list_remove(cola_ready, 0);
}

pcb* pick_next_carpincho_by_sjf() {
    double estimacion_min_value = 99999999999;
    int estimacion_min_index = -1;
    for (int i = 0; i < list_size(cola_ready); i++) {
        pcb* carpincho_i = list_get(cola_ready, i);
        double estimacion_anterior = carpincho_i->ultima_estimacion;
        double real_cpu = get_timestamp_diff(carpincho_i->blocked_at, carpincho_i->unblocked_at);

        double estimado_cpu = (estimacion_anterior * alpha_value) + real_cpu * (1 - alpha_value);
        carpincho_i->ultima_estimacion = estimado_cpu;

        if (estimado_cpu < estimacion_min_value) {
            estimacion_min_value = estimado_cpu;
            estimacion_min_index = i;
        }
    }

    return list_remove(cola_ready, estimacion_min_index);
}

double get_timestamp_diff(char* timestamp_1, char* timestamp_2) {
    int length_1 = string_length(timestamp_1);
    int length_2 = string_length(timestamp_2);

    if (!length_1 || !length_2) return 0;

    char* trimmed_timestamp_1 = string_substring_until(timestamp_1, length_1 - 1);
    char* trimmed_timestamp_2 = string_substring_until(timestamp_2, length_2 - 1);

    char** splitted_format_timestamp_1 = string_split(trimmed_timestamp_1, " -- ");
    char** splitted_format_timestamp_2 = string_split(trimmed_timestamp_2, " -- ");

    char** timestamp_arr_1 = string_n_split(splitted_format_timestamp_1[1], 4, ":");
    char** timestamp_arr_2 = string_n_split(splitted_format_timestamp_2[1], 4, ":");

    int hour_1 = atoi(timestamp_arr_1[0]);
    int min_1 = atoi(timestamp_arr_1[1]);
    int sec_1 = atoi(timestamp_arr_1[2]);
    char* coso = timestamp_arr_1[3];
    int ms_1 = atoi(timestamp_arr_1[3]);

    int hour_2 = atoi(timestamp_arr_2[0]);
    int min_2 = atoi(timestamp_arr_2[1]);
    int sec_2 = atoi(timestamp_arr_2[2]);
    int ms_2 = atoi(timestamp_arr_2[3]);

    double ms_diff = (hour_1 - hour_2) * 3600000 + (min_1 - min_2) * 60000 + (sec_1 - sec_2) * 1000 + (ms_1 - ms_2);
    return ms_diff;
}

pcb* pick_next_carpincho_by_hrrn() {
    return list_remove(cola_ready, 0);
}