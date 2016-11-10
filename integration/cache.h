//
//	CoE 1541 Fall 2016 Project 2
//	Max Garber <mbg21@pitt.edu> & Adrew Lendacky <anl119@pitt.edu>
//

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "CacheConfigurator.h"
#include "AddressParser.h"

#define CACHEDEBUG 1
#define HM_DEBUG 1
#define CS_DEBUG 0

extern cache_config_t* cache_config;
extern unsigned int cycle_number;

struct cache_blk_t {
	unsigned long tag;
	char valid;
	char dirty;
	unsigned long long ts;	//a timestamp that may be used to implement LRU replacement
	// To guarantee that L2 is inclusive of L1, you may need an additional flag 
	// in L2 to indicate that the block is cached in L1
};

struct cache_t {
	// The cache is represented by a 2-D array of blocks. 
	// The first dimension of the 2D array is "nsets" which is the number of sets (entries)
	// The second dimension is "assoc", which is the number of blocks in each set.
	int nsets;						// # sets
	int blocksize;					// block size
	int assoc;						// associativity
	int hit_latency;				// latency in case of a hit
	struct cache_blk_t **blocks;	// the array of cache blocks
};


//	size is provided in KiB - this is passed the cache_config_t parameters, it should not read the external global
struct cache_t* cache_create(int size, int blocksize, int assoc, int latency) {
	int i;
	int nblocks = 1;		// number of blocks in the cache
	int nsets = 1;			// number of sets (entries) in the cache

	// YOUR JOB: calculate the number of sets and blocks in the cache
	//
	// nblocks = X;
	// nsets = Y;
	
	//	nblocks = cachesize(KiB)*1024 / blocksize(B)
	//	nsets = nblocks / associativity
	nblocks = size*1024 / blocksize;
	nsets = nblocks / assoc;

	struct cache_t *C = (struct cache_t *)calloc(1, sizeof(struct cache_t));
		
	C->nsets = nsets; 
	C->assoc = assoc;
	C->hit_latency = latency;

	C->blocks= (struct cache_blk_t **)calloc(nsets, sizeof(struct cache_blk_t));

	for(i = 0; i < nsets; i++) {
		C->blocks[i] = (struct cache_blk_t *)calloc(assoc, sizeof(struct cache_blk_t));
	}

	return C;
}

void print_hit_or_miss(int hit_or_miss_i, uint32_t index, int way, uint32_t cache_tag, uint32_t tag2cmp) {
	if (hit_or_miss_i == 1) {
		printf("Hit!\tIndex:%04d\t\tWay:%02d\t\t\tTag in Cache:%08u\t\tTag to Find:%08u\t\tNumerical Value:%d\n", index, way, cache_tag, tag2cmp, hit_or_miss_i);
	} else if (hit_or_miss_i == 0) {
		printf("Miss!\tIndex:%04d\t\t-----\t\t\t--------------\t\tTag to Find:%08u\t\tNumerical Value:%d\n", index, tag2cmp, hit_or_miss_i); 
	} else {
		printf("Neither hit or miss. This shouldn't happen.");
	}
}

int hit_or_miss(struct cache_t* cp, uint32_t index, uint32_t tag2cmp) {
	struct cache_blk_t* blocks = cp->blocks[index]; // get blocks from cache
	struct cache_blk_t block_from_way;
	unsigned long tag; 
	int way; 
	int result; 
	
	// unsigned long cant be zero..
	// so just check for max size error
	if (index > cp->nsets - 1) {
		printf("Index exceeds the number of sets in the cache. Returning -1.\n");
		return -1; // -1 means error...we should check for this
	} else {
		// search each way..
		for (way = 0; way < cp->assoc; way++) {
			 block_from_way = blocks[way]; // get all the blocks from that index
			 tag = block_from_way.tag; // get tag from cache
			// compare tags
			if (tag == tag2cmp) { 
					result = 1; 
					if(HM_DEBUG) { print_hit_or_miss(result, index, way, tag, tag2cmp ); } 
					return 1; // hit
			} else if (tag != tag2cmp) { 
					// go to next way
			} else { printf("Something went wrong. Returning -1\n"); return -1;}
		}
		result = 0; 
		if(HM_DEBUG) {print_hit_or_miss(result, index, tag, way, tag2cmp ); }
		return result; // we searched each way and didn't find anything...miss.
	}
		
}

