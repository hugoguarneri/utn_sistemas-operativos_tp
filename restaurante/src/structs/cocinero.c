#include "cocinero.h"


t_cocinero* cocinero_create(char* id, e_estado estado, char* afinidad){
	t_cocinero* cocinero = malloc(sizeof(t_cocinero));
	cocinero->id_cocinero = strdup(id);
	cocinero->afinidad = strdup(afinidad);
	cocinero->estado = estado;

	return cocinero;
}

void cocinero_destroy(t_cocinero* cocinero){
	free(cocinero->id_cocinero);
	free(cocinero->afinidad);
	free(cocinero);
}

