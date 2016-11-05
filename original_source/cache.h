
#include <stdlib.h>
#include <stdio.h>
#include "../CacheConfigurator/CacheConfigurator.h"

#define DEBUG 1

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
  int nsets;				// # setss
  int blocksize;			// block size
  int assoc;				// associativity
  int hit_latency;			// latency in case of a hit
  struct cache_blk_t **blocks;    // the array of cache blocks
  
  struct cache_t* L2_cache; // FIX ME: We should assign this to a cache_t struct if the config files says so..
};

// Creates an L2 cache if the configuration file specifies one. 
// Returns the newly craeted L2 cache if one is specified or NULL if there is no L2 cache or if some error occured. 
int should_create_L2_cache(struct cache_config_t* cache_config){
  
  if (cache_config->size_L2 == 0){ return 0; } // there is no L2 cache
  else if (cache_config->size_L2 > 0 ){ return 1; } // we should create an L2 cache
  else { if (DEBUG){ printf("Something weird happened\n"); } return -1; }
}

// function that returns a pointer to a cache with the given parameters
struct cache_t* cache_create(int size, int blocksize, int assoc, int latency)
{
  // REASSIGN THESE
  int i;
  int nblocks = 1;			// number of blocks in the cache
  int nsets = 1;			// number of sets (entries) in the cache

  // YOUR JOB: calculate the number of sets and blocks in the cache
  
//  nblocks = X;
//  nsets = Y;

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

void print_hit_or_miss(int hit_or_miss_i, int index, int way, unsigned long cache_tag, unsigned long tag2cmp){
  
  if (hit_or_miss_i == 1){printf("Hit!\tIndex:%d\t\tWay:%d\t\t\tTag in Cache:%lu\t\tTag to Find:%lu\t\tNumerical Value:%d\n", index, way, cache_tag, tag2cmp, hit_or_miss_i); }
  else if (hit_or_miss_i == 0){ printf("Miss!\t-------\t\t-----\t\t\t--------------\t\tTag to Find:%lu\t\tNumerical Value:%d\n", tag2cmp, hit_or_miss_i); }
  else { printf("Neither hit or miss. This shouldn't happen."); }
}

// Checks if a cache hits or misses at a given index for a given tag. 
// This function returns 1 if its a hit, 0 if its a miss, or -1 is an error has occured. 
int hit_or_miss(struct cache_t *cp, int index, unsigned long tag2cmp){
    
  struct cache_blk_t* blocks = cp->blocks[index]; // get blocks from cache
  struct cache_blk_t block_with_associativity;
  unsigned long tag; 
  int way; 
  int result; 
  
   // unsigned long cant be zero..
   // so just check for max size error
  if (index > cp->nsets - 1){
    
    printf("Index exceeds the number of sets in the cache. Returning -1.\n");
    return -1; // -1 means error...we should check for this
    
  }else{
     
    // search each way..
    for (way = 0; way < cp->assoc; way++){
       block_with_associativity = blocks[way]; // get all the blocks from that index
      
       tag = block_with_associativity.tag; // get tag from cache
    
      // compare tags
      if (tag == tag2cmp){ 
        
          result = 1; 
      
          if(DEBUG) {print_hit_or_miss(result, index, way, tag, tag2cmp ); } 
      
          return 1; // hit
      }else if (tag != tag2cmp){ 
          // go to next way
      }else { printf("Something went wrong. Returning -1\n"); return -1;}
    }
    
    result = 0; 
    if(DEBUG) {print_hit_or_miss(result, index, tag, way, tag2cmp ); }
    return result; // we searched each way and didn't find anything...miss.
  }
    
}

int cache_access(struct cache_t *cp, unsigned long address, 
			 char access_type, unsigned long long now, struct cache_t *next_cp)
{
  
  // Check the associtivity of the configuration based on text configuration
  
  
  // case 1: 1 way associtivity
  
    // Get the size of the cache (from config file)
    // Turn block size into bytes (2^X) (from config file)
    // Get number of block (cache size/block size) --> 2^Y/2^X = 2^Z
    
    // index = address mod block size
    // tag = 32 - Z - X
 
    // Go to index in cache AND check for tag
        
    // if tag found: 
       // hit --> L1 hit_counter++
      
    // if tag is NOT present:
      // miss in L1 --> L1 miss_counter++
    
      // check for presence of L2 cache: 
        // if there is L2 cache, go to it --> CPI + L2 access time
          // check the index in the L2 and see if tag is found
            // if IN L2 --> grab from L2 and evict LRU entry in L1 & store --> L2 hit_counter++
            // if NOT in L2 --> go to MAIN MEMORY --> CPI + memory access_time --> L2 miss_counter++   
            
        // if there is NO L2 cache: 
          // determine LRU to replace and replace
        
        
        
    // case 2: 4 way associtivity
  
      // Get the size of the cache (from config file)
      // Turn block size into bytes (2^X) (from config file)
      // Get number of block (cache size/block size) --> 2^Y/2^X = 2^Z
      
      // index = address mod block size
      // tag = 32 - Z - X
     
      // go to index in cache AND search each 'way' for the tag (4 ways)
      
      // if tag is present:  
         // hit --> L1 hit_counter++
        
      // if tag is NOT present:
        // miss --> L1 miss_counter++; 
      
        // check for presence of L2 cache: 
          // if there is L2 cache, go to it --> CPI + L2 access time
            // check the index in the L2 and see if tag is found in a 'way'
            // if IN L2 --> grab from L2 and evict LRU entry in L1 & store --> L2 hit_counter++
            // if NOT in L2 --> go to MAIN MEMORY --> CPI + memory access_time --> L2 miss_counter++ 
             
          // if there is NO L2 cache: 
           // determine LRU to replace and replace

    
  // case 3: 8 way associtivity
  
      // Get the size of the cache (from config file)
      // Turn block size into bytes (2^X) (from config file)
      // Get number of block (cache size/block size) --> 2^Y/2^X = 2^Z
      
      // index = address mod block size
      // tag = 32 - Z - X
     
      // go to index in cache AND search each 'way' for the tag (8 ways)
      
      // if tag is present:  
         // hit --> L1 hit_counter++
        
      // if tag is NOT present: 
        // miss --> L1 miss_counter++; 
       
        // check for presence of L2 cache: 
          // if there is L2 cache, go to it --> CPI + L2 access time
            // check the index in the L2 and see if tag is found in a 'way'
            // if IN L2 --> grab from L2 and evict LRU entry in L1 & store --> L2 hit_counter++
            // if NOT in L2 --> go to MAIN MEMORY --> CPI + memory access_time --> L2 miss_counter++
            
          // if there is NO L2 cache: 
            // determine LRU to replace and replace
 

           
  //
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
	// return(cp->hit_latency);
	return(cp->hit_latency);
}

