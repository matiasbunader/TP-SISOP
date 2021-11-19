#include "config.h"

void log_init(void) {
    log_kernel = log_create(LOG_FILE_PATH, "KERNEL", true, LOG_LEVEL_INFO);
    log_info(log_kernel, "Soy el proceso Kernel! \n");
}

void config_init(void) {
	config_kernel = config_create(CFG_FILE_PATH);
	if (config_kernel == NULL){
		log_error(log_kernel, "No se pudo leer el archivo de configuración del Kernel.");
		throw_error(CONFIG_ERROR, log_kernel, 1);
	}

    char* ip = config_get_string_value(config_kernel, "IP_ESCUCHA");
    char* puerto = config_get_string_value(config_kernel, "PUERTO_ESCUCHA");
    alpha_value = config_get_double_value(config_kernel, "ALFA");

    if (string_is_empty(ip) || string_is_empty(puerto)) throw_error(CONFIG_ERROR, log_kernel, 1);
}

