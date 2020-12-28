#include "planificador-restaurante.h"

void exec_planificador_restaurante(t_obtener_restaurante_res* info_restaurante){
	t_list* afinidades = info_restaurante->afinidad;

	monitor_cocineros_init();

	lista_prioridad_horno = list_create();

	sems_cocineros_libres = dictionary_create();

	pthread_mutex_init(&mutex_cola_prioridades_horno, NULL);
	pthread_mutex_init(&mutex_cola_exit, NULL);

	cola_planificacion_init(&cola_block_reposo);
	cola_planificacion_create(cola_block_reposo, COLA_BLOCK_REPOSO);

	cola_planificacion_init(&cola_block_horno);
	cola_planificacion_create(cola_block_horno, COLA_BLOCK_HORNO);

	cola_planificacion_init(&cola_exit);
	cola_planificacion_create(cola_exit, COLA_EXIT);

	cola_planificacion_init(&cola_new);
	cola_planificacion_init(&cola_ready);
	cola_planificacion_init(&cola_exec);

	list_add(afinidades, AFINIDAD_DEFAULT);

	for(int i = 0; i < list_size(afinidades); i++){
		char* afinidad = list_get(afinidades, i);
		cola_planificacion_create(cola_new, afinidad);
		cola_planificacion_create(cola_ready, afinidad);
		cola_planificacion_create(cola_exec, afinidad);

		sem_t* sem = malloc(sizeof(sem_t));
		sem_init(sem, 0, 0);
		dictionary_put(sems_cocineros_libres, strdup(afinidad), sem);
	}

	for(int i = 0; i < info_restaurante->cant_cocineros; i++){
		char* afinidad = AFINIDAD_DEFAULT;
		if(i < list_size(afinidades)){
			afinidad = list_get(afinidades, i);
		}

		t_cocinero* cocinero = cocinero_create(string_itoa(i+1), LIBRE, afinidad);
		monitor_cocineros_put(cocinero->id_cocinero, cocinero);

		log_info(logger, "[PLANIFICADOR] - Se creo el cocinero: { id: %s, afinidad: %s }",cocinero->id_cocinero, cocinero->afinidad);

		sem_t* sem_cocinero_libre = dictionary_get(sems_cocineros_libres, afinidad);
		sem_post(sem_cocinero_libre);
	}

	pthread_t thread_block_reposo;
	pthread_create(&thread_block_reposo, NULL, (void*) cola_block_reposo_listener, NULL);
	pthread_detach(thread_block_reposo);

	pthread_t thread_block_horno;
	pthread_create(&thread_block_horno, NULL, (void*) cola_block_horno_listener, info_restaurante);
	pthread_detach(thread_block_horno);

	pthread_t thread_exit;
	pthread_create(&thread_exit, NULL, (void*) cola_exit_listener, NULL);
	pthread_detach(thread_exit);

	for(int i = 0; i < list_size(afinidades); i++){
		char* afinidad = list_get(afinidades, i);

		pthread_t thread_new;
		pthread_create(&thread_new, NULL, (void*) cola_new_listener, afinidad);
		pthread_detach(thread_new);

		pthread_t thread_ready;
		pthread_create(&thread_ready, NULL, (void*) cola_ready_listener, afinidad);
		pthread_detach(thread_ready);

		pthread_t thread_exec;
		pthread_create(&thread_exec, NULL, (void*) cola_exec_listener, afinidad);
		pthread_detach(thread_exec);
	}

	sem_post(&sem_habilitar_servidor);

}
