#ifndef INFO_CLIENTE_H_
#define INFO_CLIENTE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <commons/string.h>


typedef struct {
	char* ip;
	char* puerto;
	char* id_cliente;
	uint32_t pos_x;
	uint32_t pos_y;
} t_info_cliente;

t_info_cliente* info_cliente_create(char* ip, char* puerto, char* id_cliente, uint32_t pos_x, uint32_t pos_y);

void info_cliente_destroy(t_info_cliente* info_cliente);


#endif /* INFO_CLIENTE_H_ */
