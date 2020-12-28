#ifndef OPCODE_H_
#define OPCODE_H_
#include <commons/log.h>

extern t_log* logger;

typedef enum {
	ERROR = -1,
	CONSULTAR_RESTAURANTES = 1,
	SELECCIONAR_RESTAURANTE = 2,
	OBTENER_RESTAURANTE = 3,
	CONSULTAR_PLATOS = 4,
	CREAR_PEDIDO = 5,
	GUARDAR_PEDIDO = 6,
	ANIADIR_PLATO = 7,
	GUARDAR_PLATO = 8,
	CONFIRMAR_PEDIDO = 9,
	PLATO_LISTO = 10,
	CONSULTAR_PEDIDO = 11,
	OBTENER_PEDIDO = 12,
	FINALIZAR_PEDIDO = 13,
	TERMINAR_PEDIDO = 14,
	OBTENER_RECETA = 15,
	HANDSHAKE = 16,
} op_code;


typedef enum {
	OK_ = 0,
	FAIL_ = -1,
} status_code;

typedef enum {
	Pendiente = 0,
	Confirmado = 1,
	Terminado = 2
} estado_pedido;

#endif /* OPCODE_H_ */
