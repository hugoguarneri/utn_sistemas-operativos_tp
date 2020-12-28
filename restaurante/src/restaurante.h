#ifndef RESTAURANTE_H
#define RESTAURANTE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "sockets.h"
#include <sys/socket.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>

#include "opcode.h"
#include "paquete.h"
#include "structs/colas-planificacion.h"
#include "planificador/planificador-restaurante.h"
#include "servidor/servidor-restaurante.h"
#include "servidor/handshake-app.h"
#include "servidor/handshake-sindicato.h"

//todo: sacar esto ya que el path del log esta en el config
//#define LOG_PATH "./cfg/restaurante.log"
#define LOG_NAME "RESTAURANTE"
//#define CONFIG_PATH "./cfg/restaurante.config"

extern int errno;

t_log* logger;
t_config* config;

pthread_t thread_servidor;
pthread_t thread_planificador;

sem_t sem_habilitar_servidor;
t_dictionary* sems_cocineros_libres;

int id_pedido;
int id_pcb;

t_cola_planif* cola_new;
t_cola_planif* cola_ready;
t_cola_planif* cola_exec;
t_cola_planif* cola_block_reposo;
t_cola_planif* cola_block_horno;
t_cola_planif* cola_exit;

t_list* lista_prioridad_horno;

t_obtener_restaurante_res* info_restaurante;

#endif /* RESTAURANTE_H */
