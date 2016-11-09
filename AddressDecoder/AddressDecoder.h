//
//	AddressDecoder.h
//
//	CoE 1541 Fall 2016
//	Project 2
//
//	2016-11-07
//	Max Garber <mbg21@pitt.edu>
//	

#ifndef ADDRESSDECODER
#define ADDRESSDECODER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../CacheConfigurator/CacheConfigurator.h"

#define AD_DEBUG 1

uint32_t getCacheIndex(uint32_t address, cache_config_t* cache_config, int tier) {
	uint32_t index;
	if (!cache_config) { index = 0; }	// null cache configuration, invalid request
	if (tier == 1) {
		//	consult the L1
		uint32_t cachesize = cache_config->size_L1;					// size in KiB
		uint32_t blocksize = cache_config->blocksize;				// size in B
		uint32_t associativity = cache_config->associativity_L1;
		uint32_t blockcount = (uint32_t) ((cachesize*1024)/blocksize);
		
		uint32_t offset_w = (uint32_t) log2(blocksize);
		uint32_t setcount = (uint32_t) (blockcount/associativity);
		uint32_t index_w = (uint32_t) log2(setcount);
		uint32_t tag_w = 32 - index_w - offset_w;
		
		if (AD_DEBUG) printf("getCacheIndex(): cachesize = %u KB, blocksize = %u B, associativity = %u\n", cachesize, blocksize, associativity);
		if (AD_DEBUG) printf("    blockcount = %u, offset_w = %u, setcount = %u, index_w = %u, tag_w = %u\n", blockcount, offset_w, setcount, index_w, tag_w);
		
		uint32_t tag = address >> (index_w + offset_w);
		
		uint32_t wordindex = address << tag_w;
		wordindex = wordindex >> (tag_w + offset_w);
		
		int32_t setindex = (int32_t) wordindex / associativity;
		
		uint32_t offset = address << (index_w + tag_w);
		offset = offset >> (index_w + tag_w);
		
		if (AD_DEBUG) printf("    wordindex = %u, setindex = %u, offset = %u, tag = %u\n", wordindex, setindex, offset, tag);
		
		index = setindex;
	} else if (tier == 2) {
		//	consult the L2
		index = 0;
	} else {
		//	invalid request
		index = 0;
	}
	return index;
}

uint32_t getCacheTag(int address, cache_config_t* cache_config, int tier) {
	uint32_t tag;
	if (!cache_config) { tag = 0; }	// null cache configuration, invalid request
	if (tier == 1) {
		//	consult the L1
		uint32_t cachesize = cache_config->size_L1;					// size in KiB
		uint32_t blocksize = cache_config->blocksize;				// size in B
		uint32_t associativity = cache_config->associativity_L1;
		uint32_t blockcount = (uint32_t) ((cachesize*1024)/blocksize);
		
		uint32_t offset_w = (uint32_t) log2(blocksize);
		uint32_t setcount = (uint32_t) (blockcount/associativity);
		uint32_t index_w = (uint32_t) log2(setcount);
		uint32_t tag_w = 32 - index_w - offset_w;
		
		//if (AD_DEBUG) printf("getCacheIndex(): cachesize = %u KB, blocksize = %u B, associativity = %u\n", cachesize, blocksize, associativity);
		//if (AD_DEBUG) printf("    blockcount = %u, offset_w = %u, setcount = %u, index_w = %u, tag_w = %u\n", blockcount, offset_w, setcount, index_w, tag_w);
		
		tag = address >> (index_w + offset_w);
		
	} else if (tier == 2) {
		//	consult the L2
		tag =  0;
	} else {
		tag = 0;	
	}
	return tag;
}

#endif
//EOF
