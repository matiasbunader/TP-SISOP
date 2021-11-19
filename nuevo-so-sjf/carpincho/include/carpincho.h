#ifndef CARPINCHO_H
#define CARPINCHO_H

#include <commons/config.h>
#include <commons/log.h>
#include <readline/readline.h>
#include <matelib.h>
#include "utils.h"

#define LOG_FILE_PATH "./log/carpincho.log"
#define CONFIG_FILE_PATH "cfg/carpincho.config"

t_log *log_carpincho;

void run_io(mate_instance* mate_ref);
void run_io2(mate_instance* mate_ref);
void run_test_sem(mate_instance* mate_ref);
void run_test_sem_wait(mate_instance* mate_ref);
void run_test_sem_post(mate_instance* mate_ref);
void run_mem(mate_instance* mate_ref);

#endif