//
//	CoE 1541 Fall 2016 Project 2
//	Max Garber <mbg21@pitt.edu> & Adrew Lendacky <anl119@pitt.edu>
//

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>

#include "CacheConfigurator.h"
#include "AddressParser.h"

#define CACHEDEBUG 1
#define HM_DEBUG 1
#define CS_DEBUG 1

extern cache_config_t* cache_config;
extern unsigned int accesses;
extern unsigned int read_accesses;
extern unsigned int write_accesses;
extern unsigned int L1hits;
extern unsigned int L1misses;
extern unsigned int L2hits;
extern unsigned int L2misses;


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

void print_hit_or_miss(char access_type, int hit_or_miss_i, uint32_t index, int way, uint32_t cache_tag, uint32_t tag2cmp) {
	if (hit_or_miss_i == 1) {
		printf("Hit!  (%c)\tIndex:%04d\t\tWay:%02d\t\t\tTag in Cache:%08u\t\tTag to Find:%08u\t\tNumerical Value:%d\n", access_type, index, way, cache_tag, tag2cmp, hit_or_miss_i);
	} else if (hit_or_miss_i == 0) {
		printf("Miss! (%c)\tIndex:%04d\t\t–––––\t\t\t––––––––––––––––––––\t\tTag to Find:%08u\t\tNumerical Value:%d\n", access_type,index, tag2cmp, hit_or_miss_i); 
	} else {
		printf("Neither hit or miss. This shouldn't happen.");
	}
}

int hit_or_miss(struct cache_t* cp, uint32_t index, uint32_t tag2cmp, char access_type) {
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
			if (tag == tag2cmp && block_from_way.valid == 1) { 
					result = 1; 
					if(HM_DEBUG) { print_hit_or_miss(access_type, result, index, way, tag, tag2cmp); } 
					return 1; // hit
			} else if (tag != tag2cmp) { 
					// go to next way
			} else { printf("Something went wrong. Returning -1\n"); return -1;}
		}
		result = 0; 
		if(HM_DEBUG) {print_hit_or_miss(access_type, result, index, tag, way, tag2cmp ); }
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



// Based on address, determine the set to access in cp and examine the blocks
// in the set to check hit/miss and update the golbal hit/miss statistics
// If a miss, determine the victim in the set to replace (LRU). Replacement for 
// L2 blocks should observe the inclusion property.
//
// The function should return the hit_latency in case of a hit. In case
// of a miss, you need to figure out a way to return the time it takes to service 
// the request, which includes writing back the replaced block, if dirty, and bringing 
// the requested block from the lower level (from L2 in case of L1, and from memory in case of L2).
// This will require one or two calls to cache_access(L2, ...) for misses in L1.
// It is recommended to start implementing and testing a system with only L1 (no L2). Then add the
// complexity of an L2.

//	NEED A FLOWCHART FOR L1 H/M R/W; L2 H/M R/W…

int findOpenBlockAtIndex(struct cache_t* cp, int index) {
	int a = -1;
	for(int i=0; i<cp->assoc; i++) {
		if (cp->blocks[index][i].valid == 0) {
			a = i;
			break;
		}
	}
	return a;
}

