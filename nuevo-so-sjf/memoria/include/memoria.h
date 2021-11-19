#ifndef MEMORIA_H
#define MEMORIA_H

#include "log.h"
#include "config.h"
#include "utils.h"
#include "estructuras.h"
#include "deserializacion.h"


typedef struct
{
	int pagina;
    //int proceso;   NO HACE FALTA
    int presencia;
    int marco;
    int LRU;
    int modif;
    int uso;

}t_fila_tabla_paginas;

typedef struct
{
    t_list* tablaDePaginas;
    int client_fd;
    int id_proceso;

}t_proceso_info;


typedef struct{
    uint32_t prevAlloc;
    uint32_t nextAlloc;
    uint8_t isFree;
} HeapMetaData;

int calcularDireccionFisica(int direccionLogica);
int buscarMarcoEnTabla(int numeroDePagina);
void servir_cliente(int client_fd);
void aceptar_clientes(int server_fd);
void atender_mensajes(int client_fd, t_proceso_info* proceso);
t_proceso_info* crearProcesoEnMemoria(int client_fd);
void process_request(t_paquete* paquete, int socket, t_proceso_info* proceso);
void asignarMemoriaFirstFit(received_vmalloc, proceso, tamanio);
int buscarFirstFit(int *encontrado, void* bloque_a_agregar_en_memoria, uint32_t tamanio_a_agregar);
HeapMetaData* guardarEnPosicion(void* bloque_a_agregar_en_memoria, uint32_t tamanio_a_agregar, int posicion_inicial_nuevo_heapmetadata);
bool entraEnAlgunaEstructura(uint32_t tamanio);

//VARIABLES
int proceso_id;




#endif