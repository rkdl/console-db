#include "util.h"

#include<stdio.h>
#include<stdint.h>

size_t fpread(void *buffer, size_t size, size_t nitems, size_t offset, FILE *fp) {
    if (fseek(fp, offset, SEEK_SET) != 0) {
        return 0;
    }
    return fread(buffer, size, nitems, fp);
}

size_t fpwrite(void *buffer, size_t size, size_t nitems, size_t offset, FILE *fp) {
    if (fseek(fp, offset, SEEK_SET) != 0) {
        return 0;
    }
    return fwrite(buffer, size, nitems, fp);
}

size_t get_fsize(const char *fpath) {
    FILE *f = fopen(fpath, "rb");
    if (f == NULL) {
        return 0;
    }
    fseek(f, 0L, SEEK_END);
    size_t fsize = ftell(f);
    fclose(f);
    return fsize;
}

int64_t uint64_index_of(uint64_t array[], uint64_t length, uint64_t target) {
    for (int64_t i=0; i<length; i++) {
        if (array[i] == target) {
            return i;
        }
    }
    return -1;
}
