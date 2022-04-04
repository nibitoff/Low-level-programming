#ifndef _MEM_H_
#define _MEM_H_


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <sys/mman.h>

#define HEAP_START ((void*)0x04040000)
#define INITIAL_HEAP REGION_MIN_SIZE

void* _malloc( size_t query );
void  _free( void* mem );

#define DEBUG_FIRST_BYTES 4

void debug_struct_info( FILE* f, void const* address );
void debug_heap( FILE* f,  void const* ptr );
struct block_header* block_get_header(void* contents);

#endif
