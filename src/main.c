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

enum option_kind
{
    OPTION_VERBOSE = 1,
    OPTION_INPUT_FILE = 2,
    OPTION_OUTPUT_FILE = 3,
    OPTION_INVERT = 4,
    OPTION_BLEND_OVERLAP = 5
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
    config.blend_overlap = 10;
    config.blend_offset = 25;
    config.blend_constrain_min = 100;
    config.blend_constrain_max = 255;

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
            "blend-overlap",
            '\0',
            POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,
            &config.blend_overlap,
            OPTION_BLEND_OVERLAP,
            "Blending overlap value",
            "0:85"
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
