#include "tests.h"
#include "mem_internals.h"
#include <stdio.h>
#include <stdint.h>
#include "mem.h"
#include "util.h"

#define HEAP_SIZE 10000
#define SIZE 1000


/* Test №1
 * Обычное успешное выделение памяти.
 */
static void test_1(){
    fprintf(stderr, "Starting test №1! \n");
    void *block = _malloc(SIZE);

    fprintf(stderr, "Heap before being free: \n");
    debug_heap(stderr, HEAP_START);
    _free(block);
    fprintf(stderr, "Heap after being free: \n");
    debug_heap(stderr, HEAP_START);
    fprintf(stdout, "Test №1 succeed! \n");
}

/* Test №2
 * Освобождение одного блока из нескольких выделенных.
 */
static void test_2(){
    fprintf(stderr, "Starting test №2! \n");
    void* block_1 = _malloc(SIZE);
    void* block_2 = _malloc(SIZE);
    if (block_1 == NULL || block_2 == NULL){
        err("ERROR! Something gone wrong in test №2 \n");
    }
    fprintf(stderr, "Heap before 1st block is free: \n");
    debug_heap(stderr, HEAP_START);
    _free(block_1);

    struct block_header *block_data_1 = block_get_header(block_1);
    struct block_header *block_data_2 = block_get_header(block_2);
    if (!block_data_1 -> is_free || block_data_2 -> is_free){
        err("ERROR! Something gone wrong in test №2! Check data in blocks! \n");
    }

    _free(block_2);
    _free(block_1);
    fprintf(stderr, "Heap after being free: \n");
    debug_heap(stderr, HEAP_START);

    fprintf(stdout, "Test №2 succeed! \n");
}

/* Test №3
 * Освобождение двух блоков из нескольких выделенных.
 */
static void test_3(){
    fprintf(stderr, "Starting test №3! \n");
    void* block_1 = _malloc(SIZE);
    void* block_2 = _malloc(SIZE);
    void* block_3 = _malloc(SIZE);
    if (block_1 == NULL || block_2 == NULL || block_3 == NULL){
        err("ERROR! Something gone wrong in test №3 \n");
    }
    fprintf(stderr, "Heap before being free: \n");
    debug_heap(stderr, HEAP_START);
    _free(block_2);
    _free(block_1);
    
    fprintf(stderr, "Heap after being free: \n");
    debug_heap(stderr, HEAP_START);

    struct block_header *block_data_1 = block_get_header(block_1);
    struct block_header *block_data_2 = block_get_header(block_2);
    struct block_header *block_data_3 = block_get_header(block_3);
    if (!block_data_1 -> is_free || !block_data_2 -> is_free || block_data_3 -> is_free){
        err("ERROR! Something gone wrong in test №3! Check data in blocks! \n");
    }
    _free(block_3);
    _free(block_2);
    _free(block_1);
    debug_heap(stderr, HEAP_START);
    fprintf(stdout, "Test №3 succeed! \n");
}

/* Test №4
 * Память закончилась, новый регион памяти расширяет старый.
 */
static void test_4(){
    fprintf(stderr, "Starting test №4! \n");
    void* block_1 = _malloc(SIZE);

    if (block_1 == NULL){
        err("ERROR! Something gone wrong in test №4 \n");
    }

    fprintf(stderr,"Heap after 1st block allocation: \n");
    debug_heap(stderr, HEAP_START);

    void* block_2 = _malloc(66000);
    fprintf(stderr,"Heap after memory extension: \n");
    debug_heap(stderr, HEAP_START);

    _free(block_2);
    _free(block_1);
    fprintf(stderr,"Heap after being free: \n");
    debug_heap(stdout, HEAP_START);
    fprintf(stdout,"Test №4 succeed! \n");
}

/* Test №5
 * Память закончилась, старый регион памяти не расширить из-за другого выделенного
 * диапазона адресов, новый регион выделяется в другом месте
*/
static void test_5() {
    fprintf(stderr,"Starting test №5! \n");

    void* block_1 = _malloc(SIZE * 10);

    if (block_1 == NULL){
        err("ERROR! Something gone wrong in test №4 \n");
    }
    struct block_header *block_data_1 = block_get_header(block_1);


    int64_t offset = 13000;
    size_t length = 30000;
    fprintf(stderr,"Heap before memory extension: \n");
    debug_heap(stderr, block_data_1);

    struct block_header *reg_ext = (struct block_header *) block_data_1 + offset;
    mmap((void *) reg_ext, length, PROT_READ | PROT_WRITE, 0, 0, 0);
    _malloc(29000);

    fprintf(stderr,"Heap after memory extension: \n");
    debug_heap(stderr, block_data_1);
    _free(block_1);

    fprintf(stdout,"Test №5 succeed! \n");
}

void testing(){
        test_1();
        test_2();
        test_3();
        test_4();
        test_5();
        fprintf(stdout,"All tests are passed! \n");
}


