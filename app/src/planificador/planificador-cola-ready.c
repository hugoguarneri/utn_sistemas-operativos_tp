#include "planificador-cola-ready.h"

static void planificacion_fifo();
static void planificacion_hrrn();
static void planificacion_sjf();
static void actualizar_wait_time();



void cola_ready_listener(){
	char* algoritmo = strdup(config_get_string_value(config, "ALGORITMO_DE_PLANIFICACION"));

	if(strcmp(algoritmo, "FIFO") == 0)
		planificacion_fifo();

	if(strcmp(algoritmo, "HRRN") == 0)
		planificacion_hrrn();

	if(strcmp(algoritmo, "SJF-SD") == 0)
		planificacion_sjf();

}


static void planificacion_fifo(){
	while(1){
		sem_wait(&sem_cola_ready);
		sem_wait(&sem_grado_multiprocesamiento);

		t_pcb* pcb = pcb_duplicate(cola_planificacion_get(cola_ready, 0));

		cola_planificacion_remove(cola_ready, pcb->id);
		cola_planificacion_add(cola_exec, pcb);

		log_debug(logger, "[PLANIFICADOR] - Se movio [pcb %s] a COLA EXEC - [id_pcb: %s], [id_pedido: %s], [restaurante: %s], [id_cliente: %s], [id_repartido: %s]",
				pcb->id, pcb->id, pcb->id_pedido, pcb->restaurante, pcb->id_cliente, pcb->id_repartidor);
		sem_post(&sem_cola_exec);
	}
}

static void planificacion_sjf(){
	double alpha = config_get_double_value(config, "ALPHA");

	int estimacion_(double alpha, double estAnterior, double rafAnterior){
		return (alpha * rafAnterior) + ((1-alpha) * estAnterior);
	}

	while(1){
		sem_wait(&sem_cola_ready);
		sem_wait(&sem_grado_multiprocesamiento);

		int index = 0;
		int estimacion_aux = 9999;

		for(int i = 0; i < cola_planificacion_size(cola_ready); i++){
			t_pcb* pcb = cola_planificacion_get(cola_ready, i);
			double estimacion = estimacion_(alpha, pcb->est_anterior, pcb->raf_anterior);

			log_debug(logger, "[PLANIFICADOR] - estimacion %0.2f - [pcb %s] [alpha %0.2f] [est_anterior %0.2f] [] [raf_anterior %0.2f]",
					estimacion, pcb->id, alpha, pcb->est_anterior, pcb->raf_anterior);

			estimacion_aux = (estimacion < estimacion_aux) ? estimacion : estimacion_aux;
			index = (estimacion < estimacion_aux) ? i : index;
		}

		t_pcb* pcb = pcb_duplicate(cola_planificacion_get(cola_ready, index));
		pcb->est_anterior = estimacion_aux;

		cola_planificacion_remove(cola_ready, pcb->id);
		cola_planificacion_add(cola_exec, pcb);

		log_debug(logger, "[PLANIFICADOR] - Se movio [pcb %s] a COLA EXEC - [id_pcb: %s], [id_pedido: %s], [restaurante: %s], [id_cliente: %s], [id_repartido: %s]",
						pcb->id, pcb->id, pcb->id_pedido, pcb->restaurante, pcb->id_cliente, pcb->id_repartidor);
		sem_post(&sem_cola_exec);
	}
}

static void planificacion_hrrn(){
	double alpha = config_get_double_value(config, "ALPHA");

	pthread_t thread;
	pthread_create(&thread, NULL, (void*) actualizar_wait_time, NULL);
	pthread_detach(thread);

	double estimacion_(double alpha, double estAnterior, double rafAnterior, int w_time){
		return (1 + (w_time/ (alpha * rafAnterior) + ((1-alpha) * estAnterior)));
	}

	while(1){
		sem_wait(&sem_cola_ready);
		sem_wait(&sem_grado_multiprocesamiento);


		int index = 0;
		int estimacion_aux = 9999;

		for(int i = 0; i < cola_planificacion_size(cola_ready); i++){
			t_pcb* pcb = cola_planificacion_get(cola_ready, i);
			double estimacion = estimacion_(alpha, pcb->est_anterior, pcb->raf_anterior, pcb->w_time);

			log_debug(logger, "[PLANIFICADOR] - estimacion %0.2f - [pcb %s] [alpha %0.2f] [est_anterior %0.2f] [raf_anterior %0.2f], [wait_time %d]",
					estimacion, pcb->id, alpha, pcb->est_anterior, pcb->raf_anterior, pcb->w_time);

			estimacion_aux = (estimacion < estimacion_aux) ? estimacion : estimacion_aux;
			index = (estimacion < estimacion_aux) ? i : index;
		}

		pthread_mutex_lock(&mutex_cola_ready);
		t_pcb* pcb = pcb_duplicate(cola_planificacion_get(cola_ready, index));
		pcb->est_anterior = estimacion_aux;
		pcb->w_time = 0;

		cola_planificacion_remove(cola_ready, pcb->id);
		cola_planificacion_add(cola_exec, pcb);
		pthread_mutex_unlock(&mutex_cola_ready);

		log_debug(logger, "[PLANIFICADOR] - Se movio [pcb %s] a COLA EXEC - [id_pcb: %s], [id_pedido: %s], [restaurante: %s], [id_cliente: %s], [id_repartido: %s]",
						pcb->id, pcb->id, pcb->id_pedido, pcb->restaurante, pcb->id_cliente, pcb->id_repartidor);
		sem_post(&sem_cola_exec);
	}
}

static void actualizar_wait_time(){
	int retardo_cpu = config_get_int_value(config, "RETARDO_CICLO_CPU");
	while(1){
		sleep(retardo_cpu);
		pthread_mutex_lock(&mutex_cola_ready);
		for(int i = 0; i < cola_planificacion_size(cola_ready); i++){
			t_pcb* pcb = cola_planificacion_get(cola_ready, i);
			pcb->w_time = pcb->w_time + 1;
		}
		pthread_mutex_unlock(&mutex_cola_ready);

	}
}

