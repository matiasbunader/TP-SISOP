#include "estructuras.h"


void crearMemoriaPrincipal(){
    int tamanioMemoria = config_get_int_value("TAMANIO", config_memoria);

    memoriaPrincipal = malloc(tamanioMemoria);
    memset(memoriaPrincipal,0,tamanioMemoria);
    
}



