#ifndef SRC_STRUCTS_MONITOR_COCINEROS_H_
#define SRC_STRUCTS_MONITOR_COCINEROS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>

#include "cocinero.h"


void monitor_cocineros_init();

void monitor_cocineros_destroy();

bool monitor_cocineros_has_key(char* id_cocinero);

void monitor_cocineros_put(char* id_cocinero, t_cocinero* cocinero);

t_cocinero* monitor_cocineros_get(char* id_cocinero);

void monitor_cocineros_update_estado(char* id_cocinero, e_estado estado);

t_list* monitor_cocineros_get_all();

void monitor_cocineros_remove(char* id_cocinero);

#endif /* SRC_STRUCTS_MONITOR_COCINEROS_H_ */
