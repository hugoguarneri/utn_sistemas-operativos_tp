#include "consola.h"
#include "server.h"

#define SIZE_COMANDO_CREAR_RESTO 8
#define SIZE_COMANDO_CREAR_RECETA 4
#define CMD_EXIT "quit"
#define CMD_CREAR_RESTAURANTE "crearrestaurante"
#define CMD_CREAR_RECETA "crearreceta"

void imprimir_ayuda();
char * ejecutar_crear_receta(char *nombre, char *pasos, char *tiempo_pasos);
char* ejecutar_crear_restaurante(char *nombre, char *cantidad_cocineros, char *posicion, char *afinidad, char *platos,
		char *precio_platos, char *cantidad_hornos);
void imprimir_resultado(char *resultado_comando);

void* exec_consola(void *ptr) {
	log_debug(logger, "Iniciando consola");

	while (1) {
		char *leido = NULL;
		char *resultado_comando = NULL;
		char **comando = NULL;
		leido = readline("\nsd >");
		log_info(logger, "Comando leido: %s", leido);

		if (!leido)
			break;

		comando = string_split(leido, " \t\n");

		if (strcasecmp(comando[0], CMD_EXIT) == 0) {
			log_info(logger, "Cerrando sindicato");
			exit(0);
		}

		int count = split_size(comando);

		if (count != SIZE_COMANDO_CREAR_RECETA && count != SIZE_COMANDO_CREAR_RESTO) {
			imprimir_ayuda();
			free_string_array(comando);
			free(leido);
			continue;
		}

		// Interpreto comandos
		if (strcasecmp(comando[0], CMD_CREAR_RECETA) == 0) {
			// Si es comando crear receta
			resultado_comando = (char *)ejecutar_crear_receta(comando[1], comando[2], comando[3]);
			imprimir_resultado(resultado_comando);

		} else if (strcasecmp(comando[0], CMD_CREAR_RESTAURANTE) == 0) {
			// Si es comando crear restaurante
			resultado_comando = (char *)ejecutar_crear_restaurante(comando[1], comando[2], comando[3], comando[4], comando[5],
					comando[6], comando[7]);
			imprimir_resultado(resultado_comando);
		} else {
			imprimir_ayuda();
		}
		if(comando != NULL)
			free_string_array(comando);
		if (leido != NULL)
			free(leido);


	}
	log_info(logger, "No se pudo leer, saliendo de la lectura");

	return NULL;
}

void imprimir_resultado(char *resultado_comando) {
	printf("Resultado: %s", resultado_comando);
}

void imprimir_ayuda() {
	printf(
			"Comando ingresado no válido \n CrearReceta [NOMBRE] [PASOS] [TIEMPO_PASOS] \n CrearRestaurante [NOMBRE] [CANTIDAD_COCINEROS] [POSICION] [AFINIDAD_COCINEROS] [PLATOS] [PRECIO_PLATOS] [CANTIDAD_HORNOS]\n");
}



