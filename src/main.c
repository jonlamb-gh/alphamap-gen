/**
 * @file main.c
 * @brief TODO.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <popt.h>

#include "util.h"

// TODO - these will change
enum option_kind
{
    OPTION_VERBOSE = 1,
    OPTION_INPUT_FILE,
    OPTION_OUTPUT_FILE,
    OPTION_INVERT,
    OPTION_BLUR,
    OPTION_BLUR_FACTOR,
    OPTION_BLEND_OVERLAP,
    OPTION_RED_OFFSET,
    OPTION_RED_MIN,
    OPTION_RED_MAX,
    OPTION_GREEN_OFFSET,
    OPTION_GREEN_MIN,
    OPTION_GREEN_MAX,
    OPTION_BLUE_OFFSET,
    OPTION_BLUE_MIN,
    OPTION_BLUE_MAX
};

int main(int argc, char **argv)
{
    int err = 0;
    config_s config;
    image_data_s input_image_data;
    image_data_s output_image_data;
    poptContext opt_ctx;

    (void) memset(&config, 0, sizeof(config));

    input_image_data.data = NULL;
    output_image_data.data = NULL;

    config.invert = 0;
    config.blur = 0;
    config.blur_factor = 1.0f / 9.0f;
    config.blend_overlap = 10;

    const uint8_t default_offset = 25;
    const uint8_t default_min = 100;
    const uint8_t default_max = 255;
    config.red.offset = default_offset;
    config.red.min = default_min;
    config.red.max = default_max;
    config.green.offset = default_offset;
    config.green.min = default_min;
    config.green.max = default_max;
    config.blue.offset = default_offset;
    config.blue.min = default_min;
    config.blue.max = default_max;

    config.input_file = (char*) "heightmap.png";
    config.output_file = (char*) "alphamap.png";

    // TODO - probably should free up the arg strings
    const struct poptOption OPTIONS_TABLE[] =
    {
        {
            "verbose",
            'v',
            POPT_ARG_NONE,
            &config.verbose,
            OPTION_VERBOSE,
            "Enable verbose output",
            NULL
        },
        {
            "input-file",
            'i',
            POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT,
            &config.input_file,
            OPTION_INPUT_FILE,
            "Input file path",
            "<path>"

        },
        {
            "output-file",
            'o',
            POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT,
            &config.output_file,
            OPTION_OUTPUT_FILE,
            "Output file path",
            "<path>"

        },
        {
            "invert",
            '\0',
            POPT_ARG_NONE,
            &config.invert,
            OPTION_INVERT,
            "Invert heightmap values",
            NULL
        },
        {
            "blur",
            'b',
            POPT_ARG_NONE,
            &config.blur,
            OPTION_BLUR,
            "Apply a blur filter",
            NULL
        },
        {
            "blur-factor",
            '\0',
            POPT_ARG_FLOAT | POPT_ARGFLAG_SHOW_DEFAULT,
            &config.blur_factor,
            OPTION_BLUR,
            "Blurring factor",
            "f > 0"
        },
        {
            "blend-overlap",
            '\0',
            POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,
            &config.blend_overlap,
            OPTION_BLEND_OVERLAP,
            "Blending overlap value",
            "0:85"
        },
        {
            "red-offset",
            '\0',
            POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,
            &config.red.offset,
            OPTION_RED_OFFSET,
            "Blending red offset value",
            "0:255"
        },
        {
            "red-min",
            '\0',
            POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,
            &config.red.min,
            OPTION_RED_MIN,
            "Blending red min value",
            "0:255"
        },
        {
            "red-max",
            '\0',
            POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,
            &config.red.max,
            OPTION_RED_MAX,
            "Blending red max value",
            "0:255"
        },
        {
            "green-offset",
            '\0',
            POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,
            &config.green.offset,
            OPTION_GREEN_OFFSET,
            "Blending green offset value",
            "0:255"
        },
        {
            "green-min",
            '\0',
            POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,
            &config.green.min,
            OPTION_GREEN_MIN,
            "Blending green min value",
            "0:255"
        },
        {
            "green-max",
            '\0',
            POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,
            &config.green.max,
            OPTION_GREEN_MAX,
            "Blending green max value",
            "0:255"
        },
        {
            "blue-offset",
            '\0',
            POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,
            &config.blue.offset,
            OPTION_BLUE_OFFSET,
            "Blending blue offset value",
            "0:255"
        },
        {
            "blue-min",
            '\0',
            POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,
            &config.blue.min,
            OPTION_BLUE_MIN,
            "Blending blue min value",
            "0:255"
        },
        {
            "blue-max",
            '\0',
            POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,
            &config.blue.max,
            OPTION_BLUE_MAX,
            "Blending blue max value",
            "0:255"
        },
        POPT_AUTOHELP
        POPT_TABLEEND
    };

    opt_ctx = poptGetContext(
            NULL,
            argc,
            (const char**) argv,
            OPTIONS_TABLE,
            0);

    int opt_ret;
    while((opt_ret = poptGetNextOpt(opt_ctx)) >= 0)
    {
        if(opt_ret == OPTION_BLEND_OVERLAP)
        {
            if((config.blend_overlap < 0) || (config.blend_overlap > 85))
            {
                printf("invalid blend-overlap value %d\n", config.blend_overlap);
                err = 1;
            }
        }
    }

    if(opt_ret < -1)
    {
        (void) fprintf(
                stdout,
                "argument error '%s': %s\n\n",
                poptBadOption(opt_ctx, POPT_BADOPTION_NOALIAS),
                poptStrerror(opt_ret));
        poptPrintUsage(opt_ctx, stderr, 0);
        err = 1;
    }

    poptFreeContext(opt_ctx);

    if(err == 0)
    {
        err = read_grey8_png(
                config.input_file,
                &input_image_data);
    }

    if(err == 0)
    {
        err = generate_rgba32_alphamap(
                &config,
                &input_image_data,
                &output_image_data);
    }

    if(err == 0)
    {
        err = write_rgba32_png(
                config.output_file,
                &output_image_data);
    }

    image_data_free(&input_image_data);
    image_data_free(&output_image_data);

    if(err == 0)
    {
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}
