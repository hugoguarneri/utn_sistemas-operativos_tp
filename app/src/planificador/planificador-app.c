#include "planificador-app.h"

void exec_planificador_app(){
	log_info(logger, "[APP] - Inicializando planificador..");

	cola_planificacion_init(&cola_new);
	cola_planificacion_init(&cola_ready);
	cola_planificacion_init(&cola_exec);
	cola_planificacion_init(&cola_block);
	cola_planificacion_init(&cola_exit);

	char* repartidores_posicion_arr = strdup(config_get_string_value(config, "REPARTIDORES"));
	char* repartidores_frequencia_descanso_arr = strdup(config_get_string_value(config, "FRECUENCIA_DE_DESCANSO"));
	char* repartidores_tiempo_descanso_arr = strdup(config_get_string_value(config, "TIEMPO_DE_DESCANSO"));

	t_list* repartidores_posicion = list_from_string_array(repartidores_posicion_arr);
	t_list* repartidores_frequencia_descanso = list_from_string_array(repartidores_frequencia_descanso_arr);
	t_list* repartidores_tiempo_descanso = list_from_string_array(repartidores_tiempo_descanso_arr);

	char* algoritmo = strdup(config_get_string_value(config, "ALGORITMO_DE_PLANIFICACION"));
	log_info(logger, "[PLANIFICADOR] - Se va a ejecutar con con el algoritmo %s", algoritmo);
	free(algoritmo);

	for(int i = 0; i < repartidores_posicion->elements_count; i++){
		int pos_x = atoi(string_split((char*)list_get(repartidores_posicion, i), "|")[0]);
		int pos_y = atoi(string_split((char*)list_get(repartidores_posicion, i), "|")[1]);
		int f_descanso = atoi(list_get(repartidores_frequencia_descanso, i));
		int t_descanso = atoi(list_get(repartidores_tiempo_descanso, i));

		t_repartidor* repartidor = repartidor_create(string_itoa(i+1), pos_x, pos_y, t_descanso, f_descanso, LIBRE);
		monitor_repartidores_put(repartidor->id_repartidor, repartidor);

		log_info(logger, "[PLANIFICADOR] - Se creo el repartidor: { id: %s, pos_x: %d, pos_y: %d, tiempo_descanso: %d, tiempo_actividad: %d }",
				repartidor->id_repartidor, repartidor->pos_x, repartidor->pos_y, repartidor->tiempo_de_descanso,  repartidor->tiempo_de_actividad);
		sem_post(&sem_repartidores_libres);
	}

	pthread_create(&thread_planificador_cola_new, NULL, (void*) cola_new_listener, NULL);
	pthread_detach(thread_planificador_cola_new);

	pthread_create(&thread_planificador_cola_ready, NULL, (void*) cola_ready_listener, NULL);
	pthread_detach(thread_planificador_cola_ready);

	pthread_create(&thread_planificador_cola_exec, NULL, (void*) cola_exec_listener, NULL);
	pthread_detach(thread_planificador_cola_exec);

	pthread_create(&thread_planificador_cola_block, NULL, (void*) cola_block_listener, NULL);
	pthread_detach(thread_planificador_cola_block);

	pthread_create(&thread_planificador_cola_exit, NULL, (void*) cola_exit_listener, NULL);
	pthread_detach(thread_planificador_cola_exit);

	sem_post(&sem_habilitar_servidor);
}

