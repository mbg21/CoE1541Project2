//
//	CacheConfigurator.h
//
//	CoE 1541 Fall 2016
//	Project 2
//
//	2016-11-01
//	Max Garber <mbg21@pitt.edu>
//

#ifndef CACHECONFIGURATOR
#define CACHECONFIGURATOR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CC_DEBUG 0
#define CC_LINESIZE 128

const char* filepath = "CacheConfigurator/cache_config.txt";
const char mode = 'r';

typedef struct cache_config_t {
	int size_L1;
	int blocksize;
	int associativity_L1;
	int size_L2;
	int associativity_L2;
	int access_time_L2;
	int access_time_mem;
} cache_config_t;

cache_config_t* read_config_file () {
	FILE* file_ptr = fopen(filepath, &mode);
	void* line_buf = malloc(CC_LINESIZE);
	cache_config_t* configuration = malloc(sizeof(cache_config_t));
	memset(configuration, 0, sizeof(cache_config_t));
	
	if (!file_ptr || !line_buf) {
		configuration->size_L1 = -1;
		return configuration;
	}

	fgets(line_buf, CC_LINESIZE, file_ptr);
	configuration->size_L1 = atoi(line_buf);
	memset(line_buf, 0, CC_LINESIZE);
	
	fgets(line_buf, CC_LINESIZE, file_ptr);
	configuration->blocksize = atoi(line_buf);
	memset(line_buf, 0, CC_LINESIZE);
	
	fgets(line_buf, CC_LINESIZE, file_ptr);
	configuration->associativity_L1 = atoi(line_buf);
	memset(line_buf, 0, CC_LINESIZE);
	
	fgets(line_buf, CC_LINESIZE, file_ptr);
	configuration->size_L2 = atoi(line_buf);
	memset(line_buf, 0, CC_LINESIZE);
	
	fgets(line_buf, CC_LINESIZE, file_ptr);
	configuration->associativity_L2 = atoi(line_buf);
	memset(line_buf, 0, CC_LINESIZE);
	
	fgets(line_buf, CC_LINESIZE, file_ptr);
	configuration->access_time_L2 = atoi(line_buf);
	memset(line_buf, 0, CC_LINESIZE);
	
	fgets(line_buf, CC_LINESIZE, file_ptr);
	configuration->access_time_mem = atoi(line_buf);
	memset(line_buf, 0, CC_LINESIZE);
	
	if (CC_DEBUG) {
		printf("         L1 size: %i KiB\n", configuration->size_L1);
		printf("      Block size: %i B\n", configuration->blocksize);
		printf("L1 associativity: %i \n", configuration->associativity_L1);
		printf("         L2 size: %i KiB\n", configuration->size_L2);
		printf("L2 associativity: %i \n", configuration->associativity_L2);
		printf("  L2 access time: %i cycles\n", configuration->access_time_L2);
		printf(" mem access time: %i cycles\n", configuration->access_time_mem);
	}
	
	fclose(file_ptr);
	free(line_buf);
	
	return configuration;
}

#endif
//EOF
