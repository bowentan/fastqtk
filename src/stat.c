// Copyright (2018), Delta
//
// @author: Bowen Tan, email: notebowentan@gmail.com
//
// Created on May 21, 2018
// Lastest revised on May. 21, 2018
//
// This file is the source file of subprogram stat of fastqtk.

// Self include
#include "stat.h"

// C libraries
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void stat_main(prog_t *prog, int argc, char const **argv) {
    /* general options */
    int help = 0;

    /* file options */
    // input
    int fastq_num;
    char **fastq_fn;
    int adapter_num;
    char **adapter_fn;
    char *white_list_fn = NULL;
    // output
    char *out_dir = ".";
    char *out_basename;
    int verbose = 0;
    
    add_options(prog, 7,
                // input
                'f', "fastq", "Fastq file path(s)", OPTION_NOT_ALONG,
                OPTION_NECESSARY + OPTION_HAS_ARG + OPTION_REQUIRE_ARG + OPTION_MULTIPLE_ARG,
                VALUE_STRING, NULL, &fastq_num, &fastq_fn,

                'a', "adapter", "Adapter file path(s)", OPTION_NOT_ALONG,
                OPTION_HAS_ARG + OPTION_REQUIRE_ARG + OPTION_MULTIPLE_ARG,
                VALUE_STRING, NULL, &adapter_num, &adapter_fn,

                'w', "white_list", "Barcode white list file path", OPTION_NOT_ALONG,
                OPTION_HAS_ARG + OPTION_REQUIRE_ARG,
                VALUE_STRING, NULL, NULL, &white_list_fn,

                // output
                'O', "out_dir", "Output directory", OPTION_NOT_ALONG,
                OPTION_HAS_ARG + OPTION_REQUIRE_ARG,
                VALUE_STRING, NULL, NULL, &out_dir,

                'o', "out_basename", "Output basename", OPTION_NOT_ALONG,
                OPTION_NECESSARY + OPTION_HAS_ARG + OPTION_REQUIRE_ARG,
                VALUE_STRING, NULL, NULL, &out_basename,

                'v', "verbose", "Verbose log", OPTION_NOT_ALONG,
                OPTION_UNNECESSARY,
                VALUE_BOOL, NULL, NULL, &verbose,

                'h', "help", "Display thins help message", OPTION_ALONG,
                OPTION_UNNECESSARY,
                VALUE_BOOL, NULL, NULL, &help);
    
    parse_args(prog, argc, argv);
    
    if (help > 0) {
        print_help(prog);
        exit(1);
    }

    stat_t *stat;
    if (fastq_num == 1) {
        fastq_t *fq = fastq_open(fastq_fn[0], NULL);
        fastq_check(fq, verbose);
        stat = init_stat(fastq_num, fq->max_read_length, fq->qual_sys);

        read_t *r = init_read();
        while (get_read(fq, r) >= 0) {
            stat_read(stat, r, NULL);
        }
        destroy_read(r);
        fastq_close(fq);
    } else if (fastq_num == 2) {
        fastq_t *fq_1 = fastq_open(fastq_fn[0], NULL);
        fastq_t *fq_2 = fastq_open(fastq_fn[1], NULL);
        fastq_check(fq_1, verbose);
        fastq_check(fq_2, verbose);
        stat = init_stat(fastq_num, fq_1->max_read_length, fq_1->qual_sys);

        read_t *r_1 = init_read();
        read_t *r_2 = init_read();
        while (get_read(fq_1, r_1) >= 0 && get_read(fq_2, r_2) >= 0) {
            stat_read(stat, r_1, r_2);
        }
        destroy_read(r_1);
        destroy_read(r_2);
        fastq_close(fq_1);
        fastq_close(fq_2);
    }
    write_stat(stat, out_dir, out_basename);
    destroy_stat(stat);
}

static int _count_percentile(unsigned long *count, int count_size, unsigned long sum, int percent) {
     unsigned long idx = percent * sum / 100;
     unsigned long idx_count = 0;
     for (int k = 0; k < count_size; k++) {
        idx_count += count[k];
        if (idx_count >= idx) {
            return k;
        }
     }
     return -1;
}

