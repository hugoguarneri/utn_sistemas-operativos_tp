#ifndef OBTENER_RESTAURANTE_H_
#define OBTENER_RESTAURANTE_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include "utils.h"
#include "opcode.h"
#include "paquete.h"


typedef struct {
	char* restaurante;
} t_obtener_restaurante_req;

t_obtener_restaurante_req* obtener_restaurante_req_create(char* restaurante);

void obtener_restaurante_req_destroy(t_obtener_restaurante_req* request);

t_paquete* obtener_restaurante_req_empaquetar(t_obtener_restaurante_req* request);

t_obtener_restaurante_req* obtener_restaurante_req_desempaquetar(t_paquete* paquete);

char* obtener_restaurante_req_to_string(t_obtener_restaurante_req* request);

typedef struct {
	char* receta;
	int precio;
} t_receta_precio;

typedef struct {
	t_list* afinidad;
	uint32_t pos_x;
	uint32_t pos_y;
	t_list* receta_precio;
	uint32_t cant_hornos;
	uint32_t cant_pedidos;
	uint32_t cant_cocineros;
} t_obtener_restaurante_res;

t_receta_precio* receta_precio_create(char* receta, char* precio);

void receta_precio_destroy(t_receta_precio* receta_precio);

t_obtener_restaurante_res* obtener_restaurante_res_create(
		t_list* afinidad, uint32_t pos_x, uint32_t pos_y, t_list* receta_precio, uint32_t cant_hornos, uint32_t cant_pedidos, uint32_t cant_cocineros);

void obtener_restaurante_res_destroy(t_obtener_restaurante_res* response);

t_paquete* obtener_restaurante_res_data_empaquetar(char *, uint32_t);
t_paquete* obtener_restaurante_res_empaquetar(t_obtener_restaurante_res* response);

t_obtener_restaurante_res* obtener_restaurante_res_desempaquetar(t_paquete* paquete);


char* obtener_restaurante_res_to_string(t_obtener_restaurante_res* response);

int receta_precio_size(t_receta_precio *receta_precio);

#endif /* OBTENER_RESTAURANTE_H_ */
