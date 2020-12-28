#ifndef MONITOR_PEDIDO_DEFAULT_H_
#define MONITOR_PEDIDO_DEFAULT_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>

#include "../pedido-default.h"


void monitor_pedido_default_init();

void monitor_pedido_default_destroy();

bool monitor_pedido_default_has_key(char* id_pedido);

void monitor_pedido_default_put(char* id_pedido, t_pedido_default* pedido);

t_pedido_default* monitor_pedido_default_get(char* id_pedido);

t_list* monitor_pedido_default_get_all();

void monitor_pedido_default_remove(char* id_pedido);


#endif /* MONITOR_PEDIDO_DEFAULT_H_ */