stat_t *init_stat(int n_end, int n_base, qual_sys_t const *qual_sys) {
    stat_t *stat = malloc(sizeof(stat_t));
    stat->qual_sys = qual_sys;
    stat->n_end = n_end;
    stat->n_base = n_base;
    stat->n_read_pair = 0;
    stat->length_count = malloc(n_end * sizeof(unsigned long *));
    stat->base_count = malloc(n_end * sizeof(unsigned long *));
    // stat->base_freq = malloc(n_end * sizeof(double *));
    stat->qual_count = malloc(n_end * sizeof(unsigned long **));
    // stat->qual_freq = malloc(n_end * sizeof(double **));
    for (int i = 0; i < n_end; i++) {
        stat->length_count[i] = malloc(n_base * sizeof(unsigned long));
        stat->base_count[i] = malloc(n_base * sizeof(unsigned long[5]));
        // stat->base_freq[i] = malloc(n_base * sizeof(double[5]));
        stat->qual_count[i] = malloc(n_base * sizeof(unsigned long *));
        // stat->qual_freq[i] = malloc(n_base * sizeof(double *));
        for (int j = 0; j < n_base; j++) {
            stat->length_count[i][j] = 0;
            stat->qual_count[i][j] = malloc(qual_sys->qual_length * sizeof(unsigned long));
            // stat->qual_freq[i][j] = malloc(qual_sys->qual_length * sizeof(double));
            for (int k = 0; k < 5; k++) {
                stat->base_count[i][j][k] = 0;
                // stat->base_freq[i][j][k] = 0.0;
            }
            for (int k = 0; k < qual_sys->qual_length; k++) {
                stat->qual_count[i][j][k] = 0;
                // stat->qual_freq[i][j][k] = 0.0;
            }
        }
    }
    return stat;
}

void destroy_stat(stat_t *stat) {
    for (int i = 0; i < stat->n_end; i++) {
        free(stat->length_count[i]);
        stat->length_count[i] = NULL;
        free(stat->base_count[i]);
        stat->base_count[i] = NULL;
        // free(stat->base_freq[i]);
        // stat->base_freq[i] = NULL;
        free(stat->qual_count[i]);
        stat->qual_count[i] = NULL;
        // free(stat->qual_freq[i]);
        // stat->qual_freq[i] = NULL;
    }
    free(stat->length_count);
    stat->length_count = NULL;
    free(stat->base_count);
    stat->base_count = NULL;
    // free(stat->base_freq);
    // stat->base_freq = NULL;
    free(stat->qual_count);
    stat->qual_count = NULL;
    // free(stat->qual_freq);
    // stat->qual_freq = NULL;
    
    free(stat);
    stat = NULL;
}

void stat_read(stat_t *stat, read_t *read_1, read_t *read_2) {
    stat->length_count[0][read_1->seq_length - 1]++;
    for (int i = 0; i < read_1->seq_length; i++) {
        switch (read_1->seq[i]) {
            case 'A':
                stat->base_count[0][i][0]++;
                break;
            case 'C':
                stat->base_count[0][i][1]++;
                break;
            case 'G':
                stat->base_count[0][i][2]++;
                break;
            case 'T':
                stat->base_count[0][i][3]++;
                break;
            case 'N':
                stat->base_count[0][i][4]++;
                break;
        }
        stat->qual_count[0][i][read_1->qual[i] - stat->qual_sys->min_qual_char]++;
    }

    if (read_2 != NULL) {
        stat->length_count[1][read_2->seq_length - 1]++;
        for (int i = 0; i < read_2->seq_length; i++) {
            switch (read_2->seq[i]) {
                case 'A':
                    stat->base_count[1][i][0]++;
                    break;
                case 'C':
                    stat->base_count[1][i][1]++;
                    break;
                case 'G':
                    stat->base_count[1][i][2]++;
                    break;
                case 'T':
                    stat->base_count[1][i][3]++;
                    break;
                case 'N':
                    stat->base_count[1][i][4]++;
                    break;
            }
            stat->qual_count[1][i][read_2->qual[i] - stat->qual_sys->min_qual_char]++;
        }
    }
}

