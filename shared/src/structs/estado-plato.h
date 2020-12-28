#ifndef ESTADO_PLATO_H_
#define ESTADO_PLATO_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <commons/string.h>

typedef struct {
	char* comida;
	uint32_t cant_total;
	uint32_t cant_lista;
} t_estado_plato;

t_estado_plato* estado_plato_create(char* comida, uint32_t cant_total, uint32_t cant_lista);

void estado_plato_destroy(t_estado_plato* estado_plato);

char* estado_plato_to_string(t_estado_plato* estado_plato);

uint32_t estado_plato_size(t_estado_plato* estado_plato);


#endif /* ESTADO_PLATO_H_ */
