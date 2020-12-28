#include"consola-cliente.h"


static void procesar_comando(t_command* comando);


void exec_consola_cliente(){
	char* linea;
	while(1) {
		sem_wait(&sem_habilitar_consola);

		linea = readline("\ncli > ");

		if(!strncmp(linea, "Salir", 5)) {
		   free(linea);
		   break;
		}

		t_command* comando = command_create(linea);
		free(linea);

		procesar_comando(comando);
		command_destroy(comando);

		sem_post(&sem_habilitar_notificaion);
	}
}


static void procesar_comando(t_command* comando){

	if(strcmp(comando->cmd, CMD_CONSULTAR_RESTAURANTES) == 0){
		cmd_consultar_restaurantes_exec(comando);
		return;
	}
	if(strcmp(comando->cmd, CMD_CONSULTAR_PLATOS) == 0){
		cmd_consultar_platos_exec(comando);
		return;
	}
	if(strcmp(comando->cmd, CMD_CONSULTAR_PEDIDO) == 0){
		cmd_consultar_pedido_exec(comando);
		return;
	}
	if(strcmp(comando->cmd, CMD_ANIADIR_PLATO) == 0){
		cmd_aniadir_plato_exec(comando);
		return;
	}
	if(strcmp(comando->cmd, CMD_CONFIRMAR_PEDIDO) == 0){
		cmd_confirmar_pedido_exec(comando);
		return;
	}
	if(strcmp(comando->cmd, CMD_GUARDAR_PLATO) == 0){
		cmd_guardar_plato_exec(comando);
		return;
	}
	if(strcmp(comando->cmd, CMD_GUARDAR_PEDIDO) == 0){
		cmd_guardar_pedido_exec(comando);
		return;
	}
	if(strcmp(comando->cmd, CMD_CREAR_PEDIDO) == 0){
		cmd_crear_pedido_exec(comando);
		return;
	}
	if(strcmp(comando->cmd, CMD_TERMINAR_PEDIDO) == 0){
		cmd_terminar_pedido_exec(comando);
		return;
	}
	if(strcmp(comando->cmd, CMD_OBTENER_PEDIDO) == 0){
		cmd_obtener_pedido_exec(comando);
		return;
	}
	if(strcmp(comando->cmd, CMD_SELECCIONAR_RESTAURANTE) == 0){
		cmd_seleccionar_restaurante_exec(comando);
		return;
	}
	if(strcmp(comando->cmd, CMD_OBTENER_RECETA) == 0){
		cmd_obtener_receta_exec(comando);
		return;
	}
	if(strcmp(comando->cmd, CMD_OBTENER_RESTAURANTE) == 0){
		cmd_obtener_restaurante_exec(comando);
		return;
	}
	if(strcmp(comando->cmd, CMD_PLATO_LISTO) == 0){
		cmd_plato_listo_exec(comando);
		return;
	}
	if(strcmp(comando->cmd, CMD_FINALIZAR_PEDIDO) == 0){
		cmd_finalizar_pedido_exec(comando);
		return;
	}

	printf("[%s] no es un comando valido.\n", comando->cmd);

}




