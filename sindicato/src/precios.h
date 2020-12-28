#ifndef SRC_PRECIOS_H_
#define SRC_PRECIOS_H_
#include "sindicato.h"
#include <commons/string.h>

typedef struct {
	pthread_rwlock_t* rwlock;
	t_dictionary* dictionary;
} t_precios;

t_precios* precios;

void precios_init();
void precios_destroy();
int precios_get_precio(char* restaurante, char* receta);
void precios_set_precio(char* restaurante, char* receta, int precio);


#endif /* SRC_PRECIOS_H_ */