int evictLRUblockAtIndex(struct cache_t* cp, uint32_t ui32_address, int index, struct cache_t* next_cp) {
	int lru_aindex = -1;
	unsigned long long lru_ts_min = ULLONG_MAX;
	for (int i=0; i<cp->assoc; i++) {
		if ((cp->blocks[index][i].ts < lru_ts_min) && (cp->blocks[index][i].valid == 1)) {
			lru_ts_min = cp->blocks[index][i].ts;
			lru_aindex = i;
		}
	}
	
	// case 1: evict from L1, no L2
	if (cache_config->size_L2 == 0) {
		cp->blocks[index][lru_aindex].valid = 0;
		cp->blocks[index][lru_aindex].tag = 0;
		cp->blocks[index][lru_aindex].dirty = 0;
		cp->blocks[index][lru_aindex].ts = 0;
	}
	else if (cache_config->size_L2 != 0 && next_cp != NULL) {
		// case 2: evict from L1, have L2
		printf("evicting from L1 to L2\n");
		int l2cache_index = getCacheIndex(ui32_address, cache_config, 2);
		uint32_t l2address_tag = getCacheTag(ui32_address, cache_config, 2);
		
		int l2_aindex = findOpenBlockAtIndex(next_cp, l2cache_index);
		if (l2_aindex == -1) {
			// need to LRU evict on L2
			l2_aindex = evictLRUblockAtIndex(next_cp, ui32_address, l2cache_index, NULL);
		}
		// transfer to L2
		//next_cp->blocks[l2cache_index][l2_aindex].tag = l2address_tag;
		//next_cp->blocks[l2cache_index][l2_aindex].valid = 1;
		//next_cp->blocks[l2cache_index][l2_aindex].ts = cp->blocks[index][lru_aindex].ts;
		next_cp->blocks[l2cache_index][l2_aindex].dirty = cp->blocks[index][lru_aindex].dirty;
		
		//	clear L1 entry
		cp->blocks[index][lru_aindex].valid = 0;
		cp->blocks[index][lru_aindex].tag = 0;
		cp->blocks[index][lru_aindex].dirty = 0;
		cp->blocks[index][lru_aindex].ts = 0;
	}
	else if (cache_config->size_L2 != 0 && next_cp == NULL) {
		// case 3: evict LRU from L2
		printf("evicting from L2\n");
		int l2cache_index = getCacheIndex(ui32_address, cache_config, 2);
		
		int lru_aindex = -1;
		unsigned long long lru_ts_min = ULLONG_MAX;
		for (int i=0; i<cp->assoc; i++) {
			if ((cp->blocks[index][i].ts < lru_ts_min) && (cp->blocks[index][i].valid == 1)) {
				lru_ts_min = cp->blocks[index][i].ts;
				lru_aindex = i;
			}
		}
		
		if (lru_aindex != -1) {
			cp->blocks[l2cache_index][lru_aindex].tag = 0;
			cp->blocks[l2cache_index][lru_aindex].valid = 0;
			cp->blocks[l2cache_index][lru_aindex].ts = 0;
			cp->blocks[l2cache_index][lru_aindex].dirty = 0;
		}
		else {
			printf("could not locate eviction target in L2\n");
		}
	}
	else {
		printf("LRU eviction encountered an error.\n");
	}
	// return associativity index of evicted block
	//printf("evicted LRU block of L? at index %i, a_index %i\n", index, lru_aindex);
	return lru_aindex;
}

