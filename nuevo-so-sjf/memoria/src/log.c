#include "log.h"

void leer_log(){
	if(log_create("./log/memoria.log", "MEMORIA", true, LOG_LEVEL_INFO) == NULL){
			printf("No se pudo leer el archivo de log de Memoria.");
			exit(3);

	}
	log_memoria = log_create("./log/memoria.log", "MEMORIA", true, LOG_LEVEL_INFO);
}