void write_stat(stat_t *stat, char const *out_dir, char const *base_name) {
    // out_dir/base_name.stat.read.length.1
    // out_dir/base_name.stat.read.length.2
    char length_file_name[strlen(out_dir) + strlen(base_name) + 
                          5/* ".stat" */ + 5/* ".read" */ + 7/* ".length" */ + 2/* ".1" */ + 1];
    // out_dir/base_name.stat.base.1
    // out_dir/base_name.stat.base.2
    char base_file_name[strlen(out_dir) + 
                        strlen(base_name) + 5/* ".stat" */ + 
                        5/* ".base" */ + 2/* ".1" */+ 1];
    // out_dir/base_name.stat.base.qual.1
    // out_dir/base_name.stat.base.qual.2
    char qual_file_name[strlen(out_dir) + 
                        strlen(base_name) + 5/* ".stat" */ +
                        5/* ".base" */ + 5/* ".qual" */ + 2/* ".1" */+ 1];

    for (int i = 0; i < stat->n_end; i++) {
        sprintf(length_file_name, "%s/%s.%s.%s.%s.%d",
                out_dir, base_name, "stat", "read", "length", i + 1);
        sprintf(base_file_name, "%s/%s.%s.%s.%s.%d", 
                out_dir, base_name, "stat", "base", "", i + 1);
        sprintf(qual_file_name, "%s/%s.%s.%s.%s.%d",
                out_dir, base_name, "stat", "base", "qual", i + 1);

        FILE *length_file = fopen(length_file_name, "w");
        FILE *base_file = fopen(base_file_name, "w");
        FILE *qual_file = fopen(qual_file_name, "w");

        if (!length_file || !base_file || !qual_file) {
            fprintf(stderr, "Error: no such directory -- \"%s\"\n", out_dir);
            exit(1);
        }

        fprintf(length_file, "len\tn\tpercent\n");
        fprintf(base_file, "Pos\tA\tC\tG\tT\tN\t\%_A\t\%_C\t\%_G\t\%_T\t\%_N\tTot\n");
        fprintf(qual_file, "Pos");
        for (int j = 0; j < stat->qual_sys->qual_length; j++) {
            fprintf(qual_file, "\t%d",
                    stat->qual_sys->min_qual_char - stat->qual_sys->zero_qual_char + j);
        }
        fprintf(qual_file, "\tTot\tMean\t5_percent\t10_percent\t25_percent\tMedian"
                           "\t75_percent\t90_percent\t95_percent\n");

        unsigned long length_count_sum = 0;
        unsigned long base_count_sum[5];
        unsigned long qual_count_sum[stat->qual_sys->qual_length];

        for (int k = 0; k < 5; k++) {
            base_count_sum[k] = 0;
        }
        for (int k = 0; k < stat->qual_sys->qual_length; k++) {
            qual_count_sum[k] = 0;
        }

        // length file
        for (int j = 0; j < stat->n_base; j++) {
            length_count_sum += stat->length_count[i][j];
        }

        for (int j = 0; j < stat->n_base; j++) {
            // length file
            fprintf(length_file, "%d\t%d\t%.2f\n",
                    j + 1, stat->length_count[i][j],
                    stat->length_count[i][j] * 100.0 / length_count_sum);

            // base file
            unsigned long base_count_per_pos = 0;
            for (int k = 0; k < 5; k++) {
                base_count_sum[k] += stat->base_count[i][j][k];
                base_count_per_pos += stat->base_count[i][j][k];
            }

            // base file
            fprintf(base_file, "%d", j + 1);
            for (int k = 0; k < 5; k++) {
                fprintf(base_file, "\t%d",
                        stat->base_count[i][j][k]);
            }
            for (int k = 0; k < 5; k++) {
                fprintf(base_file, "\t%.2f",
                        stat->base_count[i][j][k] * 100.0 / base_count_per_pos);
            }
            fprintf(base_file, "\t%d\n", base_count_per_pos);

            // qual file
            unsigned long qual_count_per_pos = 0;
            unsigned long qual_sum_per_pos = 0;
            fprintf(qual_file, "%d", j + 1);
            for (int k = 0; k < stat->qual_sys->qual_length; k++) {
                qual_count_sum[k] += stat->qual_count[i][j][k];
                qual_count_per_pos += stat->qual_count[i][j][k];
                qual_sum_per_pos += stat->qual_count[i][j][k] * 
                                    (stat->qual_sys->min_qual_char - stat->qual_sys->zero_qual_char + k);
                fprintf(qual_file, "\t%d",
                        stat->qual_count[i][j][k]);
            }
            int percent_5_idx = _count_percentile(stat->qual_count[i][j], stat->qual_sys->qual_length,
                                               qual_count_per_pos, 5);
            int percent_10_idx = _count_percentile(stat->qual_count[i][j], stat->qual_sys->qual_length,
                                               qual_count_per_pos, 10);
            int percent_25_idx = _count_percentile(stat->qual_count[i][j], stat->qual_sys->qual_length,
                                               qual_count_per_pos, 25);
            int percent_50_idx = _count_percentile(stat->qual_count[i][j], stat->qual_sys->qual_length,
                                               qual_count_per_pos, 50);
            int percent_75_idx = _count_percentile(stat->qual_count[i][j], stat->qual_sys->qual_length,
                                               qual_count_per_pos, 75);
            int percent_90_idx = _count_percentile(stat->qual_count[i][j], stat->qual_sys->qual_length,
                                               qual_count_per_pos, 90);
            int percent_95_idx = _count_percentile(stat->qual_count[i][j], stat->qual_sys->qual_length,
                                               qual_count_per_pos, 95);
            fprintf(qual_file, "\t%d\t%.2f\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
                    qual_count_per_pos, (double)qual_sum_per_pos / qual_count_per_pos,
                    stat->qual_sys->min_qual_char - stat->qual_sys->zero_qual_char + percent_5_idx,
                    stat->qual_sys->min_qual_char - stat->qual_sys->zero_qual_char + percent_10_idx,
                    stat->qual_sys->min_qual_char - stat->qual_sys->zero_qual_char + percent_25_idx,
                    stat->qual_sys->min_qual_char - stat->qual_sys->zero_qual_char + percent_50_idx,
                    stat->qual_sys->min_qual_char - stat->qual_sys->zero_qual_char + percent_75_idx,
                    stat->qual_sys->min_qual_char - stat->qual_sys->zero_qual_char + percent_90_idx,
                    stat->qual_sys->min_qual_char - stat->qual_sys->zero_qual_char + percent_95_idx);
        }
        // length file final "total" line
        fprintf(length_file, "Tot\t%d\t%.2f\n",
                length_count_sum, length_count_sum * 100.0 / length_count_sum);

        // base file final "total" line
        unsigned long base_total = 0;
        fprintf(base_file, "Tot");
        for (int k = 0; k < 5; k++) {
            base_total += base_count_sum[k];
            fprintf(base_file, "\t%d",
                    base_count_sum[k]);
        }
        for (int k = 0; k < 5; k++) {
            fprintf(base_file, "\t%.2f",
                    base_count_sum[k] * 100.0 / base_total);
        }
        fprintf(base_file, "\t%d\n", base_total);

        // qual file final "total" line
        unsigned long qual_count_total = 0;
        unsigned long qual_sum = 0;
        fprintf(qual_file, "Tot");
        for (int k = 0; k < stat->qual_sys->qual_length; k++) {
            qual_count_total += qual_count_sum[k];
            qual_sum += qual_count_sum[k] * 
                        (stat->qual_sys->min_qual_char - stat->qual_sys->zero_qual_char + k);
            fprintf(qual_file, "\t%d",
                    qual_count_sum[k]);
        }
        int percent_5_idx = _count_percentile(qual_count_sum, stat->qual_sys->qual_length,
                qual_count_total, 5);
        int percent_10_idx = _count_percentile(qual_count_sum, stat->qual_sys->qual_length,
                qual_count_total, 10);
        int percent_25_idx = _count_percentile(qual_count_sum, stat->qual_sys->qual_length,
                qual_count_total, 25);
        int percent_50_idx = _count_percentile(qual_count_sum, stat->qual_sys->qual_length,
                qual_count_total, 50);
        int percent_75_idx = _count_percentile(qual_count_sum, stat->qual_sys->qual_length,
                qual_count_total, 75);
        int percent_90_idx = _count_percentile(qual_count_sum, stat->qual_sys->qual_length,
                qual_count_total, 90);
        int percent_95_idx = _count_percentile(qual_count_sum, stat->qual_sys->qual_length,
                qual_count_total, 95);
        fprintf(qual_file, "\t%d\t%.2f\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
                qual_count_total, (double)qual_sum / qual_count_total,
                stat->qual_sys->min_qual_char - stat->qual_sys->zero_qual_char + percent_5_idx,
                stat->qual_sys->min_qual_char - stat->qual_sys->zero_qual_char + percent_10_idx,
                stat->qual_sys->min_qual_char - stat->qual_sys->zero_qual_char + percent_25_idx,
                stat->qual_sys->min_qual_char - stat->qual_sys->zero_qual_char + percent_50_idx,
                stat->qual_sys->min_qual_char - stat->qual_sys->zero_qual_char + percent_75_idx,
                stat->qual_sys->min_qual_char - stat->qual_sys->zero_qual_char + percent_90_idx,
                stat->qual_sys->min_qual_char - stat->qual_sys->zero_qual_char + percent_95_idx);

        fclose(length_file);
        fclose(base_file);
        fclose(qual_file);
    }
}
