#ifndef SRC_PLANIFICADOR_COLAS_COLA_BLOCK_H_
#define SRC_PLANIFICADOR_COLAS_COLA_BLOCK_H_

#include "stdbool.h"
#include "restaurante.h"
#include "planificador/planificador-restaurante.h"
#include "operaciones/obtener-restaurante.h"

void cola_block_reposo_listener();
void cola_block_horno_listener(t_obtener_restaurante_res* info_restaurante);
void pedir_horno(char* pcb_id);

#endif /* SRC_PLANIFICADOR_COLAS_COLA_BLOCK_H_ */
