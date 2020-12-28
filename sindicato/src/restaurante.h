#ifndef SRC_RESTAURANTE_H_
#define SRC_RESTAURANTE_H_
#include "sindicato.h"
#include "filesystem.h"

char* ejecutar_crear_restaurante(char *nombre, char *cantidad_cocineros, char *posicion, char *afinidad, char *platos,
		char *precio_platos, char *cantidad_hornos);
void free_crear_rest_ctx(t_crear_rest_ctx *datos);
char *afip_crear_restaurante(t_crear_rest_ctx *data);
void afip_guardar_restaurante_bin(char *filename, int bloque_inicial, int tamanio_total);
char* afip_obtener_restaurante(char *nombre);
char* afip_leer_archivo(char *afip_path);
char* afip_obtener_restaurante(char *nombre);
bool existe_restaurante(char *restaurante);
uint32_t afip_obtener_cantidad_pedidos(char *restaurante);

#endif /* SRC_RESTAURANTE_H_ */
