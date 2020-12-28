#ifndef SRC_PEDIDO_H_
#define SRC_PEDIDO_H_
#include "sindicato.h"
#include "filesystem.h"
#include "precios.h"

#define ARRAY_VACIO "[]"
#define STR_ESTADO_PENDIENTE "Pendiente"
#define STR_ESTADO_CONFIRMADO "Confirmado"
#define STR_ESTADO_TERMINADO "Terminado"

typedef struct {
	char* estado;
	t_list* platos;
	int precio_total;
} t_pedido_data;

void pedido_data_destroy(t_pedido_data* data);


status_code afip_guardar_pedido(char *restaurante, char *id_pedido);
char* get_path_pedido(char *restaurante, char *id_pedido);
status_code afip_guardar_plato(char *restaurante, char *id_pedido, char *comida, int cantidad);
void pedido_data_agregar_plato(char *restaurante, t_pedido_data *pedido_data, char *comida, int cantidad);
char* pedido_to_stream(t_pedido_data* pedido_data);

status_code afip_plato_listo(char *restaurante, char *id_pedido, char *comida);
status_code afip_cambiar_estado_pedido(char *restaurante, char *id_pedido, estado_pedido actual, estado_pedido futuro, char *nombre_futuro);
bool pedido_data_plato_listo(t_pedido_data *pedido_data, char *comida);
t_obtener_pedido_res* afip_recuperar_pedido(char *restaurante, char *id_pedido);
t_obtener_pedido_res* afip_obtener_pedido(char *restaurante, char *id_pedido);
t_pedido_data* stream_to_pedido(char *data);

#endif /* SRC_PEDIDO_H_ */
