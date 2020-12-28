#include "pedido-control-block.h"



t_pcb* pcb_create(char* id, char* id_pedido, char* restaurante, char* id_repartidor, char* id_cliente, double estimacion_inicial){
	t_pcb* pcb = malloc(sizeof(t_pcb));
	pcb->id = strdup(id);
	pcb->id_pedido = strdup(id_pedido);
	pcb->restaurante = strdup(restaurante);
	pcb->id_repartidor = id_repartidor;
	pcb->id_cliente = strdup(id_cliente);
	pcb->destino = RESTAURANTE;
	pcb->estado_repartidor = ACTIVO;

	pcb->est_anterior = estimacion_inicial;
	pcb->raf_anterior = estimacion_inicial;
	pcb->raf_contador = 0;

	pcb->w_time = 0;

	return pcb;
}

t_pcb* pcb_duplicate(t_pcb* pcb){
	t_pcb* pcb_aux = malloc(sizeof(t_pcb));
	pcb_aux->id = strdup(pcb->id);
	pcb_aux->id_pedido = strdup(pcb->id_pedido);
	pcb_aux->restaurante = strdup(pcb->restaurante);
	pcb_aux->id_cliente = strdup(pcb->id_cliente);
	pcb_aux->destino = pcb->destino;
	pcb_aux->estado_repartidor = pcb->estado_repartidor;

	if(pcb->id_repartidor != NULL){
		pcb_aux->id_repartidor = strdup(pcb->id_repartidor);
	}else{
		pcb_aux->id_repartidor = pcb->id_repartidor;
	}

	pcb_aux->est_anterior = pcb->est_anterior;
	pcb_aux->raf_anterior = pcb->raf_anterior;
	pcb_aux->raf_contador = pcb->raf_contador;

	pcb_aux->w_time = pcb->w_time;
	return pcb_aux;
}

void pcb_destroy(t_pcb* pcb){
	free(pcb->id);
	free(pcb->id_pedido);
	free(pcb->restaurante);
	free(pcb->id_repartidor);
	free(pcb->id_cliente);
	free(pcb);
}

