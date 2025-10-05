// STB Image Library - Simplified for debug purposes
#ifndef STB_IMAGE_H
#define STB_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

// Function declarations
unsigned char *stbi_load(const char *filename, int *x, int *y, int *comp, int req_comp);
void stbi_image_free(void *retval_from_stbi_load);

#ifdef __cplusplus
}
#endif

#ifdef STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Simplified implementation for debugging
unsigned char *stbi_load(const char *filename, int *x, int *y, int *comp, int req_comp) {
    // For debug purposes, create a simple 64x64 white texture
    (void)filename; // Suppress unused parameter warning
    
    *x = 64;
    *y = 64;
    *comp = 3; // RGB
    
    int size = 64 * 64 * 3;
    unsigned char *data = (unsigned char*)malloc(size);
    if (data) {
        // Fill with white
        memset(data, 255, size);
    }
    
    return data;
}

void stbi_image_free(void *retval_from_stbi_load) {
    free(retval_from_stbi_load);
}

#endif // STB_IMAGE_IMPLEMENTATION

#endif // STB_IMAGE_H