#ifndef COMANDA_H
#define COMANDA_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <commons/bitarray.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include "utils.h"
#include "opcode.h"
#include "paquete.h"
#include "sockets.h"
#include "config.h"
#include "logger.h"
#include "structs/estado-pedido.h"
#include "structs/estado-plato.h"

#define CONFIG_FILENAME "./cfg/comanda.config"
#define OK "Ok"
#define FAIL "Fail"

#define ARCHIVO_LOG "./cfg/comanda.log"
#define LOG_NAME "COMANDA"
#define PUERTO_ESCUCHA "PUERTO_ESCUCHA"
#define IP_ESCUCHA "IP_ESCUCHA"
#define TAMANIO_MEMORIA "TAMANIO_MEMORIA"
#define TAMANIO_SWAP "TAMANIO_SWAP"
#define ALGORITMO_REEMPLAZO "ALGORITMO_REEMPLAZO"

#define FRAME_SIZE 32

typedef enum {
	LRU = 0,
	CMJ = 1l
} enum_algoritmo;

typedef struct {
	char* puerto;
	char* ip_escucha;
	uint32_t tamanio_memoria_bytes;
	uint32_t tamanio_swap_bytes;
	uint32_t memoria_max_frames;
	uint32_t swap_max_frames;
	enum_algoritmo algoritmo;
} t_configuracion;

typedef struct {
	char plato[24];
	uint32_t cantidad;
	uint32_t cantidad_lista;
} __attribute__((packed)) t_plato_comanda;

t_configuracion* configuracion;

typedef struct {
	pthread_mutex_t mutex;
	t_dictionary* restaurantes;
} t_restaurante_dictionary;

typedef struct {
	pthread_mutex_t mutex;
	t_list* segmento_list;
} t_segmentos;

typedef struct {
	pthread_mutex_t mutex;
	t_list* pagina_list;
} t_paginas;

typedef struct {
	uint32_t pedido_id;
	e_estado_pedido estado;
	t_paginas* paginas;
} t_nodo_segmento;

typedef struct {
	pthread_mutex_t mutex;
	int32_t frame_principal;
	int32_t frame_virtual;
} t_nodo_pagina;

typedef struct {
	bool ocupado;
	uint32_t offset;
	long updated_at;
	t_nodo_pagina* referencia;
} t_nodo_frame_ppal;

typedef struct {
	bool ocupado;
	uint32_t offset;
} t_nodo_frame_virt;

t_restaurante_dictionary* restaurantes_dictionary;

t_log* iniciar_logger(void);
t_config* leer_config(void);
void terminar_programa();
void registrar_signals();
void manejar_signal(int signal);

#endif
