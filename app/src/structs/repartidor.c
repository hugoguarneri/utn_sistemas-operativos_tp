#include "repartidor.h"

t_repartidor* repartidor_create(char* id, int x, int y, int t_descanso, int t_actividad, e_estado estado){
	t_repartidor* repartidor = malloc(sizeof(t_repartidor));
	repartidor->id_repartidor = strdup(id);
	repartidor->pos_x = x;
	repartidor->pos_y = y;
	repartidor->tiempo_de_descanso = t_descanso;
	repartidor->tiempo_de_actividad = t_actividad;
	repartidor->estado = estado;
	repartidor->tiempo = 0;

	return repartidor;
}

void repartidor_destroy(t_repartidor* repartidor){
	free(repartidor->id_repartidor);
	free(repartidor);
}
