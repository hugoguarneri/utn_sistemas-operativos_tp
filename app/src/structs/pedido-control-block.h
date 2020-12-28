#ifndef PEDIDO_CONTROL_BLOCK_H_
#define PEDIDO_CONTROL_BLOCK_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <commons/string.h>
#include <commons/collections/list.h>

typedef enum {
	RESTAURANTE = 1, CLIENTE = 2
} e_destino;

typedef enum {
	ACTIVO = 1, REPARTIDOR_EN_DESCANSO = 2,  REPARTIDOR_ESPERANDO_PEDIDO = 3
} e_estado_repartidor;

typedef struct {
	char* id;

	char* id_pedido;
    char* restaurante;
    char* id_repartidor;
    char* id_cliente;
    e_destino destino;
    e_estado_repartidor estado_repartidor;

    double est_anterior;
    double raf_anterior;
    double raf_contador;

    int w_time;

} t_pcb;


t_pcb* pcb_create(char* id, char* id_pedido, char* restaurante, char* id_repartidor, char* id_cliente, double estimacion_inicial);

t_pcb* pcb_duplicate(t_pcb* pcb);

void pcb_destroy(t_pcb* pcb);


#endif /* PEDIDO_CONTROL_BLOCK_H_ */
