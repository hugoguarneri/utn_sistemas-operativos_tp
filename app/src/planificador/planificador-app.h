#ifndef PLANIFICADOR_APP_H
#define PLANIFICADOR_APP_H

#include "app.h"

#include "planificador-cola-new.h"
#include "planificador-cola-ready.h"
#include "planificador-cola-exec.h"
#include "planificador-cola-block.h"
#include "planificador-cola-exit.h"

#include "../../../shared/src/opcode.h"
#include "../../../shared/src/paquete.h"
#include "../../../shared/src/sockets.h"
#include "../structs/monitors/monitor-clientes-restaurantes.h"
#include "../structs/monitors/monitor-info-clientes.h"
#include "../structs/monitors/monitor-info-restaurantes.h"
#include "../structs/monitors/monitor-repartidores.h"

pthread_t thread_planificador_cola_new;
pthread_t thread_planificador_cola_ready;
pthread_t thread_planificador_cola_exec;
pthread_t thread_planificador_cola_block;
pthread_t thread_planificador_cola_exit;



void exec_planificador_app();

#endif /* PLANIFICADOR_APP_H */
