#ifndef SRC_PLANIFICADOR_PLANIFICADOR_RESTAURANTE_H_
#define SRC_PLANIFICADOR_PLANIFICADOR_RESTAURANTE_H_

#include "restaurante.h"

#include "opcode.h"
#include "paquete.h"
#include "sockets.h"
#include "operaciones/obtener-restaurante.h"

#include "colas/cola-new.h"
#include "colas/cola-ready.h"
#include "colas/cola-exec.h"
#include "colas/cola-exit.h"
#include "colas/cola-block.h"
#include "structs/cocinero.h"
#include "structs/monitor-cocineros.h"

#define AFINIDAD_DEFAULT "Afinidad Default"
#define COLA_BLOCK_REPOSO "Cola_Block_Reposo"
#define COLA_BLOCK_HORNO "Cola_Block_Horno"
#define COLA_EXIT "Cola_Exit"

sem_t sem_ciclo_cpu;
pthread_mutex_t mutex_cola_prioridades_horno;
pthread_mutex_t mutex_cola_exit;

void exec_planificador_restaurante(t_obtener_restaurante_res* info_restaurante);

#endif /* SRC_PLANIFICADOR_PLANIFICADOR_RESTAURANTE_H_ */
