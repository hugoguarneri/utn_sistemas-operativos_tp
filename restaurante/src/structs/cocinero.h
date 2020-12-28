#ifndef SRC_STRUCTS_COCINERO_H_
#define SRC_STRUCTS_COCINERO_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>

typedef enum {
	LIBRE = 0,
	OCUPADO = -1
} e_estado;

typedef struct {
	char* id_cocinero;
	e_estado estado;
	char* afinidad;
} t_cocinero;

t_cocinero* cocinero_create(char* id, e_estado estado, char* afinidad);
void cocinero_destroy(t_cocinero* cocinero);

#endif /* SRC_STRUCTS_COCINERO_H_ */
