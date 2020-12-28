#ifndef SINDICATO_H
#define SINDICATO_H

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <commons/bitarray.h>
#include "utils.h"
#include "opcode.h"
#include "paquete.h"
#include "sockets.h"
#include "precios.h"

#define OK "Ok"
#define FAIL "Fail"

#define LOG_FILENAME "sindicato.log"
#define LOG_NAME "SINDICATO"
#define CONFIG_FILENAME "./cfg/sindicato.config"

#define CONFIG_PUNTO_MONTAJE "PUNTO_MONTAJE"
#define CONFIG_KEY_PUERTO "PUERTO_ESCUCHA"
#define CONFIG_KEY_IP "IP_ESCUCHA"

typedef struct {
	char *nombre;
	int cantidad_cocineros;
	char *posicion;
	char *afinidades;
	char *platos;
	char *precios;
	int cantidad_hornos;
} t_crear_rest_ctx;

typedef struct {
	char *nombre;
	char *pasos;
	char *tiempos;
} t_crear_receta_ctx;

#include "server.h"
#include "consola.h"
#include "filesystem.h"

/*** variables globales *****/
t_log *logger;
t_config *config;

#endif /* SINDICATO_H */
