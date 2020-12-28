#ifndef APP_H_
#define APP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>

#include "../../shared/src/config.h"
#include "../../shared/src/logger.h"
#include "../../shared/src/opcode.h"
#include "../../shared/src/paquete.h"
#include "../../shared/src/sockets.h"

#include "structs/monitors/monitor-info-clientes.h"
#include "structs/monitors/monitor-info-restaurantes.h"
#include "structs/monitors/monitor-pedido-default.h"
#include "structs/monitors/monitor-repartidores.h"

#include "structs/colas-planificacion.h"
#include "structs/pedido-control-block.h"
#include "structs/repartidor.h"

#include "servidor/servidor-app.h"
#include "servidor/handshake-comanda.h"
#include "servidor/restaurante-default.h"
#include "planificador/planificador-app.h"
#include "structs/monitors/monitor-clientes-restaurantes.h"


#define LOG_NAME "APP"
#define LOG_FILENAME "./cfg/app.log"

#define CONFIG_FILENAME "./cfg/app.config"

pthread_t thread_servidor;
pthread_t thread_planificador;
pthread_t thread_restaurante;

sem_t sem_habilitar_resto_default;
sem_t sem_habilitar_servidor;

sem_t sem_repartidores_libres;
sem_t sem_grado_multiprocesamiento;

sem_t sem_cola_new;
sem_t sem_cola_ready;
sem_t sem_cola_exec;
sem_t sem_cola_block;
sem_t sem_cola_exit;

pthread_mutex_t mutex_cola_block;
pthread_mutex_t mutex_cola_ready;

t_cola_planificacion* cola_new;
t_cola_planificacion* cola_ready;
t_cola_planificacion* cola_exec;
t_cola_planificacion* cola_block;
t_cola_planificacion* cola_exit;

int id_pcb;

void finalizar_proceso();

#endif /* APP_H_ */
