#include <stdio.h>
#include "CacheSimulator.h"
#include "SimulationData.h"

int main(int argc, char *argv[]) {
	
	//const char* filepath = "CacheConfigurator/cache_config.txt";
	//struct cache_t* cache = file2Cache(filepath); 
	srand((unsigned)time(NULL)); 
	struct cache_t* cache = cache_create(1024, 1, 2, 10);
	int num_data = 5; 
	int num_sets = 1000; 
	struct SimulationData** data = (struct SimulationData **)calloc(num_data, sizeof(struct SimulationData));


	for (int i = 0; i < num_data; i++) {
		
		// create index for data
		int ind = rand() % num_sets ;
		unsigned long tag = rand();
			
		// create tag for index
		data[i] = (struct SimulationData *)calloc(1, sizeof(struct SimulationData)); 
		data[i] = create_data(ind, tag); 
		
		if (CS_DEBUG){
			//print_data(data[i]); 
		}
	}
	
	//read_tag(cache, 0, 0);
	hit_or_miss(cache, 0, 100); // miss --> not in cache
	write_tag(cache, 0, 1, 3);  // put 3 in a cache slot: index-0, way-1
	hit_or_miss(cache, 0, 3); // hit --> 3 found in cache
	
	write_tag(cache, 0, 0, 100); // put 100 in cache slot: index-0, way-0
	hit_or_miss(cache, 0, 3); // hit --> 3 is found in index-0, way-1
	hit_or_miss(cache, 0, 100); // hit --> 100 found in index-0, way-0
		
		
		
	//printf("%d\n",cache->blocks[0][0].valid );
	//printf("%d\n", is_valid(cache, 0, 3)); 
	
	printf("%d\n", set_valid_bit(cache, 0, 3, 1));
	printf("%d\n", is_valid(cache, 0, 3));
	printf("%d\n", cache->blocks[0][1].valid); 
		
//	write_tag(cache, 0, 0, 200); 
//	write_tag(cache, 0, 2, 3);
//	write_tag(cache, 0, 1, 10000);   
	//read_tag(cache, 0, 1);
}