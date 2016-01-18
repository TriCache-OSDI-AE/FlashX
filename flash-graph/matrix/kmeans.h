#ifndef __FG_KMEANS_H__
#define __FG_KMEANS_H__

/*
 * Copyright 2014 Open Connectome Project (http://openconnecto.me)
 * Written by Disa Mhembere (disa@jhu.edu)
 *
 * This file is part of FlashMatrix.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY CURRENT_KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <string.h>
#include <sys/time.h>
#ifdef PROFILER
#include <gperftools/profiler.h>
#endif
#include <vector>
#include <limits> 
#include <iostream>
#include <algorithm>

#include "log.h"
#include "common.h"

namespace {
    /**
     * \brief Print an arry of some length `len`.
     *	 \param len The length of the array.
     */
    template <typename T>
        static void print_arr(T* arr, unsigned len) {
            printf("[ ");
            for (unsigned i = 0; i < len; i++) {
                std::cout << arr[i] << " ";
            }
            printf("]\n");
        }

    /*\Internal
     * \brief print a col wise matrix of type double / double.
     * Used for testing only.
     * \param matrix The col wise matrix.
     * \param rows The number of rows in the mat
     * \param cols The number of cols in the mat
     */
    template <typename T>
        static void print_mat(T* matrix, const unsigned rows, const unsigned cols) {
            for (unsigned row = 0; row < rows; row++) {
                std::cout << "[";
                for (unsigned col = 0; col < cols; col++) {
                    std::cout << " " << matrix[row*cols + col];
                }
                std::cout <<  " ]\n";
            }
        }
}

namespace fg {
/**
 * \brief Compute kmeans on matrix of features
 * \param matrix The matrix who's row IDs are being clustered.
 * \param clusters The cluster centers (means).
 * \param cluster_assignments Which cluster each sample falls into.
 * \param cluster_assignment_counts How many members each cluster has.
 * \param num_rows The number of rows in `matrix`.
 * \param nev The number of eigenvalues / number of columns in `matrix`.
 * \param k The number of clusters required.
 * \param max_iters The maximum number of iterations of K-means to perform.
 * \param init The type of initilization ["random", "forgy", "kmeanspp"]
 **/
unsigned compute_kmeans(const double* matrix, double* clusters, 
		unsigned* cluster_assignments, unsigned* cluster_assignment_counts,
		const unsigned num_rows, const unsigned num_cols, const size_t k, 
		const unsigned MAX_ITERS, const int max_threads, const std::string init="kmeanspp",
		const double tolerance=-1, const std::string dist_type="eucl");
}
#endif
