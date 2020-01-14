#include <stdlib.h>
#include <stdio.h>

#include "program_options/program_options.h"

#include "config.h"
#include "stat.h"
#include "bcdist.h"
#include "filter.h"

int main(int argc, char const** argv) {
    int help = 0;
    int version = 0;

    prog_t* prog = init_prog("fastqtk", "A toolkit for processing fastq files");
    add_options(prog, 2,
                'v', "version", "Version info", OPTION_ALONG,
                OPTION_UNNECESSARY,
                VALUE_BOOL, NULL, NULL, &version,
                'h', "help", "Display this help message", OPTION_ALONG,
                OPTION_UNNECESSARY,
                VALUE_BOOL, NULL, NULL, &help);
    add_subprogs(prog, 3,
                 "stat", "Output the various information about given fastq(s)", stat_main,
                 "bcdist", "Count barcode distribution", bcdist_main,
                 "filter", "Filter reads", filter_main);
    parse_args(prog, argc, argv);

    if (version > 0) {
        fprintf(stderr, "%s\n", VERSION);
    }

    destroy_prog(prog);
    return 0;
}
