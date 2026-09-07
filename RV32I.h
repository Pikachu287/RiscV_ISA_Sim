#include <stdio.h>
#include <stdlib.h>

typedef struct {
    Memory* data;
    __uint32_t base;
    __uint32_t size;

}Memory;


typedef struct {
    __uint32_t X[32];
    Memory * mem;
    __uint32_t PC;
    __uint8_t running;
}CPU;

Memory *create_memory();
void destroy_memory();
CPU *create_CPU();
void destroy_CPU();
