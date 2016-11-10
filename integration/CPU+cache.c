//
//	CoE 1541 Fall 2016 Project 2
//	Max Garber <mbg21@pitt.edu> & Adrew Lendacky <anl119@pitt.edu>
//

#include <stdio.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "CPU.h"
#include "cache.h"
#include "CacheConfigurator.h"
#include "AddressParser.h"

#define CC_USEFILE 1

// to keep cache statistics
unsigned int accesses = 0;
unsigned int read_accesses = 0;
unsigned int write_accesses = 0;
unsigned int L1hits = 0;
unsigned int L1misses = 0;
unsigned int L2hits = 0;
unsigned int L2misses = 0;

// globally-accessible cache configuration pointer
cache_config_t* cache_config;

// making cycle_number globally accessible for non-colliding timestamps of memory accesses
unsigned int cycle_number;

int main(int argc, char **argv)
{
	struct trace_item *tr_entry;
	size_t size;
	char *trace_file_name;
	int trace_view_on = 0;
	
	unsigned char t_type = 0;
	unsigned char t_sReg_a= 0;
	unsigned char t_sReg_b= 0;
	unsigned char t_dReg= 0;
	unsigned int t_PC = 0;
	unsigned int t_Addr = 0;

	cycle_number = 0;

	if (argc == 1) {
		fprintf(stdout, "\nUSAGE: tv <trace_file> <switch - any character>\n");
		fprintf(stdout, "\n(switch) to turn on or off individual item view.\n\n");
		exit(0);
	}
		
	trace_file_name = argv[1];
	if (argc == 3) trace_view_on = atoi(argv[2]) ;
	
	// here you should extract the cache parameters from the configuration file (cache size, associativity, latency)
	unsigned int L1size;
	unsigned int bsize;
	unsigned int L1assoc;
	unsigned int L2size;
	unsigned int L2assoc;
	unsigned int L2_hit_latency;
	unsigned int mem_latency;
	
	char* filepath = "./cache_config.txt";
	
	if (CC_USEFILE == 1) cache_config = read_config_file(filepath);
	
	if (CC_USEFILE && cache_config!=NULL) {
		L1size = cache_config->size_L1;
		bsize = cache_config->blocksize;
		L1assoc = cache_config->associativity_L1;
		L2size = cache_config->size_L2;
		L2assoc = cache_config->associativity_L2;
		L2_hit_latency = cache_config->access_time_L2;
		mem_latency = cache_config->access_time_mem;
	} else {
		L1size = 16; 
		bsize = 32;
		L1assoc = 4;
		L2size = 0;
		L2_hit_latency = 6;
		mem_latency = 100;
		//printf("used default cache values\n");
	}
	
	fprintf(stdout, "\n ** opening file %s\n", trace_file_name);
	trace_fd = fopen(trace_file_name, "rb");
	if (!trace_fd) {
		fprintf(stdout, "\ntrace file %s not opened.\n\n", trace_file_name);
		exit(0);
	}

	trace_init();
	struct cache_t *L1, *L2, *nextL;
	L1 = cache_create(L1size, bsize, L1assoc, 0); 
	nextL = NULL;   // the next level in the hierarchy -- NULL indicates main memory
	if (L2size != 0)
	{
		L2 = cache_create(L2size, bsize, L2assoc, L2_hit_latency);
		nextL = L2;
	}

	while(1) {
		size = trace_get_item(&tr_entry);
		if (!size) {	/* no more instructions (trace_items) to simulate */
			printf("+ Simulation terminates at cycle : %u\n", cycle_number);
			printf("+ Cache statistics \n");
			break;
		}
		else{	/* parse the next instruction to simulate */
			cycle_number++;
			t_type = tr_entry->type;
			t_sReg_a = tr_entry->sReg_a;
			t_sReg_b = tr_entry->sReg_b;
			t_dReg = tr_entry->dReg;
			t_PC = tr_entry->PC;
			t_Addr = tr_entry->Addr;
		}

		// SIMULATION OF A SINGLE CYCLE cpu IS TRIVIAL - EACH INSTRUCTION IS EXECUTED
		// IN ONE CYCLE, EXCEPT IF THERE IS A DATA CACHE MISS.

		switch(tr_entry->type)
		{
			case ti_NOP:
				if (trace_view_on) printf("[cycle %d] NOP:", cycle_number);
				break;
			
			case ti_RTYPE:
				if (trace_view_on) {
					printf("[cycle %d] RTYPE:", cycle_number);
					printf(" (PC: %x)(sReg_a: %d)(sReg_b: %d)(dReg: %d) \n", tr_entry->PC, tr_entry->sReg_a, tr_entry->sReg_b, tr_entry->dReg);
				}
				break;
			
			case ti_ITYPE:
				if (trace_view_on){
					printf("[cycle %d] ITYPE:", cycle_number);
					printf(" (PC: %x)(sReg_a: %d)(dReg: %d)(addr: %x)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->dReg, tr_entry->Addr);
				}
				break;
			
			case ti_LOAD:
				if (trace_view_on){
					printf("[cycle %d] LOAD:", cycle_number);
					printf(" (PC: %x)(sReg_a: %d)(dReg: %d)(addr: %x)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->dReg, tr_entry->Addr);
				}
				accesses++;
				read_accesses++;
				cycle_number = cycle_number + cache_access(L1, tr_entry->Addr, 'r', cycle_number, nextL);
				break;
			
			case ti_STORE:
				if (trace_view_on){
					printf("[cycle %d] STORE:", cycle_number);
					printf(" (PC: %x)(sReg_a: %d)(sReg_b: %d)(addr: %x)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->sReg_b, tr_entry->Addr);
				}
				accesses++;
				write_accesses++;
				cycle_number = cycle_number + cache_access(L1, tr_entry->Addr, 'w', cycle_number, nextL);
				break;
			
			case ti_BRANCH:
				if (trace_view_on) {
					printf("[cycle %d] BRANCH:", cycle_number);
					printf(" (PC: %x)(sReg_a: %d)(sReg_b: %d)(addr: %x)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->sReg_b, tr_entry->Addr);
				}
				break;
			
			case ti_JTYPE:
				if (trace_view_on) {
					printf("[cycle %d] JTYPE:", cycle_number);
					printf(" (PC: %x)(addr: %x)\n", tr_entry->PC, tr_entry->Addr);
				}
				break;
			
			case ti_SPECIAL:
				if (trace_view_on) printf("[cycle %d] SPECIAL:", cycle_number);
				break;
			
			case ti_JRTYPE:
				if (trace_view_on) {
					printf("[cycle %d] JRTYPE:", cycle_number);
					printf(" (PC: %x) (sReg_a: %d)(addr: %x)\n", tr_entry->PC, tr_entry->dReg, tr_entry->Addr);
				};
				break;
		}//end-switch
		
	}//end-while

	trace_uninit();
	free(cache_config);
	exit(0);
}//end-main()

//EOF