void write_tag(struct cache_t* cp, uint32_t index, int assoc, uint32_t tag2write){	
	if (assoc > cp->assoc - 1|| assoc < 0) {
		printf("Cache is %i-way associative, %i is out of bounds.\n", cp->assoc, assoc); 
	} else {
		if (CS_DEBUG) { printf("Current Tag at (Index:%d, Way:%d)-->%lu, will be written with %u\n", index, assoc, cp->blocks[index][assoc].tag, tag2write); }
		cp->blocks[(int) index][assoc].tag = tag2write;
		
		//if (CS_DEBUG) { printf("Tag at (Index:%d, Way:%d) overwritten with %lu\n", index, assoc, cp->blocks[index][assoc].tag); read_tag(cp, index, assoc); }
	}
}


int set_valid_bit(struct cache_t* cp, uint32_t index, uint32_t tag2find, char valid_bit) {
  
  // get the block from the specific set
  struct cache_blk_t* blocks = cp->blocks[index]; 
  
  // search those blocks for the given tag
  for (int i = 0; i < cp->assoc; i++){
    
    struct cache_blk_t block_from_way = blocks[i]; 
    unsigned long tag = block_from_way.tag; 
    
    // if tag is found: 
    // check that block and see if bit is valid
    if (tag == tag2find){ cp->blocks[index][i].valid = valid_bit; return 0; } // set the valid bit to the specified bit
    else {  } // check next way..
  }
  
  // we went through the whole set and couldn't find the tag
  // thus, we can't change the bit
  return -1;
}

// Sets the valid bit of some block at a given index for a given tag to the specified dirty input. 
// Returns 0 if the bit was successfully changed, and -1 if bit was not found in the cache set.
int set_dirty_bit(struct cache_t* cp, uint32_t index, uint32_t tag2find, char dirty_bit) {
  
  // get the block from the specific set
  struct cache_blk_t* blocks = cp->blocks[index]; 
  
  // search those blocks for the given tag
  for (int i = 0; i < cp->assoc; i++){
    
    struct cache_blk_t block_from_way = blocks[i]; 
    unsigned long tag = block_from_way.tag; 
    
    // if tag is found: 
    // check that block and see if bit is valid
    if (tag == tag2find){ cp->blocks[index][i].dirty = dirty_bit; return 0; } // set the valid bit to the specified bit
    else {  } // check next way..
  }
  
  // we went through the whole set and couldn't find the tag
  // thus, we can't change the bit 
  return -1;
}


//	this will need to read the external global
int cache_access(struct cache_t* cp, unsigned long address, char access_type, unsigned long long now, struct cache_t* next_cp)
{
	//	Try to get from the L1
	uint32_t ui32_address = (uint32_t) address;
	uint32_t cache_index = getCacheIndex(ui32_address, cache_config, 1);
	uint32_t address_tag = getCacheTag(ui32_address, cache_config, 1);
	//printf("--CACHE REQUEST: address %u  ->  index: %u, tag: %u\n", ui32_address, cache_index, address_tag);
	
	//	Check hit/miss
	int L1hit = hit_or_miss(cp, cache_index, address_tag);
	
	if (L1hit == 1) {
		// update the last accession value
		unsigned long long access_time = (unsigned long long) cycle_number;
		printf(" -- access time: %llu\n", access_time);
	} else if (L1hit == 0) {
		// L1 miss--is there an L2?
		if (cache_config->size_L2 != 0) {
			//	try the L2…
		} else {
			// no L2
			// find empty slot in the set
			int empty_way = -1;
			for(int i=0; i<cp->assoc; i++) {
				if (cp->blocks[(int) cache_index][i].valid == 0) {
					empty_way = i;
					break;
				}
			}
			// if no emtpy slot, evict LRU
			if (empty_way == -1) {
				// LRU eviction
				int lru_way = 0;
				for(int i = 0; i < cp->assoc; i++) {
					if ( cp->blocks[(int) cache_index][i].ts < cp->blocks[(int) cache_index][lru_way].ts ) {
						lru_way = i;
					}
				}
				// evict
				//	what do we do other than null the entry? write back?
				cp->blocks[(int) cache_index][lru_way].tag = 0;
				cp->blocks[(int) cache_index][lru_way].valid = 0;
				cp->blocks[(int) cache_index][lru_way].ts = 0;
				empty_way = lru_way;
			}
			//	place the block into the cache
			cp->blocks[(int) cache_index][empty_way].tag = address_tag;
			cp->blocks[(int) cache_index][empty_way].valid = 1;
			if(access_type == 'w') {
				cp->blocks[(int) cache_index][empty_way].dirty = 1;
			} else if (access_type == 'r') {
				cp->blocks[(int) cache_index][empty_way].dirty = 0;
			} else {
				// problem
			}
			cp->blocks[(int) cache_index][empty_way].ts = (unsigned long long) cycle_number;
			
			
			
			// return memory latency
			return cache_config->access_time_mem;
		}
	} else {
		// something has gone very wrong
		printf("an internal error has occurred.\n");
	}
	
	return(cp->hit_latency);
}

//EOF
