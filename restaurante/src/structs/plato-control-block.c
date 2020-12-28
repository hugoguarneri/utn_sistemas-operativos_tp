#include <structs/plato-control-block.h>

t_list* paso_tiempo_duplicate(t_list* paso_tiempo){
	t_list* paso_tiempo_aux = list_create();
	for(int i = 0; i < paso_tiempo->elements_count; i++){
		t_paso_info* paso_info = list_get(paso_tiempo, i);
		t_paso_info* paso_info_aux = malloc(sizeof(t_paso_info));
		paso_info_aux->paso = strdup(paso_info->paso);
		paso_info_aux->tiempo = paso_info->tiempo;
		list_add(paso_tiempo_aux, paso_info_aux);
	}
	return paso_tiempo_aux;
}

t_paso_info* paso_info_create(char *paso, int tiempo) {
	t_paso_info *result = calloc(sizeof(t_paso_info), 1);

	result->paso = string_duplicate(paso);
	result->tiempo = tiempo;

	return result;
}

t_pcb* pcb_create(char* id, char* id_pedido, char* id_cocinero, char* plato, t_list* pasos_tiempos, int quantum){
	t_pcb* pcb = malloc(sizeof(t_pcb));

	pcb->id = strdup(id);
	pcb->id_pedido = strdup(id_pedido);
	pcb->id_cocinero = id_cocinero;
	pcb->plato = strdup(plato);

	t_list* paso_tiempo_list = list_create();

	for(int i = 0; i < pasos_tiempos->elements_count; i++){
		t_paso_info* paso_info = list_get(pasos_tiempos, i);
		t_paso_info* paso_info_aux = paso_info_create(
				string_duplicate(paso_info->paso),
				paso_info->tiempo);

		list_add(paso_tiempo_list, paso_info_aux);
	}

	pcb->index_paso = 0;
	pcb->tiempo_paso = 0;
	pcb->pasos_tiempo = paso_tiempo_list;
	pcb->rafaga_cpu = 0;
	pcb->quantum = quantum;

	return pcb;
}

t_pcb* pcb_duplicate(t_pcb* pcb){

	t_pcb* pcb_aux = malloc(sizeof(t_pcb));

	pcb_aux->id = strdup(pcb->id);
	pcb_aux->id_pedido = strdup(pcb->id_pedido);

	if(pcb->id_cocinero != NULL)
		pcb_aux->id_cocinero = strdup(pcb->id_cocinero);
	else
		pcb_aux->id_cocinero = pcb->id_cocinero;

	pcb_aux->plato = strdup(pcb->plato);
	pcb_aux->tiempo_paso = pcb->tiempo_paso;
	pcb_aux->pasos_tiempo = paso_tiempo_duplicate(pcb->pasos_tiempo);
	pcb_aux->index_paso = pcb->index_paso;
	pcb_aux->rafaga_cpu = pcb->rafaga_cpu;
	pcb_aux->quantum = pcb->quantum;

	return pcb_aux;
}

void pasos_tiempo_destroy(t_paso_info* paso_tiempo){
	free(paso_tiempo->paso);
	free(paso_tiempo);
}

void pcb_destroy(t_pcb* pcb){
	list_destroy_and_destroy_elements(pcb->pasos_tiempo, (void*)pasos_tiempo_destroy);
	free(pcb->id);
	free(pcb->id_pedido);
	free(pcb->id_cocinero);
	free(pcb->plato);
	free(pcb);
}
