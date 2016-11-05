#include <stdio.h>
#include "cache.h"

int main(int argc, char *argv[]) {
	
	struct cache_t* cache = cache_create(1024, 1, 1, 100); 

	
	int h = hit_or_miss(cache, 0, 0);
	
	
	printf("%d", h); 
	
	
	
}