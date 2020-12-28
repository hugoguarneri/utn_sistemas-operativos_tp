#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netdb.h>
#include <string.h>
#include <stdarg.h>

#include "logger.h"
#include "opcode.h"
#include "paquete.h"

pthread_t thread;

int crear_conexion (char* ip, char* puerto);

void liberar_conexion (int socket);

int iniciar_servidor (char *ip, char* puerto);

int esperar_cliente_ (int socket_servidor);

void esperar_cliente (int socket_servidor, void* operacion_a_ejecutar);

t_paquete* recibir_mensaje (int socket);

int enviar_mensaje (int socket, t_paquete* paquete);

#endif /* SOCKETS_H_ */
