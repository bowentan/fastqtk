// Copyright (2018), Delta
//
// @author: Bowen Tan, email: notebowentan@gmail.com
//
// Created on May 17, 2018
// Lastest revised on May. 17, 2018
//
// This file is the header file of subprogram filter of fastqtk.

#ifndef FASTQTK_FILTER_H_
#define FASTQTK_FILTER_H_

// Other libraries
#include "program_options/program_options.h"

typedef struct thread_args {
} thread_args_t;

/* Main function */
void filter_main(prog_t* prog, int argc, char const** argv);

#endif  // FASTQTK_FILTER_H_
