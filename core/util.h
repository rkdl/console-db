#pragma once

#include<stdio.h>
#include<stdint.h>

size_t fpread(void *buffer, size_t size, size_t mitems, size_t offset, FILE *fp);
size_t fpwrite(void *buffer, size_t size, size_t mitems, size_t offset, FILE *fp);
size_t get_fsize(const char *fpath);
int64_t uint64_index_of(uint64_t array[], uint64_t length, uint64_t target);
