#include "platos-comanda.h"

/*
 * platosComanda.c
 *
 *  Created on: 20 oct. 2020
 *      Author: utnso
 */

t_plato_comanda* crear_plato_comanda(char* plato,uint32_t cantidad){
	t_plato_comanda* miPlato = malloc(FRAME_SIZE);
	miPlato->cantidad = cantidad;
	miPlato->cantidad_lista = 0;
	strcpy(miPlato->plato, plato);
	return miPlato;
}

void destroy_plato_comanda(t_plato_comanda* miPlato){
	free(miPlato->plato);
	free(miPlato);
}







