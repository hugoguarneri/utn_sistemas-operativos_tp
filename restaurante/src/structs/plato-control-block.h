#ifndef SRC_STRUCTS_PLATO_CONTROL_BLOCK_H_
#define SRC_STRUCTS_PLATO_CONTROL_BLOCK_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <commons/string.h>
#include <commons/collections/list.h>

#define REPOSAR "Reposar"
#define HORNEAR "Hornear"


typedef struct {
	char* id;
	char* id_pedido;
	char* id_cocinero;
    char* plato;
    t_list* pasos_tiempo;

    int index_paso; //Actual paso
    int tiempo_paso; //Rafagas del paso
    int quantum;
    int rafaga_cpu;
} t_pcb;

typedef struct {
	char* paso;
	int tiempo;
} t_paso_info;

t_pcb* pcb_create(char* id, char* id_pedido, char* id_cocinero, char* plato, t_list* pasos_tiempos, int quantum);
t_pcb* pcb_duplicate(t_pcb* pcb);
void pcb_destroy(t_pcb* pcb);

#endif /* SRC_STRUCTS_PLATO_CONTROL_BLOCK_H_ */
