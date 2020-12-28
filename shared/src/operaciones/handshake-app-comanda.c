#include "handshake-app-comanda.h"

t_paquete* handshake_app_comanda_req_empaquetar(){
	char* stream = string_new();
	t_paquete* paquete = paquete_create(HANDSHAKE, sizeof(stream), stream);

	free(stream);
	return paquete;
}

char* handshake_app_comanda_req_to_string(){
	uint32_t size = strlen("");
	char* request_str = string_from_format("{op_code: %s, size: %d}", "HANDSHAKE", size);

	return request_str;
}

t_handshake_app_comanda_res* handshake_app_comanda_res_create(status_code code){
	t_handshake_app_comanda_res* response = malloc(sizeof(t_handshake_app_comanda_res));
	response->code = code;

	return response;
}

void handshake_app_comanda_res_destroy(t_handshake_app_comanda_res* response){
	free(response);
}

t_paquete* handshake_app_comanda_res_empaquetar(t_handshake_app_comanda_res* response){
	uint32_t size = handshake_app_comanda_res_size(response);

	void* stream = malloc(size);
	memcpy(stream, &(response->code), sizeof(response->code));

	t_paquete* paquete = paquete_create(HANDSHAKE, size, stream);

	free(stream);
	return paquete;
}

t_handshake_app_comanda_res* handshake_app_comanda_res_desempaquetar(t_paquete* paquete){
	t_handshake_app_comanda_res* response = malloc(sizeof(t_handshake_app_comanda_res));

	void* stream = paquete->buffer->stream;
	memcpy(&(response->code), stream, sizeof(response->code));

	return response;
}

char* handshake_app_comanda_res_to_string(t_handshake_app_comanda_res* response){
	char* response_str = string_from_format("{op_code: %s, size: %d, data: { code: %s }}",
			"HANDSHAKE",
			handshake_app_comanda_res_size(response),
			response->code == OK_ ? "OK" : "FAIL");

	return response_str;
}

uint32_t handshake_app_comanda_res_size(t_handshake_app_comanda_res* response){
	return sizeof(response->code);
}
