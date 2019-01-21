/*
 * Copyright 2016 neurodata (http://neurodata.io/)
 * Written by Disa Mhembere (disa@jhu.edu)
 *
 * This file is part of k-par-means
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

// Read raw row-wise data in a file and print to stdout

#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include "../../../../../libkcommon/io.hpp"

int main(int argc, char* argv []) {

    if (argc < 4) {
        printf("usage: ./read num_rows num_cols filename [-h | --has_header]\n");
        exit(EXIT_FAILURE);
    }

    size_t num_rows = atol(argv[1]);
    size_t num_cols = atol(argv[2]);

    FILE *f = fopen(argv[3], "rb");

    if (argc == 5) {
        std::cout << "Original pos:" << ftell(f) << "\n";
        fseek(f, 4096, SEEK_CUR);
        std::cout << "Pos after skip:" << ftell(f) << "\n";
    }

    double in [num_cols];

    for (size_t i = 0; i < num_rows; i++) {
        assert(fread(&(in[0]), sizeof(double), num_cols, f));

        printf("Row: %lu ==> ", i);
        knor::base::print<double>(in, num_cols);
    }
    fclose(f);

    return EXIT_SUCCESS;
}