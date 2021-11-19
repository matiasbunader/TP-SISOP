#ifndef IO_H
#define IO_H

#include "kernel.h"

void io_init(void);
void* simular_io(mate_io_dispositivo* io_name);
void* simular_espera(mate_io_dispositivo* io);
mate_io_command* deserializar_io_command(t_paquete* paquete);
mate_io_dispositivo* get_io_by_key(char* key);
void manejar_io(mate_io_command* io_command, pcb* carpincho);

#endif