#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include "utils.h"

#define LOG_FILE_PATH "./log/memoria.log"
#define CFG_FILE_PATH "./config/memoria.config"

t_log* log_memoria;
t_config* config_memoria;

void log_init(void);
void config_init(void);

char* obtener_cadena(char*);
int obtener_entero(char*);

#endif