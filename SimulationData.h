#ifndef SIMULATIONDATA
#define SIMULATIONDATA

#include <stdio.h>

struct SimulationData {

	int index; 
	unsigned long tag; 
}; 

struct SimulationData* create_data(int index, unsigned long tag){

	struct SimulationData* data = (struct SimulationData *)calloc(1, sizeof(struct SimulationData));
	data->index = index; 
	data->tag = tag; 
	
	return data; 
}

void print_data(struct SimulationData* data){ printf("Found at Index:%i with a Tag of:%lu\n", data->index, data->tag); }

#endif