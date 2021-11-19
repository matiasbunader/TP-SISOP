#include "utils.h"

backend_code obtener_backend_code(char *backend) {
    if (strcmp(backend, "KERNEL") == 0)
        return KERNEL;
    if (strcmp(backend, "MEMORIA") == 0)
        return MEMORIA;
    return UNKNOWN;
}

char *obtener_nombre_modulo(backend_code modulo) {
    if (modulo == 1)
        return "KERNEL";
    if (modulo == 2)
        return "MEMORIA";
    return "UNKNOWN";
}

// TODO: reemplazar por uuid
char *rand_string(char *str, size_t size) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK...";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int)(sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

void liberar_conexion(int socket_cliente) {
    // shutdown(socket_cliente, SHUT_RDWR);
    close(socket_cliente);
}

int crear_conexion(char *ip, char *puerto, t_log *logger) {
    struct addrinfo hints;
    struct addrinfo *server_info;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &server_info);

    // Ahora vamos a crear el socket.
    int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype,
                                server_info->ai_protocol);

    // Ahora que tenemos el socket, vamos a conectarlo
    if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
        log_error(logger, "error al conectar");
        throw_error(CONNECTION_ERROR, logger, 0);
    }

    freeaddrinfo(server_info);

    return socket_cliente;
}

int iniciar_servidor(char *ip, char *puerto, t_log *logger) {
    int socket_servidor;

    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &servinfo);

    // Creamos el socket de escucha del servidor
    socket_servidor =
        socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    // Asociamos el socket a un puerto
    if (bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen) != 0) {
        log_error(logger, "falló el bind");
        throw_error(CONNECTION_ERROR, logger, 1);
    }

    // Escuchamos las conexiones entrantes
    listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    log_info(logger, "Socket(%d) :: Escuchando en %s:%s", socket_servidor, ip, puerto);

    return socket_servidor;
}

int esperar_cliente(int socket_servidor, t_log *logger) {
    // Aceptamos un nuevo cliente
    int socket_cliente = accept(socket_servidor, NULL, NULL);

    if (socket_cliente == -1) {
        log_error(logger, "Error intentando aceptar un cliente");
        throw_error(CONNECTION_ERROR, logger, 0);
    }

    log_info(logger, "Socket(%d) :: Se conecto un cliente!", socket_cliente);

    return socket_cliente;
}
