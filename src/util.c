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
#include <assert.h>

#include "lodepng.h"
#include "libattopng.h"
#include "util.h"

#ifndef CONSTRAIN
#define CONSTRAIN(amt, low, high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif

static uint8_t constrain_color_channel(
        const blend_constraints_s * const cstr,
        const uint16_t value)
{
    assert(cstr->offset >= 0);
    assert(cstr->max >= cstr->min);

    const uint16_t min = CONSTRAIN(cstr->min, 0, 255);
    const uint16_t max = CONSTRAIN(cstr->max, min, 255);
    const uint16_t val = value + (uint16_t) cstr->offset;

    return (uint8_t) CONSTRAIN(val, min, max);
}

static void blend_magnitude_rgb(
        const config_s * const config,
        const unsigned char magn,
        unsigned char * const red,
        unsigned char * const green,
        unsigned char * const blue)
{
    // TODO - I'm sure there is a better way to do this
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

    if(*red != 0)
    {
        *red = constrain_color_channel(&config->red, *red);
    }

    if(*green != 0)
    {
        *green = constrain_color_channel(&config->green, *green);
    }

    if(*blue != 0)
    {
        *blue = constrain_color_channel(&config->blue, *blue);
    }
}

static int grey8_to_rgba32_alphamap(
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

static int blur_rgba32(
        const config_s * const config,
        const image_data_s * const in,
        image_data_s * const out)
{
    unsigned int x;
    unsigned int y;
    int kx;
    int ky;

    assert(config->blur_factor > 0.0f);

    const float mat[3][3] =
    {
        {config->blur_factor, config->blur_factor, config->blur_factor},
        {config->blur_factor, config->blur_factor, config->blur_factor},
        {config->blur_factor, config->blur_factor, config->blur_factor}
    };

    assert(out->width == in->width);
    assert(out->height == in->height);
    assert(out->data != NULL);

    // skip top and bottom edges
    for(y = 1; y < (in->height - 1); y += 1)
    {
        // skip left and right edges
        for(x = 1; x < (in->width - 1); x += 1)
        {
            const unsigned int offset =
                    (4 * in->width * y) + (4 * x);

            // rgba [0:3]
            uint8_t * const out_color =
                    (uint8_t*) &out->data[offset];

            float r_sum = 0.0f;
            float g_sum = 0.0f;
            float b_sum = 0.0f;

            for(ky = -1; ky <= 1; ky += 1)
            {
                for(kx = -1; kx <= 1; kx += 1)
                {
                    const unsigned int sub_offset =
                            ((y + ky) * in->width * 4) + ((x + kx) * 4);

                    // rgba [0:3]
                    const uint8_t * const in_color =
                            (uint8_t*) &in->data[sub_offset];

                    const float r_val = (float) in_color[0];
                    const float g_val = (float) in_color[1];
                    const float b_val = (float) in_color[2];

                    r_sum += (mat[ky + 1][kx + 1] * r_val);
                    g_sum += (mat[ky + 1][kx + 1] * g_val);
                    b_sum += (mat[ky + 1][kx + 1] * b_val);
                }
            }

            out_color[0] = (unsigned char) CONSTRAIN(r_sum, 0.0f, 255.0f);
            out_color[1] = (unsigned char) CONSTRAIN(g_sum, 0.0f, 255.0f);
            out_color[2] = (unsigned char) CONSTRAIN(b_sum, 0.0f, 255.0f);
            out_color[3] = 0xFF;
        }
    }

    return 0;
}

int image_data_alloc_rgba32(
        const unsigned int width,
        const unsigned int height,
        image_data_s * const image_data)
{
    int ret = 0;
    image_data->size = (width * height * 4);

    image_data->width = width;
    image_data->height = height;

    image_data->data = calloc(image_data->size, 1);

    if(image_data->data == NULL)
    {
        printf("failed to allocate image data\n");
        ret = 1;
    }

    return ret;
}

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

    if(err == 0)
    {
        image_data->size = (image_data->width * image_data->height * 1);
    }

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

int generate_rgba32_alphamap(
        const config_s * const config,
        const image_data_s * const in,
        image_data_s * const out)
{
    int ret;

    assert(in->size > 0);

    ret = image_data_alloc_rgba32(in->width, in->height, out);

    if(ret == 0)
    {
        ret = grey8_to_rgba32_alphamap(config, in, out);
    }

    if(ret == 0)
    {
        if(config->blur != 0)
        {
            image_data_s tmp;
            ret = image_data_alloc_rgba32(out->width, out->height, &tmp);

            if(ret == 0)
            {
                ret = blur_rgba32(config, out, &tmp);

                (void) memcpy(&out->data[0], &tmp.data[0], tmp.size);

                free(tmp.data);
            }
        }
    }

    return ret;
}
