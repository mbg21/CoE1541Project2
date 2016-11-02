//
//	TestCacheConfigurator.h
//
//	CoE 1541 Fall 2016
//	Project 2
//
//	2016-11-01
//	Max Garber <mbg21@pitt.edu>
//

#include <stdio.h>
#include <stdlib.h>

#include "CacheConfigurator.h"

int main (int argc, const char* argv[]) {
	printf("reading from path ./cache_config.txt:\n");	
	cache_config_t* cache_config = read_config_file();
	free(cache_config);
	return 0;
}

//EOF
