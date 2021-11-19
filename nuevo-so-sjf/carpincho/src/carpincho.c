#include "carpincho.h"

int main() {
    log_carpincho = log_create(LOG_FILE_PATH, "CARPINCHO", true, LOG_LEVEL_INFO);
    // Matelib instantiation
    mate_instance mate_ref;
    int status = mate_init(&mate_ref, CONFIG_FILE_PATH);

    if (status != SUCCESS) {
        log_error(log_carpincho, "Error instanciando matelib");
        throw_error(status, log_carpincho, 1);
    }

    log_info(log_carpincho, "Soy el proceso Carpincho! \n");
    log_info(log_carpincho, "Connected to %s => %s:%s ... ",
             obtener_nombre_modulo(mate_ref.group_info->backend_code),
             mate_ref.group_info->ip,
             mate_ref.group_info->port);

    char* linea;
    while (1) {
        printf("\n¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯\n");
        printf("\ncarpincho dice...\n\n");
        printf("\033[1;33m");
        linea = (char*) readline("ʕ=｀ェ´=ʔ < ");
        printf("\033[0m"); 
        if (strcmp(linea, "exit") == 0 || strcmp(linea, "q") == 0) {
            free(linea);
            break;
        }
        if (!strcmp(linea, "io")) run_io(&mate_ref);
        else if (!strcmp(linea, "io2")) run_io2(&mate_ref);
        else if (!strcmp(linea, "sem")) run_test_sem(&mate_ref);
        else if (!strcmp(linea, "sem wait")) run_test_sem_wait(&mate_ref);
        else if (!strcmp(linea, "sem post")) run_test_sem_post(&mate_ref);
        else if (!strcmp(linea, "mem")) run_mem(&mate_ref);
        else printf("\nno te entiendo nada...\n\n");
        free(linea);
    }

    log_destroy(log_carpincho);
    mate_close(&mate_ref);
    return 0;
}

void run_io(mate_instance* mate_ref) {
    log_info(log_carpincho, "llamaré a laguna-io (2500 ms) ... \n");
    int res = mate_call_io(mate_ref, "laguna", "((mensaje))");

    log_info(log_carpincho, "terminó laguna-io\n");

    log_info(log_carpincho, "llamaré a hierbitas-io (10000 ms) ... \n");
    res = mate_call_io(mate_ref, "hierbitas", "((mensaje2))");
    log_info(log_carpincho, "[ %d ]terminó hierbitas-io\n", res);
}

void run_io2(mate_instance* mate_ref) {
    log_info(log_carpincho, "llamaré a hierbitas-io (10000 ms) ... \n");
    int res = mate_call_io(mate_ref, "hierbitas", "((mensaje2))");
    log_info(log_carpincho, "[ %d ]terminó hierbitas-io \n", res);
}

void run_test_sem(mate_instance* mate_ref) {
    int res = mate_sem_init(mate_ref, "sem_1", 1);
    log_info(log_carpincho, "[ %d ]", res);
    sleep(3);

    res = mate_sem_wait(mate_ref, "sem_1");
    log_info(log_carpincho, "[ %d ]", res);
    sleep(5);

    res = mate_sem_post(mate_ref, "sem_1");
    log_info(log_carpincho, "[ %d ]", res);
    sleep(3);

    // mate_sem_destroy(mate_ref, "sem_1");
}

void run_test_sem_wait(mate_instance* mate_ref) {
    int res = mate_sem_wait(mate_ref, "sem_1");
    log_info(log_carpincho, "[ %d ]", res);
    sleep(5);
}

void run_test_sem_post(mate_instance* mate_ref) {
    int res = mate_sem_post(mate_ref, "sem_1");
    log_info(log_carpincho, "[ %d ]", res);
    sleep(3);
}

void run_mem(mate_instance* mate_ref) {
    // TODO
}