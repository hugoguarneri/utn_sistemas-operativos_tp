#ifndef SRC_RECETA_H_
#define SRC_RECETA_H_
#include "sindicato.h"
#include "filesystem.h"

char *afip_crear_receta(t_crear_receta_ctx *data);
void afip_guardar_receta_bin(char *filename, int bloque_inicial, int tamanio_total);
void crear_receta_ctx(t_crear_receta_ctx *datos);
char * ejecutar_crear_receta(char *nombre, char *pasos, char *tiempo_pasos);
void free_crear_receta_ctx(t_crear_receta_ctx *data);
char* afip_obtener_receta(char *nombre);

#endif /* SRC_RECETA_H_ */
