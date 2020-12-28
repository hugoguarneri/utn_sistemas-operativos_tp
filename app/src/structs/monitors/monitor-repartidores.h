#ifndef MONITOR_REPARTIDORES_H_
#define MONITOR_REPARTIDORES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>

#include "../repartidor.h"


void monitor_repartidores_init();

void monitor_repartidores_destroy();

bool monitor_repartidores_has_key(char* id_repartidor);

void monitor_repartidores_put(char* id_repartidor, t_repartidor* repartidor);

t_repartidor* monitor_repartidores_get(char* id_repartidor);

t_list* monitor_repartidores_get_all();

void monitor_repartidores_remove(char* id_cliente);


#endif /* MONITOR_REPARTIDORES_H_ */
