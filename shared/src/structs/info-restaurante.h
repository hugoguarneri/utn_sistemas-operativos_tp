#ifndef INFO_RESTAURANTE_H_
#define INFO_RESTAURANTE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <commons/string.h>


typedef struct {
	char* ip;
	char* puerto;
	char* nombre;
	uint32_t pos_x;
	uint32_t pos_y;
} t_info_restaurante;

t_info_restaurante* info_restaurante_create(char* ip, char* puerto, char* nombre, uint32_t pos_x, uint32_t pos_y);

void info_restaurante_destroy(t_info_restaurante* info_restaurante);

char* info_restaurante_to_string(t_info_restaurante* info_restaurante);


#endif /* INFO_RESTAURANTE_H_ */
