#include "config.h"

void log_init(void) {
    log_memoria = log_create(LOG_FILE_PATH, "MEMORIA", true, LOG_LEVEL_INFO);
    log_info(log_memoria, "Soy el proceso Memoria! \n");
}

void config_init(void) {
	config_memoria = config_create(CFG_FILE_PATH);
	if (config_memoria == NULL){
		log_error(log_memoria, "No se pudo leer el archivo de configuración de Memoria.");
		throw_error(CONFIG_ERROR, log_memoria, 1);
	}
}


char* obtener_cadena(char* campo){
	char* cadena;
	cadena = config_get_string_value(config_memoria, campo);
	return cadena;
}


int obtener_entero(char* campo){
	int entero;
	entero = config_get_int_value(config_memoria, campo);
	return entero;
}

//Acá obtengo un vector, hay que ver como vamos obteniendo los elementos
char** obtener_vector(char* campo){
	char** dispositivos_io;
	dispositivos_io = config_get_array_value(config_memoria, campo);
	return dispositivos_io;
}



