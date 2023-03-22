#ifndef zad1_h
#define zad1_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct {
    void** blocks;  // tablica wskaźników na bloki pamięci
    size_t max_size;  // maksymalny rozmiar tablicy
    size_t current_size;  // aktualny rozmiar tablicy
} MemoryBlocks;

MemoryBlocks* init(size_t max_size);
char* show(MemoryBlocks* mb, size_t index);
void delete_block(MemoryBlocks* mb, size_t index);
void destroy(MemoryBlocks* mb);
void count(MemoryBlocks* mb, const char* filename);

#endif