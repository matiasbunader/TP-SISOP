#ifndef PLANIFICADORES_H
#define PLANIFICADORES_H

#include "kernel.h"

void planificadores_init(void);
void lanzar_planificador_largo_plazo(void);
void lanzar_planificador_corto_plazo(int grado_multiprocesamiento);
void* manejar_multiprogramacion(void);
void* ejecutar(int index);
void pasar_a_ready(pcb* carpincho);
void pasar_a_exec(pcb* carpincho);
void pasar_a_blocked(pcb* carpincho);
void pasar_a_exit(pcb* carpincho);

pcb* get_carpincho_by_fd(int client_fd);
int index_of_carpincho_by_fd(int client_fd);

pcb* pick_next_carpincho_to_exec(void);
pcb* pick_next_carpincho_by_sjf(void);
pcb* pick_next_carpincho_by_hrrn(void);

double get_timestamp_diff(char* timestamp_1, char* timestamp_2);

#endif