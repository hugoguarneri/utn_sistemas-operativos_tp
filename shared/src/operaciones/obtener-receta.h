#ifndef SRC_OPERACIONES_OBTENER_RECETA_H_
#define SRC_OPERACIONES_OBTENER_RECETA_H_

#include <stdio.h>
#include <string.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include "utils.h"
#include "opcode.h"
#include "paquete.h"

typedef struct {
	char* receta;
} t_obtener_receta_req;

t_obtener_receta_req* obtener_receta_req_create(char* receta);
void obtener_receta_req_destroy(t_obtener_receta_req* request);
t_paquete* obtener_receta_req_empaquetar(t_obtener_receta_req* request);
t_obtener_receta_req* obtener_receta_req_desempaquetar(t_paquete* paquete);
char* obtener_receta_req_to_string(t_obtener_receta_req* request);

typedef struct {
	char* paso;
	int tiempo;
} t_paso_tiempo;

typedef struct {
	t_list* pasos_tiempo;
} t_obtener_receta_resp;

t_paso_tiempo* paso_tiempo_create(char* receta, char* tiempo);
void paso_tiempo_destroy(t_paso_tiempo* paso_tiempo);
int paso_tiempo_size(t_paso_tiempo* paso_tiempo);

void obtener_receta_resp_destroy(t_obtener_receta_resp* response);
t_paquete* obtener_receta_resp_data_empaquetar(char *);
t_paquete* obtener_receta_resp_empaquetar(t_obtener_receta_resp* response);
t_obtener_receta_resp* obtener_receta_resp_desempaquetar(t_paquete* paquete);
char* obtener_receta_resp_to_string(t_obtener_receta_resp* response);

#endif /* SRC_OPERACIONES_OBTENER_RECETA_H_ */
