#ifndef MONITOR_INFO_CLIENTES_H_
#define MONITOR_INFO_CLIENTES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>

#include "../../../../shared/src/structs/info-cliente.h"


void monitor_info_clientes_init();

void monitor_info_clientes_destroy();

bool monitor_info_clientes_has_key(char* id_cliente);

void monitor_info_clientes_put(char* id_cliente, t_info_cliente* info_cliente);

t_info_cliente* monitor_info_clientes_get(char* id_cliente);

t_list* monitor_info_clientes_get_all();

void monitor_info_clientes_remove(char* id_cliente);

#endif /* MONITOR_INFO_CLIENTES_H_ */
