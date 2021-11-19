#ifndef UTILS_H
#define UTILS_H

#include <commons/log.h>
#include <matelib.h>
#include "errors.h"

#define PROCEDA 888  // codigo para señalizar ACK

backend_code obtener_backend_code(char *backend_code);

char *obtener_nombre_modulo(backend_code codigo);

char *rand_string(char *str, size_t size);

int crear_conexion(char *ip, char *puerto, t_log *logger);

void liberar_conexion(int socket_cliente);

int iniciar_servidor(char *ip, char *puerto, t_log *logger);

int esperar_cliente(int socket_servidor, t_log *logger);

void serve_client(int socket);

#endif
