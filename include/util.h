/**
 * @file util.h
 * @brief TODO.
 *
 */

#ifndef UTIL_H
#define UTIL_H

typedef struct
{
    int verbose;
    int invert;
    int blend_overlap;
    int blend_offset;
    int blend_constrain_min;
    int blend_constrain_max;
    char *input_file;
    char *output_file;
} config_s;

typedef struct
{
    unsigned int width;
    unsigned int height;
    unsigned char *data;
} image_data_s;

void image_data_free(
        image_data_s * const image_data);

int read_grey8_png(
        const char * const filename,
        image_data_s * const image_data);

int write_rgba32_png(
        const char * const filename,
        const image_data_s * const image_data);

void blend_magnitude_rgb(
        const config_s * const config,
        const unsigned char magn,
        unsigned char * const red,
        unsigned char * const green,
        unsigned char * const blue);

int grey8_to_rgba32_alphamap(
        const config_s * const config,
        const image_data_s * const in,
        image_data_s * const out);

int generate_rgba32_alphamap(
        const config_s * const config,
        const image_data_s * const in,
        image_data_s * const out);

#endif /* UTIL_H */
