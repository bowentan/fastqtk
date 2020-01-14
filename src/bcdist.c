// Copyright (2018), Delta
//
// @author: Bowen Tan, email: notebowentan@gmail.com
//
// Created on May 17, 2018
// Lastest revised on May. 17, 2018
//
// This file is the header file of subprogram bcdist of fastqtk.

// Self include
#include "bcdist.h"

// C libraries
#include <stdlib.h>

// Other libraries
#include "fastq/fastq.h"
#include "fastq/barcode.h"

void bcdist_main(prog_t* prog, int argc, char const** argv) {
    int help = 0;
    char* wl_file_name;
    char* fq_file_name;
    char* dist_file_name;

    add_options(prog, 4,
                'l', "white_list", "Barcode white list file", OPTION_NOT_ALONG,
                OPTION_NECESSARY + OPTION_HAS_ARG + OPTION_REQUIRE_ARG,
                VALUE_STRING, NULL, NULL, &wl_file_name,
                'f', "fastq", "Fastq file", OPTION_NOT_ALONG,
                OPTION_NECESSARY + OPTION_HAS_ARG + OPTION_REQUIRE_ARG,
                VALUE_STRING, NULL, NULL, &fq_file_name,
                'o', "out", "Barcode distribution output", OPTION_NOT_ALONG,
                OPTION_NECESSARY + OPTION_HAS_ARG + OPTION_REQUIRE_ARG,
                VALUE_STRING, NULL, NULL, &dist_file_name,
                'h', "help", "Display this help message", OPTION_ALONG,
                OPTION_UNNECESSARY,
                VALUE_BOOL, NULL, NULL, &help);
    parse_args(prog, argc, argv);
    
    if (help > 0) {
        print_help(prog);
        exit(1);
    }

    fastq_t* fq = fastq_open(fq_file_name, NULL);
    white_list_t* wl = load_white_list(wl_file_name);
    count_barcode(wl, fq, 16);
    write_white_list_count(wl, dist_file_name);
    destroy_white_list(wl);
    fastq_close(fq);
}
