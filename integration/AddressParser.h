//
//	CoE 1541 Fall 2016 Project 2
//	Max Garber <mbg21@pitt.edu> & Adrew Lendacky <anl119@pitt.edu>
//

#ifndef ADDRESSPARSER
#define ADDRESSPARSER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "CacheConfigurator.h"

extern cache_config_t* cache_config;

#define AD_DEBUG 0

uint32_t getCacheIndex(uint32_t address,  cache_config_t*  cache_config, int  tier) { 
	uint32_t  index;
	if  (cache_config == NULL)  {
		index  = 0;
	}  // null  cache configuration, invalid request
	if (tier == 1) {
		//	consult the L1
		uint32_t cachesize = cache_config->size_L1; // size in KiB
		uint32_t blocksize = cache_config->blocksize; // size in B
		uint32_t associativity = cache_config->associativity_L1;
		uint32_t nblocks = (uint32_t) ((cachesize*1024)/blocksize);
		uint32_t nsets = (uint32_t) (nblocks/associativity);
		// compute exponents 
		uint32_t cachesize_log = (uint32_t) log2(cachesize*1024); 
		uint32_t blocksize_log = log2(blocksize);
		uint32_t nblocks_log = (uint32_t) log2(nblocks);
		uint32_t nsets_log = (uint32_t) log2(nsets);
		// compute bit widths
		uint32_t index_w = nsets_log; 
		uint32_t tag_w = 32 - nsets_log - blocksize_log;
		uint32_t offset_w = blocksize_log; 
		// compute the index value
		index = (uint32_t) address % nsets; 
		// compute value for tag
		uint32_t tag = address >> (index_w + offset_w);
		// compute offset value
		uint32_t offset = address << (index_w + tag_w);
		offset = offset >> (index_w + tag_w);
		
		// if (AD_DEBUG) printf("Cachesize: %u, Blocksize: %u, Number Blocks:%u, Number of Sets:%u\n", cachesize*1024, blocksize, nblocks, nsets);
		// if (AD_DEBUG) printf("Cachesize Log: %u, Blocksize Log: %u, Number Blocks Log:%u, Number of Sets Log:%u\n", cachesize_log, blocksize_log, nblocks_log, nsets_log);
		// if (AD_DEBUG) printf("Tag Bits = %u bits\n", tag_w);
		// if (AD_DEBUG) printf("    index = %u, offset = %u, tag = %u\n", index, offset, tag);
	} 
	else if (tier == 2) {
		//	consult the L1
		uint32_t cachesize = cache_config->size_L2; // size in KiB
		uint32_t blocksize = cache_config->blocksize; // size in B
		uint32_t associativity = cache_config->associativity_L2;
		uint32_t nblocks = (uint32_t) ((cachesize*1024)/blocksize);
		uint32_t nsets = (uint32_t) (nblocks/associativity);
		// compute exponents 
		uint32_t cachesize_log = (uint32_t) log2(cachesize*1024); 
		uint32_t blocksize_log = log2(blocksize);
		uint32_t nblocks_log = (uint32_t) log2(nblocks);
		uint32_t nsets_log = (uint32_t) log2(nsets);
			
		// compute bit widths
		uint32_t index_w = nsets_log; 
		uint32_t tag_w = 32 - nsets_log - blocksize_log;
		uint32_t offset_w = blocksize_log; 
				
		// compute the index value
		index = (uint32_t) address % nsets; 
		
		// compute value for tag
		uint32_t tag = address >> (index_w + offset_w);
		
		// compute offset value
		uint32_t offset = address << (index_w + tag_w);
		offset = offset >> (index_w + tag_w);
	}
	else {
		//	invalid request
		index = 0;
	}
	return index;
}

uint32_t getCacheTag(uint32_t address, cache_config_t* cache_config, int tier) {
	uint32_t tag;
	if (cache_config == NULL) {
		tag = 0;
	}	// null cache configuration, invalid request
	if (tier == 1) {
		//	consult the L1
		uint32_t cachesize = cache_config->size_L1; // size in KiB
		uint32_t blocksize = cache_config->blocksize; // size in B
		uint32_t associativity = cache_config->associativity_L1;
		uint32_t nblocks = (uint32_t) ((cachesize*1024)/blocksize);
		uint32_t nsets = (uint32_t) (nblocks/associativity);
		// compute exponents 
		uint32_t cachesize_log = (uint32_t) log2(cachesize*1024); 
		uint32_t blocksize_log = log2(blocksize);
		uint32_t nblocks_log = (uint32_t) log2(nblocks);
		uint32_t nsets_log = (uint32_t) log2(nsets);
			
		// compute bit widths
		uint32_t index_w = nsets_log; 
		uint32_t tag_w = 32 - nsets_log - blocksize_log;
		uint32_t offset_w = blocksize_log; 
				
		// compute the index value
		uint32_t index = (uint32_t) address % nsets; 
		
		// compute value for tag
		tag = address >> (index_w + offset_w);
		
		// compute offset value
		uint32_t offset = address << (index_w + tag_w);
		offset = offset >> (index_w + tag_w);
		
		//if (AD_DEBUG) printf("Cachesize: %u, Blocksize: %u, Number Blocks:%u, Number of Sets:%u\n", cachesize*1024, blocksize, nblocks, nsets);
		//if (AD_DEBUG) printf("Cachesize Log: %u, Blocksize Log: %u, Number Blocks Log:%u, Number of Sets Log:%u\n", cachesize_log, blocksize_log, nblocks_log, nsets_log);
		//if (AD_DEBUG) printf("Tag Bits = %u bits\n", tag_w);
		//if (AD_DEBUG) printf("    index = %u, offset = %u, tag = %u\n", index, offset, tag);
		
	}
	else if (tier == 2) {
		//	consult the L1
		uint32_t cachesize = cache_config->size_L2; // size in KiB
		uint32_t blocksize = cache_config->blocksize; // size in B
		uint32_t associativity = cache_config->associativity_L2;
		uint32_t nblocks = (uint32_t) ((cachesize*1024)/blocksize);
		uint32_t nsets = (uint32_t) (nblocks/associativity);
		// compute exponents 
		uint32_t cachesize_log = (uint32_t) log2(cachesize*1024); 
		uint32_t blocksize_log = log2(blocksize);
		uint32_t nblocks_log = (uint32_t) log2(nblocks);
		uint32_t nsets_log = (uint32_t) log2(nsets);
			
		// compute bit widths
		uint32_t index_w = nsets_log; 
		uint32_t tag_w = 32 - nsets_log - blocksize_log;
		uint32_t offset_w = blocksize_log; 
				
		// compute the index value
		uint32_t index = (uint32_t) address % nsets; 
		
		// compute value for tag
		tag = address >> (index_w + offset_w);
		
		// compute offset value
		uint32_t offset = address << (index_w + tag_w);
		offset = offset >> (index_w + tag_w);
	}
	else {
		tag = 0;	
	}
	return tag;
}

#endif
//EOF
