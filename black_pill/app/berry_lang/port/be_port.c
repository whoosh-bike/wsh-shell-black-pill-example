#include "be_mem.h"
#include "be_sys.h"
#include "berry.h"
#include "def_rtos.h"
#include <stdio.h>
#include <string.h>

void* be_malloc_impl(size_t size) {
    size_t* p = pvPortMalloc(size + sizeof(size_t));
    *p        = size;
    return (void*)(p + 1);
}

void be_free_impl(void* ptr) {
    vPortFree(((size_t*)ptr) - 1);
}

static size_t be_size_impl(void* ptr) {
    return ((size_t*)ptr)[-1];
}

void* be_realloc_impl(void* ptr, size_t new_size) {
    if (!ptr) {
        return be_malloc_impl(new_size);
    }

    if (new_size == 0) {
        be_free_impl(ptr);
        return NULL;
    }

    size_t old_size = be_size_impl(ptr);
    void* new_ptr   = be_malloc_impl(new_size);
    if (!new_ptr) {
        return NULL;
    }

    size_t copy_size = old_size < new_size ? old_size : new_size;
    memcpy(new_ptr, ptr, copy_size);
    be_free_impl(ptr);
    return (void*)new_ptr;
}

BERRY_API void be_writebuffer(const char* buffer, size_t length) {
    be_fwrite(stdout, buffer, length);
}

BERRY_API char* be_readstring(char* buffer, size_t size) {
    return be_fgets(stdin, buffer, (int)size);
}

void* be_fopen(const char* filename, const char* modes) {
    return fopen(filename, modes);
}

int be_fclose(void* hfile) {
    return fclose(hfile);
}

size_t be_fwrite(void* hfile, const void* buffer, size_t length) {
    return fwrite(buffer, 1, length, hfile);
}

size_t be_fread(void* hfile, void* buffer, size_t length) {
    return fread(buffer, 1, length, hfile);
}

char* be_fgets(void* hfile, void* buffer, int size) {
    return fgets(buffer, size, hfile);
}

int be_fseek(void* hfile, long offset) {
    return fseek(hfile, offset, SEEK_SET);
}

long int be_ftell(void* hfile) {
    return ftell(hfile);
}

long int be_fflush(void* hfile) {
    return fflush(hfile);
}

size_t be_fsize(void* hfile) {
    long int size, offset = be_ftell(hfile);
    fseek(hfile, 0L, SEEK_END);
    size = ftell(hfile);
    fseek(hfile, offset, SEEK_SET);
    return size;
}