int cache_access(struct cache_t* cp, unsigned long address, char access_type, unsigned long long now, struct cache_t* next_cp) {
	int total_latency = 0;
	
	uint32_t ui32_address = (uint32_t) address;
	
	int l1cache_index = getCacheIndex(ui32_address, cache_config, 1);
	uint32_t l1address_tag = getCacheTag(ui32_address, cache_config, 1);
	
	int l2cache_index = -1;
	uint32_t l2address_tag = 0;
	if (next_cp != NULL) {
		l2cache_index = getCacheIndex(ui32_address, cache_config, 2);
		l2address_tag = getCacheTag(ui32_address, cache_config, 2);
	}
	
	if (CACHEDEBUG > 1) printf("–address %u -> L1i %i, L1t %u, L2i %i, L2t %u\n", ui32_address, l1cache_index, l1address_tag, l2cache_index, l2address_tag);
	
	printf("L1:");
	//	Check hit/miss
	int L1hit = hit_or_miss(cp, l1cache_index, l1address_tag, access_type);
	
	if (L1hit == 1) {	
		//	update access time
		//	1. find where in the set the block resides
		for(int i=0; i<cp->assoc; i++) {
			if( cp->blocks[l1cache_index][i].tag == l1address_tag ) {
				//	2. update its ts with now
				cp->blocks[l1cache_index][i].ts = now;
				cp->blocks[l1cache_index][i].dirty = ((access_type == 'w') ? 1:0);
				if (CACHEDEBUG > 1) printf(" -- L1 Hit (%c) @ cyle %llu for index %u, tag %u\n", access_type, now, l1cache_index, l1address_tag);
			}
		}
		L1hits++;
		total_latency = 0;
	}//end-if-L1hit
	else if (L1hit == 0) {	
		L1misses++;
		if (CACHEDEBUG > 1) printf(" -- L1 Miss\n");
		
		if (cache_config->size_L2 == 0) {
			// No L2, go to memory
			// nothing to fetch, just create the entry in L1
			int assoc_index = findOpenBlockAtIndex(cp, l1cache_index);
			
			// if no open spot, LRU eviction
			if (assoc_index == -1) {
				assoc_index = evictLRUblockAtIndex(cp, ui32_address, l1cache_index, NULL);
				if (CACHEDEBUG) printf(" -- No open L1 spots, evicting from L1index %i at a_index %i\n", l1cache_index, assoc_index);
			}
			
			// cache the block in the L1
			cp->blocks[l1cache_index][assoc_index].valid = 1;
			cp->blocks[l1cache_index][assoc_index].tag = l1address_tag;
			cp->blocks[l1cache_index][assoc_index].dirty = 0;
			cp->blocks[l1cache_index][assoc_index].ts = now;
			
			// increment the latency by the memory access latency
			total_latency += cache_config->access_time_mem;
		}//end-if-l1miss-noL2
		else {
			//	start into the L2
			printf(" -- searching L2: L2index %i tag %i\n", l2cache_index, l2address_tag);
			
			printf("L2:");
			// Check hit/miss
			int L2hit = hit_or_miss(next_cp, l2cache_index, l2address_tag, access_type);
			
			if (L2hit == 1) {
				L2hits++;
				
				if (CACHEDEBUG) printf(" -- L2 Hit\n");
				
				// bring from L2 into L1, first ensure space for it in L1
				int l1_aindex = findOpenBlockAtIndex(cp, l1cache_index);
				if (l1_aindex == -1) {
					l1_aindex = evictLRUblockAtIndex(cp, ui32_address, l1cache_index, next_cp);
				}
				cp->blocks[l1cache_index][l1_aindex].tag = l1address_tag;
				cp->blocks[l1cache_index][l1_aindex].ts = now;
				cp->blocks[l1cache_index][l1_aindex].dirty = ((access_type == 'w') ? 1:0);
				cp->blocks[l1cache_index][l1_aindex].valid = 1;
				
				total_latency += cache_config->access_time_L2;
			}//end-if-L2hit
			else {
				L2misses++;
				
				if (CACHEDEBUG) printf(" -- L2 Miss\n");
				
				// go to memory
				
				// put in L2
				//	find open spot
				int l2aindex = findOpenBlockAtIndex(next_cp, l2cache_index);
				if (l2aindex == -1) {
					l2aindex = evictLRUblockAtIndex(next_cp, ui32_address, l2cache_index, NULL);
					if (CACHEDEBUG) printf(" -- No open L2 spots, evicting from L2index %i at a_index %i\n", l2cache_index, l2aindex);
				}
				next_cp->blocks[l2cache_index][l2aindex].tag = l2address_tag;
				next_cp->blocks[l2cache_index][l2aindex].valid = 1;
				next_cp->blocks[l2cache_index][l2aindex].dirty = 0;				// L1 will record if dirty, L2 will reflect memory until then
				next_cp->blocks[l2cache_index][l2aindex].ts = now;
				printf(" -- cached in L2: L2index %i, tag: %i\n", l2cache_index, l2address_tag);
				
				// put in L1
				//	find open spot
				int l1aindex = findOpenBlockAtIndex(cp, l1cache_index);
				if (l1aindex == -1) {
					l1aindex = evictLRUblockAtIndex(cp, ui32_address, l1cache_index, next_cp);
					if (CACHEDEBUG) printf(" -- No open L1 spots, evicting from L1index %i at a_index %i\n", l1cache_index, l1aindex);
				}
				cp->blocks[l1cache_index][l1aindex].tag = l1address_tag;
				cp->blocks[l1cache_index][l1aindex].valid = 1;
				cp->blocks[l1cache_index][l1aindex].dirty = ((access_type == 'w') ? 1:0);
				cp->blocks[l1cache_index][l1aindex].ts = now;
				
				total_latency += (cache_config->access_time_mem) + (cache_config->access_time_L2);
			}//end-elif-L2miss
		}//end-elif-l1miss-haveL2
	}//end-elif-l1miss
	return total_latency;
}

//EOF
