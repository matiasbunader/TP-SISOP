#ifndef SEMAFOROS_H
#define SEMAFOROS_H

#include "kernel.h"

t_list * mate_sem_list;

mate_sem_t* deserializar_mate_sem(t_paquete* paquete);
char* deserializar_mate_sem_name(t_paquete* paquete);
mate_sem_t* get_mate_sem_by_name(char* sem_name);
void manejar_sem_destroy(mate_sem_t* mate_sem, pcb* carpincho);
void manejar_sem_init(mate_sem_t* mate_sem, pcb* carpincho); 
void manejar_sem_wait(mate_sem_t* mate_sem, pcb* carpincho);
void manejar_sem_post(mate_sem_t* mate_sem, pcb* carpincho);

#endif


