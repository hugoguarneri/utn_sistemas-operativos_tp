#ifndef SRC_REPARTIDOR_H_
#define SRC_REPARTIDOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

typedef enum {
	LIBRE = 0, OCUPADO = -1
} e_estado;

typedef struct {
	char* id_repartidor;
	int pos_y;
	int pos_x;
	int tiempo_de_descanso;
	int tiempo_de_actividad;
	e_estado estado;
	int tiempo;
} t_repartidor;


t_repartidor* repartidor_create(char* id, int x, int y, int t_descanso, int t_actividad, e_estado estado);

void repartidor_destroy(t_repartidor* repartidor);


#endif /* SRC_REPARTIDOR_H_ */
