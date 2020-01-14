// Copyright (2018), Delta
//
// @author: Bowen Tan, email: notebowentan@gmail.com
//
// Created on May 17, 2018
// Lastest revised on May. 17, 2018
//
// This file is the source file of subprogram filter of fastqtk.

// Self include
#include "filter.h"

// C libraries
#include <stdlib.h>

void filter_main(prog_t* prog, int argc, char const** argv) {
    /* general options */
    int help = 0;
    int thread_num = 1;
    char* tmp_dir = ".";

    /* file options */
    // input
    int fastq_num;
    char** fastq_fn;
    int adapter_num;
    char** adapter_fn;
    char* white_list_fn;
    // output
    char* out_dir = ".";
    char* out_basename;
    
    /* filter param options */
    int check_fastq = 1;             // Indicator for whether to check fastq at first
    int clean_qual_sys_idx = 4;      // Quality system of clean fastq
    double max_base_N_rate = 0.05;   // Maximum rate allowed for base 'N' 
    double min_ave_qual = 0;         // Minimum average quality allowed
    double min_base_qual = 5;        // Lowest quality allowed of bases
    double max_low_qual_rate = 0.5;  // Maximum rate allowed for low quality
    int min_read_length = 90;        // Minimum read length
    int max_read_length = 150;       // Maximum read length
    double ten_X_confidence;         // Confidence level for 10X data

    add_options(prog, 15,
                // file options
                'f', "fastq", "Fastq file path(s)", OPTION_NOT_ALONG,
                OPTION_NECESSARY + OPTION_HAS_ARG + OPTION_REQUIRE_ARG + OPTION_MULTIPLE_ARG,
                VALUE_STRING, NULL, &fastq_num, &fastq_fn,

                'a', "adapter", "Adapter file path(s)", OPTION_NOT_ALONG,
                OPTION_HAS_ARG + OPTION_REQUIRE_ARG + OPTION_MULTIPLE_ARG,
                VALUE_STRING, NULL, &adapter_num, &adapter_fn,

                'w', "white_list", "Barcode white list file path", OPTION_NOT_ALONG,
                OPTION_HAS_ARG + OPTION_REQUIRE_ARG,
                VALUE_STRING, "c", NULL, &white_list_fn,

                'O', "out_dir", "Output directory", OPTION_NOT_ALONG,
                OPTION_HAS_ARG + OPTION_REQUIRE_ARG,
                VALUE_STRING, NULL, NULL, &out_dir,

                'o', "out_basename", "Output basename", OPTION_NOT_ALONG,
                OPTION_NECESSARY + OPTION_HAS_ARG + OPTION_REQUIRE_ARG,
                VALUE_STRING, NULL, NULL, &out_basename,

                // filter paramter options
                'C', "not_check", "Not check fastq before filtering", OPTION_NOT_ALONG,
                OPTION_UNNECESSARY,
                VALUE_BOOL, NULL, NULL, &check_fastq,

                's', "clean_sys",
                "Quality system of clean fastq\n"
                "  0: Sanger\n"
                "  1: Solexa\n"
                "  2: Illumina 1.3+\n"
                "  3: Illumina 1.5+\n"
                "  4: Illumina 1.8+\n",
                OPTION_NOT_ALONG,
                OPTION_HAS_ARG + OPTION_REQUIRE_ARG + OPTION_HAS_DEFAULT,
                VALUE_INT, NULL, NULL, &clean_qual_sys_idx,

                'N', "max_base_N_rate", "Maximum base N rate", OPTION_NOT_ALONG,
                OPTION_HAS_ARG + OPTION_REQUIRE_ARG + OPTION_HAS_DEFAULT,
                VALUE_FLOAT, NULL, NULL, &max_base_N_rate,

                'Q', "min_ave_qual", "Minimum average quality along a read", OPTION_NOT_ALONG,
                OPTION_HAS_ARG + OPTION_REQUIRE_ARG + OPTION_HAS_DEFAULT,
                VALUE_FLOAT, NULL, NULL, &min_ave_qual,

                'q', "min_base_qual", "Minimum base quality along a read", OPTION_NOT_ALONG,
                OPTION_HAS_ARG + OPTION_REQUIRE_ARG + OPTION_HAS_DEFAULT,
                VALUE_INT, NULL, NULL, &min_base_qual,

                'r', "max_low_qual_rate", "Maximum low base quality rate along a read", OPTION_NOT_ALONG,
                OPTION_HAS_ARG + OPTION_REQUIRE_ARG + OPTION_HAS_DEFAULT,
                VALUE_FLOAT, NULL, NULL, &max_low_qual_rate,

                'l', "min_read_len", "Minimum read length", OPTION_NOT_ALONG,
                OPTION_HAS_ARG + OPTION_REQUIRE_ARG + OPTION_HAS_DEFAULT,
                VALUE_INT, NULL, NULL, &min_read_length,

                'L', "max_read_len", "Maximum read length", OPTION_NOT_ALONG,
                OPTION_HAS_ARG + OPTION_REQUIRE_ARG + OPTION_HAS_DEFAULT,
                VALUE_INT, NULL, NULL, &max_read_length,
                
                'c', "10X_conf", "10X confidence level", OPTION_NOT_ALONG,
                OPTION_HAS_ARG + OPTION_REQUIRE_ARG,
                VALUE_FLOAT, "w", NULL, &ten_X_confidence,

                'h', "help", "Display thins help message", OPTION_ALONG,
                OPTION_UNNECESSARY,
                VALUE_BOOL, NULL, NULL, &help);
    
    parse_args(prog, argc, argv);
    
    if (help > 0) {
        print_help(prog);
        exit(1);
    }
}
