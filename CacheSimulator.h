
#ifndef CACHESIMULATOR
#define CACHESIMULATOR


#include "CacheConfigurator/CacheConfigurator.h"
#include "original_source/cache.h"
#include "SimulationData.h"

#define CS_DEBUG 0

void read_tag(struct cache_t* cp, int index, int assoc){
	
	int i; 
	
	printf("Tag at (Index:%d, Way:%i)-->%lu\n", index, assoc, cp->blocks[index][assoc].tag); 
}

void write_tag(struct cache_t* cp, int index, int assoc, unsigned long tag2write){	
	int i; 
	
	if (assoc > cp->assoc - 1|| assoc < 0){
		
		printf("Cache is %i-way associative, %i is out of bounds.\n", cp->assoc, assoc); 
	}else {

		if (CS_DEBUG){ printf("Current Tag at (Index:%d, Way:%d)-->%lu, will be written with %lu\n", index, assoc, cp->blocks[index][assoc].tag, tag2write); }
		cp->blocks[index][assoc].tag = tag2write; 
		if (CS_DEBUG){ printf("Tag at (Index:%d, Way:%d) overwritten with %lu\n", index, assoc, cp->blocks[index][assoc].tag); read_tag(cp, index, assoc); } 
	}
}

struct cache_t* file2Cache(char* filepath){
	
	struct cache_config_t* config = read_config_file(filepath); 
	
	int size_L1 = config->size_L1;
	int blocksize = config->blocksize; 
	int associativity_L1 = config->associativity_L1; 
	int size_L2 = config->size_L2; 
	int associativity_L2 = config->associativity_L2; 
	int access_time_L2 = config->access_time_L2; 
	int access_time_mem = config->access_time_mem; 
	
	struct cache_t* cp = cache_create(size_L1, blocksize, associativity_L1, 10);
	free(config); 
	return cp; 
}


void detect_hit_or_miss(struct cache_t* cp, int index, unsigned long tag){
	
	int result = hit_or_miss(cp, index, tag);

}


#endif
