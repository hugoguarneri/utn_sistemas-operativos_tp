#ifndef CLIENTE_H_
#define CLIENTE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>

#include "config.h"
#include "logger.h"
#include "opcode.h"
#include "paquete.h"
#include "sockets.h"
#include "structs/info-cliente.h"

#include "consola-cliente.h"
#include "notificaciones-cliente.h"

#define LOG_NAME "CLIENTE"

t_info_cliente* info_cliente;

pthread_t thread_consola;
pthread_t thread_notificaciones;

sem_t sem_habilitar_notificaion;
sem_t sem_habilitar_consola;

void finalizar_proceso();

#endif /* CLIENTE_H_ */
