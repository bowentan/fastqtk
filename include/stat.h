// Copyright (2018), Delta
//
// @author: Bowen Tan, email: notebowentan@gmail.com
//
// Created on May 21, 2018
// Lastest revised on May. 21, 2018
//
// This file is the header file of subprogram stat of fastqtk.

#ifndef FASTQTK_STAT_H_
#define FASTQTK_STAT_H_

// Other libraries
#include "program_options/program_options.h"
#include "fastq/fastq.h"

typedef struct Stat {
    int n_end;
    int n_base;
    unsigned long n_read_pair;
    unsigned long **length_count;     // n_end by n_base

    unsigned long (**base_count)[5];  // n_end by n_base by 5 (ACGTN)
    // double (**base_freq)[5];
    unsigned long ***qual_count;      // n_end by n_base by n_qual
    // double ***qual_freq;

    qual_sys_t const *qual_sys;
} stat_t;

/* Main function */
void stat_main(prog_t *prog, int argc, char const **argv);

/* Utilization function */
// end : 1 for single end, 2 for pair end
stat_t *init_stat(int n_end, int n_base, qual_sys_t const *qual_sys);
void destroy_stat(stat_t *stat);
void stat_read(stat_t *stat, read_t *read_1, read_t *read_2);
void write_stat(stat_t *stat, char const *out_dir, char const *base_name);

#endif  // FASTQTK_STAT_H_
