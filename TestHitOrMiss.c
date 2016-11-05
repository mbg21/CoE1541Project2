#include <stdio.h>
#include "original_source/cache.h"
#include "CacheConfigurator/CacheConfigurator.h"

int main(int argc, char *argv[]) {
			
	// Test Variables
	int h, index; 
	unsigned long tag2cmp;
	struct cache_t* cache; 
	cache = cache_create(1024, 1, 2, 100); 
	//printf("%d \n", cache->assoc);
	// Test 1: Access index greater than number of blocks
		
		index = 3; 
		tag2cmp = 0; 
		h = hit_or_miss(cache, index, tag2cmp); 
	
	// Test 2: Should return a hit..
	// tag in block is initally 0;  
		
		index = 0; 
		tag2cmp = 0; 
		h = hit_or_miss(cache, index, tag2cmp); 
	
	// Test 3: should return a miss..
	
		cache->blocks[index][0].tag = 4; 
		index = 0;
		tag2cmp = 0; 
		h = hit_or_miss(cache, 0, 0); 
		
	// Test 4: should return a miss
	
		index = 0; 
		tag2cmp = 100; 
		h = hit_or_miss(cache, index, tag2cmp); 

	// Test 5: Should be a hit
	// nblks was changed in the caches
//		cache = cache_create(1024, 2, 1, 100); 
//		
//		index = 1; 
//		tag2cmp = 0;
//		h = hit_or_miss(cache, index, tag2cmp); 
//	    printf("%d\n", h); 
	
}