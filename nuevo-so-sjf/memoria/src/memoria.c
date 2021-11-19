#include "memoria.h"

int main(int argc, char **argv) {
    
    log_init();
    config_init();
    proceso_id = 0;

    char* ip = config_get_string_value(config_memoria, "IP");
    char* puerto = config_get_string_value(config_memoria, "PUERTO");

    if (string_is_empty(ip) || string_is_empty(puerto)) exit(CONFIG_ERROR);

    int server_fd = iniciar_servidor(ip, puerto, log_memoria);
    
    
    // lanzar thread de aceptacion de clientes
    pthread_t aceptacion_de_clientes;
    pthread_create(&aceptacion_de_clientes, NULL, aceptar_clientes, server_fd);
    pthread_detach(&aceptacion_de_clientes);

   
    while(1){
    
    }
    


    
    
    log_destroy(log_memoria);
}


void aceptar_clientes(int server_fd) {
    while (1) {
        int client_fd = esperar_cliente(server_fd, log_memoria);

        // lanzar thread de escucha de mensajes por cliente
        pthread_t escucha_de_mensajes;
        pthread_create(&escucha_de_mensajes, NULL, servir_cliente, client_fd);
        pthread_detach(&escucha_de_mensajes);
    }
}

void servir_cliente(int client_fd) {
    
    

    t_proceso_info* proceso = crearProcesoEnMemoria(client_fd);


    atender_mensajes(client_fd, proceso);
}

t_proceso_info* crearProcesoEnMemoria(int client_fd){
    proceso_id ++;
    t_proceso_info* proceso = malloc(sizeof(t_proceso_info));
    proceso->client_fd = client_fd;
    proceso->id_proceso = proceso_id;
    proceso->tablaDePaginas = list_create();

    return proceso;
}

void atender_mensajes(int client_fd, t_proceso_info* proceso) {
    while (1) {
        t_paquete* paquete = malloc(sizeof(t_paquete));
        paquete->buffer = malloc(sizeof(t_buffer));
        // Recibimos el codigo de operacion
        recv(client_fd, &(paquete->codigo_operacion), sizeof(int32_t), MSG_WAITALL);
        // Recibimos el tamaño del buffer
        recv(client_fd, &(paquete->buffer->size), sizeof(int32_t), MSG_WAITALL);
        paquete->buffer->stream = malloc(paquete->buffer->size);
        // Recibimos el contenido buffer
        recv(client_fd, paquete->buffer->stream, paquete->buffer->size, MSG_WAITALL);

        process_request(paquete, client_fd, proceso);

        free(paquete->buffer->stream);
        free(paquete->buffer);
        free(paquete);
    }
}

void process_request(t_paquete* paquete, int socket, t_proceso_info* proceso){
    
    vmalloc_t* received_vmalloc;

    switch (paquete->codigo_operacion) {
        case MEM_ALLOC:
         
         log_info(log_memoria, "op: Reservar memoria");
 
        //Deserializar lo que me venga de matelib, buscar espacio libre y armar tabla de páginas
         received_vmalloc = deserializar_vmalloc(paquete);
         int tamanio = sizeof(HeapMetaData) + received_vmalloc->value;
         asignarMemoriaFirstFit(received_vmalloc, proceso, tamanio);
       

           

           // send(socket, ACK, sizeof(op_code), 0);
            break;
        default:
            throw_error(UNKNOWN, log_memoria, 1);
    }
}

//CHECKEAR BIEN SEMAFOROS


HeapMetaData* guardarEnPosicion(void* bloque_a_agregar_en_memoria, uint32_t tamanio_a_agregar, int posicion_inicial_nuevo_heapmetadata){

    HeapMetaData* nuevoHeapMetaData = malloc(sizeof(HeapMetaData));

    //memcpy(memoriaPrincipal + posicion_inicial_nuevo_heapmetadata, bloque_a_agregar_en_memoria, tamanio_a_agregar);  ESTO VA EN MEMWRITE
    nuevoHeapMetaData->prevAlloc = posicion_inicial_nuevo_heapmetadata;
    nuevoHeapMetaData->nextAlloc = sizeof(HeapMetaData) + tamanio_a_agregar;

    return nuevoHeapMetaData;


}

void asignarMemoriaFirstFit(vmalloc_t* memalloc, t_proceso_info* proceso, uint32_t tamanio_a_agregar){

    HeapMetaData* nuevoHeapMetaData;
    int encontrado = 0;

    int posicion_inicial_nuevo_heapmetadata = buscarFirstFit(&encontrado, memalloc, tamanio_a_agregar);

    if (encontrado == 1){

        nuevoHeapMetaData = guardarEnPosicion(memalloc, tamanio_a_agregar, posicion_inicial_nuevo_heapmetadata);

    }


}


int buscarFirstFit(int *encontrado, void* bloque_a_agregar_en_memoria, uint32_t tamanio_a_agregar){

    int posicion_inicial_nuevo_heapmetadata = -1;
    //int sumatoria = tamanio_a_agregar;
    
    
    //Buscar si el tamaño a asignar entra en alguna de las estructuras administrativas que ya existen
   // if (entraEnAlgunaEstructura()){


    //}


    //Si todavía la memoia está vacía, que devuelva la primera posición.
    if(list_is_empty(estructuras_administrativas)){ //¿No hay nada en memoria?
         posicion_inicial_nuevo_heapmetadata = 0;
        *encontrado = 1;
    }

    
    return posicion_inicial_nuevo_heapmetadata;
}

bool entraEnAlgunaEstructura(uint32_t tamanio){

    for (int i=0; i<list_size(estructuras_administrativas); i++){

    }
    return true;
}

























int calcularDireccionFisica(int direccionLogica){
    int tamanioPagina = config_get_int_value("TAMANIO_PAGINA", config_memoria);
    int numeroDePagina = direccionLogica / tamanioPagina;
    int marco = buscarMarcoEnTabla(numeroDePagina);
    int offset = direccionLogica % tamanioPagina;

    return 0;
}

int buscarMarcoEnTabla(numeroDePagina){
    

    return 0;
}


/* void serve_client(int socket) {
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));

    // Primero recibimos el codigo de operacion
    recv(socket, &(paquete->codigo_operacion), sizeof(int), MSG_WAITALL);
    // Después ya podemos recibir el buffer. Primero su tamaño seguido del contenido
    recv(socket, &(paquete->buffer->size), sizeof(int), MSG_WAITALL);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(socket, paquete->buffer->stream, paquete->buffer->size, MSG_WAITALL);

    process_request(paquete, socket);
}

*/