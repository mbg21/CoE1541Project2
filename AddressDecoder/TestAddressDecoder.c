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
#include <string.h>
#include "AddressDecoder.h"

//	word-address access sequence from Homework 4
int access_seq2[] = {0x00000004, 0x00001001, 0x10004884, 0x7FFF8000, 0xFFFFC000};
int access_count2 = 5;

void run_accesses1(int access_sequence[], int access_count, cache_config_t* cache) {
	for(int i=0; i<access_count; i++) {
		uint32_t address = (uint32_t) access_sequence[i];
		uint32_t index = getCacheIndex(address, cache, 1);
		int32_t tag = getCacheTag(address, cache, 1);
		printf("byte_address: 0x%08X (%u) -> (index: %u, tag: %u)\n\n", address, address, index, tag);
	}
}

int main(int argc, char *argv[]) {
	printf("Testing Address Decoder Module\n");
	cache_config_t* cache1 = calloc(1, sizeof(cache_config_t));
	
	printf("Test Case 1: 4KiB, 1-word blocks, 4-way associative\n\n");
	cache1->size_L1 = 4;
	cache1->blocksize = 4;
	cache1->associativity_L1 = 4;
	run_accesses1(access_seq2, access_count2, cache1);
	
	return 0;
}

//EOF
