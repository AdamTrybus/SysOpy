#include "mlib.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define BUFFER_SIZE 1024

MemoryBlocks* init(size_t max_size) {
    MemoryBlocks* mb = calloc(1, sizeof(MemoryBlocks));  // alokacja pamięci na strukturę i zerowanie jej
    mb->blocks = calloc(max_size, sizeof(void*));  // alokacja pamięci na tablicę wskaźników i zerowanie jej
    mb->max_size = max_size;
    mb->current_size = 0;
    return mb;
}

// funkcja zwracająca zawartość bloku o zadanym indeksie
char* show(MemoryBlocks* mb, size_t index) {
    if (index >= mb->current_size) {
        return NULL;  // indeks poza zakresem
    }
    return mb->blocks[index];
}

void delete_block(MemoryBlocks* mb, size_t index) {
    if (index >= mb->current_size) {
        return;  // indeks poza zakresem
    }
    void* block_to_delete = mb->blocks[index];
    free(block_to_delete);  // zwolnienie pamięci bloku
    mb->blocks[index] = NULL;  // ustawienie wskaźnika w tablicy na NULL
    mb->current_size--;  // zmniejszenie rozmiaru
}

void destroy(MemoryBlocks* mb) {
    // zwolnienie pamięci dla każdego bloku
    for (size_t i = 0; i < mb->current_size; i++) {
        void* block = mb->blocks[i];
        free(block);
    }
    // zwolnienie pamięci dla tablicy wskaźników
    free(mb->blocks);
    // zwolnienie pamięci dla struktury
    free(mb);
}

void count(MemoryBlocks* mb, const char* filename) {
    // utworzenie komendy wc z argumentami
    char command[BUFFER_SIZE];
    snprintf(command, BUFFER_SIZE, "wc %s > /tmp/wc_output", filename);

    // uruchomienie komendy wc w systemie
    int ret = system(command);
    if (ret != 0) {
        printf("Błąd podczas uruchamiania komendy wc.\n");
        return;
    }

    // otwarcie pliku tymczasowego
    FILE* fp = fopen("/tmp/wc_output", "r");
    if (fp == NULL) {
        printf("Błąd podczas otwierania pliku tymczasowego.\n");
        return;
    }

    // wczytanie liczby linii i słów
    int lines = 0;
    int words = 0;
    fscanf(fp, "%d %d", &lines, &words);
    fclose(fp);

    // usunięcie pliku tymczasowego
    remove("/tmp/wc_output");

    // zarezerwowanie bloku pamięci i przeniesienie danych
    size_t data_size = lines + words + 2;  // +2 dla separatora i końca linii
    void* data = calloc(data_size, sizeof(char));
    snprintf(data, data_size, "%d %d\n", lines, words);

    // zapisanie wskaźnika na blok w tablicy
    mb->blocks[mb->current_size] = data;
    mb->current_size++;
}