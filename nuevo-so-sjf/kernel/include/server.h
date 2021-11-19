#ifndef SERVER_H
#define SERVER_H

#include "kernel.h"

void server_init(void);
void* aceptar_clientes(conexiones_fds* fds);
void* servir_cliente(conexiones_fds* fds);
pcb* pcb_init(int client_fd);
void atender_mensajes(conexiones_fds fds);
void process_request(t_paquete* paquete, conexiones_fds fds);
int conectar_con_memoria();

#endif