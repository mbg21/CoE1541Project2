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

typedef struct parsed_address_t {
	unsigned int tag;
	unsigned int index;
	unsigned int offset;
} parsed_address_t;

typedef struct cache_location_t {
	unsigned int index;		//	set index
	unsigned int way;		//	index within the set
} cache_location_t;

parsed_address_t parseAddress(unsigned int address, cache_config_t* cache_config) {
	parsed_address_t parsed_address;
	parsed_address.tag = 0x00;
	parsed_address.index = 0x00;
	parsed_address.offset = 0x00;
	return parsed_address;
}

cache_location_t cacheLocationForAddress(int address, cache_config_t* cache_config) {
	cache_location_t cache_location;
	cache_location.index = 0x00;
	cache_location.way = 0;
	return cache_location;
}

#endif
//EOF
