#include "sockets.h"

int crear_conexion (char* ip, char* puerto) {
	struct addrinfo hints;
	struct addrinfo *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &servinfo);

	int socket_cliente = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	if (connect(socket_cliente, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		log_error(logger, "Ocurrio un error al intentar establecer la conexion con %s:%s -> %s", ip, puerto, strerror(errno));
		socket_cliente = -1;
	}

	freeaddrinfo(servinfo);
	return socket_cliente;
}

void liberar_conexion (int socket) {
	if (close(socket) == -1) {
		log_error(logger, "Ocurrio un error al intentar liberar la conexion socket: %d -> %s", socket, strerror(errno));
	}
}

int iniciar_servidor (char *ip, char* puerto) {
	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &servinfo);

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			log_error(logger, "Ocurrio un error al intentar abrir el socket -> %s", strerror(errno));
			continue;
		}

		int flag = 1;
		setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

		if(bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
			log_error(logger, "Ocurrio un error al intentar bindear puerto -> %s", strerror(errno));
			close(socket_servidor);
			socket_servidor = -1;
			continue;
		}
		break;
	}

	listen(socket_servidor, SOMAXCONN);
	freeaddrinfo(servinfo);

	return socket_servidor;
}


int esperar_cliente_ (int socket_servidor) {
	struct sockaddr_in addr_cliente;
	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &addr_cliente, (void*) &tam_direccion);

	return socket_cliente;
}

void esperar_cliente (int socket_servidor, void* operacion_a_ejecutar) {
	struct sockaddr_in addr_cliente;
	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &addr_cliente, (void*) &tam_direccion);

	if (socket_cliente > 0) {
		pthread_create(&thread,NULL,(void*)operacion_a_ejecutar,&socket_cliente);
		pthread_detach(thread);
	}
}

t_paquete* recibir_mensaje (int socket) {
	int codigo_operacion;
	if (recv(socket, &codigo_operacion, sizeof(int), MSG_WAITALL) <= 0) {
		log_error(logger, "Ocurrio un error al recibir op_code");
		codigo_operacion = ERROR;
	}

	uint32_t size;
	if (recv(socket, &size, sizeof(uint32_t), MSG_WAITALL) <= 0) {
		log_error(logger, "Ocurrio un error al recibir size");
		codigo_operacion = ERROR;
	}

	void* stream = malloc(size);
	if (recv(socket, stream, size, MSG_WAITALL) <= 0) {
		log_error(logger, "Ocurrio un error al recibir stream");
		codigo_operacion = ERROR;
	}

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));

	paquete->codigo_operacion = codigo_operacion;
	paquete->buffer->stream = stream;
	paquete->buffer->size = size;

	return paquete;
}

int enviar_mensaje (int socket, t_paquete* paquete) {

	int size_paquete_serializado = sizeof(paquete->codigo_operacion) + sizeof(paquete->buffer->size) + paquete->buffer->size;

	void *paquete_serializado = malloc(size_paquete_serializado);
	int bytes_escritos = 0;

	memcpy(paquete_serializado + bytes_escritos, &(paquete->codigo_operacion), sizeof(paquete->codigo_operacion));
	bytes_escritos += sizeof(paquete->codigo_operacion);

	memcpy(paquete_serializado + bytes_escritos, &(paquete->buffer->size), sizeof(paquete->buffer->size));
	bytes_escritos += sizeof(paquete->buffer->size);

	memcpy(paquete_serializado + bytes_escritos, paquete->buffer->stream, paquete->buffer->size);
	bytes_escritos += paquete->buffer->size;

	int status = send(socket, paquete_serializado, size_paquete_serializado, 0);

	free(paquete_serializado);
	return status;
}









