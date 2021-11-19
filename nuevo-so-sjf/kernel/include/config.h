#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include "kernel.h"
#include "utils.h"

#define LOG_FILE_PATH "./log/kernel.log"
#define CFG_FILE_PATH "./config/kernel.config"

t_log *log_kernel;
t_config* config_kernel;

void log_init(void);
void config_init(void);

double alpha_value;

#endif
