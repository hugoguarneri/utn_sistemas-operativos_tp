#include "paquete.h"

t_paquete* paquete_create(op_code codigo_operacion, uint32_t size, void* stream) {
    t_paquete* paquete = malloc(sizeof(t_paquete));
	t_buffer* buffer = buffer_create(size, stream);

	paquete->codigo_operacion = codigo_operacion;
    paquete->buffer = buffer;

	return paquete;
}

void paquete_destroy(t_paquete* paquete) {
    buffer_destroy(paquete->buffer);
    free(paquete);
}


t_buffer* buffer_create(uint32_t size, void* stream) {
    t_buffer* buffer = malloc(sizeof(t_buffer));
    buffer->stream = malloc(size);
	memcpy(buffer->stream, stream, size);
	buffer->size = size;

	return buffer;
}

void buffer_destroy(t_buffer* buffer) {
    free(buffer->stream);
    free(buffer);
}

void log_info_message(char *action, char *msg){
	log_info(logger, "%s: %s", action, msg);
	free(msg);
}
