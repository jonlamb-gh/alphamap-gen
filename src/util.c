/**
 * @file util.c
 * @brief TODO.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

// TODO - add some read-png() logic to libattopng
// TODO - use assertions/sanity-checks
#include "lodepng.h"
#include "libattopng.h"
#include "util.h"

void image_data_free(
        image_data_s * const image_data)
{
    if(image_data->data != NULL)
    {
        free(image_data->data);
        image_data->data = NULL;
    }
}

int read_grey8_png(
        const char * const filename,
        image_data_s * const image_data)
{
    unsigned int err;

    err = lodepng_decode_file(
            &image_data->data,
            &image_data->width,
            &image_data->height,
            filename,
            LCT_GREY,
            8);

    if(err != 0)
    {
        printf(
                "failed to read GREY8 file '%s' - %s\n",
                filename,
                lodepng_error_text(err));
    }

    return (int) err;
}

int write_rgba32_png(
        const char * const filename,
        const image_data_s * const image_data)
{
    int err = 0;

    libattopng_t * const png = libattopng_new(
            image_data->width,
            image_data->height,
            PNG_RGBA);

    if(png == NULL)
    {
        err = 1;
    }

    if(err == 0)
    {
        unsigned int x;
        unsigned int y;

        for(y = 0; y < image_data->height; y += 1)
        {
            for(x = 0; x < image_data->width; x += 1)
            {
                const unsigned int offset =
                        (4 * image_data->width * y) + (4 * x);

                const uint32_t * const color =
                        (uint32_t*) &image_data->data[offset];

                libattopng_set_pixel(
                        png,
                        x,
                        y,
                        *color);
            }
        }

        libattopng_save(png, filename);
    }

    if(png != NULL)
    {
        libattopng_destroy(png);
    }

    if(err != 0)
    {
        printf(
                "failed to write RGBA32 file '%s'\n",
                filename);
    }

    return err;
}

// TODO - probably a better way to do this
// 0 - 255 (higher to lower)
// blue - green - red
//
// example descrete blending:
// 0:84 -> blue 0:255
// 85:169 -> green 0:255
// 170:255 -> red 0:255
//
// constraints (min shouldn't be lower than x)?
void blend_magnitude_rgb(
        const config_s * const config,
        const unsigned char magn,
        unsigned char * const red,
        unsigned char * const green,
        unsigned char * const blue)
{
    const uint8_t blend_overlap = (uint8_t) config->blend_overlap;

    const uint8_t magnitude = (config->invert == 0) ? magn : (255 - magn);
    const uint8_t channel_max = 255.0f;
    const uint8_t num_subchannels = 3;
    const uint8_t subchannel_max = channel_max / num_subchannels;

    const uint8_t t0 = subchannel_max;
    const uint8_t t1 = (subchannel_max * 2);

    const uint8_t t0_pos_overlap = (t0 + blend_overlap);
    const uint8_t t0_neg_overlap = (t0 - blend_overlap);
    const uint8_t t1_pos_overlap = (t1 + blend_overlap);
    const uint8_t t1_neg_overlap = (t1 - blend_overlap);

    *red = 0;
    *green = 0;
    *blue = 0;

    if(magnitude >= t1)
    {
        *red = (magnitude - t1) * num_subchannels;

        if(magnitude <= t1_pos_overlap)
        {
            *green = (t1_pos_overlap - magnitude) * num_subchannels;
        }
    }
    else if(magnitude >= t0)
    {
        *green = (magnitude - t0) * num_subchannels;

        if(magnitude >= t1_neg_overlap)
        {
            *red = (magnitude - t1_neg_overlap) * num_subchannels;
        }
        else if(magnitude <= t0_pos_overlap)
        {
            *blue = (t0_pos_overlap - magnitude) * num_subchannels;
        }
    }
    else
    {
        *blue = magnitude * num_subchannels;

        if(magnitude >= t0_neg_overlap)
        {
            *green = (magnitude - t0_neg_overlap) * num_subchannels;
        }
    }
}

int grey8_to_rgba32_alphamap(
        const config_s * const config,
        const image_data_s * const in,
        image_data_s * const out)
{
    int ret = 0;
    unsigned int x;
    unsigned int y;
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    const unsigned char a = 0xFF;

    for(y = 0; y < in->height; y += 1)
    {
        for(x = 0; x < in->width; x += 1)
        {
            const unsigned char magn = in->data[(in->width * y) + x];

            blend_magnitude_rgb(
                    config,
                    magn,
                    &r,
                    &g,
                    &b);

            out->data[(4 * in->width * y) + (4 * x) + 0] = r;
            out->data[(4 * in->width * y) + (4 * x) + 1] = g;
            out->data[(4 * in->width * y) + (4 * x) + 2] = b;
            out->data[(4 * in->width * y) + (4 * x) + 3] = a;
        }
    }

    return ret;
}

int generate_rgba32_alphamap(
        const config_s * const config,
        const image_data_s * const in,
        image_data_s * const out)
{
    int ret = 0;
    const unsigned int image_size =
            (in->width * in->height * 4);

    out->width = in->width;
    out->height = in->height;

    out->data = calloc(image_size, 1);

    if(out->data == NULL)
    {
        printf("failed to allocate image data\n");
        ret = 1;
    }

    if(ret == 0)
    {
        ret = grey8_to_rgba32_alphamap(config, in, out);
    }

    return ret;
}
