#include <stdio.h>
#include "original_source/cache.h"
#include "CacheConfigurator/CacheConfigurator.h"

int main(int argc, char *argv[]) {
		
	cache_config_t* cache_config = read_config_file(); 
	struct cache_t* cache = cache_create(1024, 1, 1, 100); 

	int index = 0;
	unsigned long tag2cmp = 1; 


	int h = hit_or_miss(cache, index, tag2cmp);
	free(cache_config);
}