#include <stdarg.h>
#define _DEFAULT_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "mem_internals.h"
#include "mem.h"
#include "util.h"

void debug_block(struct block_header* b, const char* fmt, ... );
void debug(const char* fmt, ... );

extern inline block_size size_from_capacity( block_capacity cap );
extern inline block_capacity capacity_from_size( block_size sz );

static bool block_is_big_enough( size_t query, struct block_header* block ) { return block->capacity.bytes >= query; }

static size_t  pages_count ( size_t mem )  { return mem / getpagesize() + ((mem % getpagesize()) > 0); }

static size_t round_pages ( size_t mem )   { return getpagesize() * pages_count( mem ) ; }

static void block_init( void* restrict addr, block_size block_sz, void* restrict next ) {
  *((struct block_header*)addr) = (struct block_header) {
    .next = next,
    .capacity = capacity_from_size(block_sz),
    .is_free = true
  };
}

static size_t region_actual_size( size_t query ) { return size_max( round_pages( query ), REGION_MIN_SIZE ); }

extern inline bool region_is_invalid( const struct region* r );



 void* map_pages(void const* addr, size_t length, int additional_flags) {
  return mmap( (void*) addr, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | additional_flags , 0, 0 );
}

/*  аллоцировать регион памяти и инициализировать его блоком */
static struct region alloc_region  ( void const * addr, size_t query ) {
    size_t query_now = region_actual_size(query);
    void *new_addr_reg = map_pages(addr, query_now, MAP_FIXED);

    if (new_addr_reg == MAP_FAILED){
        new_addr_reg = map_pages(addr, query_now, 0);
    }
    if (new_addr_reg == MAP_FAILED){
        return REGION_INVALID;
    }
    struct region region = {.addr = new_addr_reg, .size = query_now, .extends = false};
    block_init(new_addr_reg, (block_size) {query_now}, NULL );

    return region;

}


static void* block_after( struct block_header const* block );

static void* heap_init( size_t initial ) {
  const struct region region = alloc_region( HEAP_START, initial);
  if ( region_is_invalid(&region) ) return NULL;

  return region.addr;
}

#define BLOCK_MIN_CAPACITY 24

/*  --- Разделение блоков (если найденный свободный блок слишком большой )--- */

static bool block_splittable( struct block_header* restrict block, size_t query) {
  return block-> is_free && query + offsetof( struct block_header, contents ) + BLOCK_MIN_CAPACITY <= block->capacity.bytes;
}


static bool split_if_too_big( struct block_header* block, size_t query ) {
    const bool check_split = block_splittable(block, query);
    if (check_split){
        const block_size my_size = (block_size) {
            size_from_capacity(block -> capacity).bytes - (query + offsetof(struct block_header, contents))};
        block -> capacity.bytes = query;
        void *pointer_second_block_part = block_after(block);
        block_init(pointer_second_block_part, my_size, block->next);
        block -> next = pointer_second_block_part;
        return true;
    } else {
        return false;
    }
}


/*  --- Слияние соседних свободных блоков --- */

static void* block_after( struct block_header const* block ){
  return  (void*) (block->contents + block->capacity.bytes);
}
static bool blocks_continuous (struct block_header const* fst,
                               struct block_header const* snd ) {
  return (void*)snd == block_after(fst);
}

static bool mergeable(struct block_header const* restrict fst, struct block_header const* restrict snd) {
  return fst->is_free && snd->is_free && blocks_continuous( fst, snd ) ;
}

static bool try_merge_with_next( struct block_header* block ) {
  if (block -> next && mergeable(block, block -> next)) {
      struct block_header *next = block->next;
      block->next = next->next;
      block->capacity.bytes += size_from_capacity(next->capacity).bytes;
      return true;
  } else {
      return false;
  }
}


/*  --- ... ecли размера кучи хватает --- */

struct block_search_result {
  enum {BSR_FOUND_GOOD_BLOCK, BSR_REACHED_END_NOT_FOUND, BSR_CORRUPTED} type;
  struct block_header* block;
};


static struct block_search_result find_good_or_last  ( struct block_header* restrict block, size_t sz ) {
    struct block_header *el_now = block;
    struct block_header *el_last = block;
    while (el_now) {
        el_last = el_now;
            if(block_is_big_enough(sz, el_now) && el_now->is_free) {
                return (struct block_search_result) {
                        .type = BSR_FOUND_GOOD_BLOCK,
                        .block = el_now
                };
            }
        el_now = el_now->next;

    }
        return (struct block_search_result) {
                .type = BSR_REACHED_END_NOT_FOUND,
                .block = el_last
        };
    }


/*  Попробовать выделить память в куче начиная с блока `block` не пытаясь расширить кучу
 Можно переиспользовать как только кучу расширили. */
static struct block_search_result try_memalloc_existing ( size_t query, struct block_header* block ) {
    struct block_search_result res = find_good_or_last(block, query);
    return res;
}


static struct block_header* grow_heap( struct block_header* restrict last, size_t query ) {
  struct region reg = alloc_region(block_after(last), query + offsetof(struct block_header, contents));
  if (region_is_invalid(&reg)){
      return NULL;
  } else{
      last -> next = reg.addr;
      if (!try_merge_with_next(last)){
          return last -> next;
      } else{
          return last;
      }
  }
}
//initializing heap if it's not
static bool init_heap_when_starts() {
    static bool check;
    if (!check) {
        if (heap_init(INITIAL_HEAP) == NULL) {
            fprintf(stderr, "Error! Problems with heap initializing!");
            check = false;
        } else {
            check = true;
        }
    }
    return check;
}


/*  Реализует основную логику malloc и возвращает заголовок выделенного блока */
static struct block_header* memalloc( size_t query, struct block_header* heap_start) {
    if (!init_heap_when_starts()) {
        return NULL;
    }
    struct block_search_result res = try_memalloc_existing(size_max(query, BLOCK_MIN_CAPACITY), heap_start);
    if (res.type  != BSR_FOUND_GOOD_BLOCK){
        res.block = grow_heap(res.block, query);
    } else if (res.type == BSR_CORRUPTED){
        return NULL;
    }
    split_if_too_big(res.block, query);
    res.block -> is_free = false;
    return res.block;
}

void* _malloc( size_t query ) {
  struct block_header* const addr = memalloc( query, (struct block_header*) HEAP_START );
  if (addr) return addr->contents;
  else return NULL;
}

struct block_header* block_get_header(void* contents) {
  return (struct block_header*) (((uint8_t*)contents)-offsetof(struct block_header, contents));
}

void _free( void* mem ) {
  if (!mem) return ;
  struct block_header* header = block_get_header( mem );
  header->is_free = true;
  //merging block with other free blocks
    while (try_merge_with_next(header));
}
