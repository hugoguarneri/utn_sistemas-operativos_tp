#ifndef MONITOR_INFO_RESTAURANTES_H_
#define MONITOR_INFO_RESTAURANTES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>

#include "../../../../shared/src/structs/info-restaurante.h"


void monitor_info_restaurantes_init();

void monitor_info_restaurantes_destroy();

bool monitor_info_restaurantes_has_key(char* restaurante);

void monitor_info_restaurantes_put(char* restaurante, t_info_restaurante* info_restaurante);

t_info_restaurante* monitor_info_restaurantes_get(char* restaurante);

t_list* monitor_info_restaurantes_get_all();

void monitor_info_restaurantes_remove(char* restaurante);

#endif /* MONITOR_INFO_RESTAURANTES_H_ */
