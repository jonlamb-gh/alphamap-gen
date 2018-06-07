/**
 * @file util.h
 * @brief TODO.
 *
 */

#ifndef UTIL_H
#define UTIL_H

typedef struct
{
    int offset;
    int min;
    int max;
} blend_constraints_s;

typedef struct
{
    int verbose;
    int invert;
    int blur;
    float blur_factor;
    int blend_overlap;
    blend_constraints_s red;
    blend_constraints_s green;
    blend_constraints_s blue;
    char *input_file;
    char *output_file;
} config_s;

typedef struct
{
    unsigned int width;
    unsigned int height;
    unsigned int size;
    unsigned char *data;
} image_data_s;

int image_data_alloc_rgba32(
        const unsigned int width,
        const unsigned int height,
        image_data_s * const image_data);

void image_data_free(
        image_data_s * const image_data);

int read_grey8_png(
        const char * const filename,
        image_data_s * const image_data);

int write_rgba32_png(
        const char * const filename,
        const image_data_s * const image_data);

int generate_rgba32_alphamap(
        const config_s * const config,
        const image_data_s * const in,
        image_data_s * const out);

#endif /* UTIL_H */
