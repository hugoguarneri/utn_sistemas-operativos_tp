#ifndef PAQUETE_H_
#define PAQUETE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "opcode.h"

typedef struct {
    uint32_t size;
    void* stream;
} t_buffer;

typedef struct {
	op_code codigo_operacion;
    t_buffer* buffer;
} t_paquete;

t_paquete* paquete_create(op_code codigo_operacion, uint32_t size, void* stream);

void paquete_destroy(t_paquete* paquete);

t_buffer* buffer_create(uint32_t size, void* stream);

void buffer_destroy(t_buffer* buffer);

#define MSG_INFO_ENVIO "ENVIO"
#define MSG_INFO_RECIBO "RECIBO"
void log_info_message(char *action, char *msg);


#endif /* PAQUETE_H_ */
