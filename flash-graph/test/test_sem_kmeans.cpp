/*
 * Copyright 2014 Open Connectome Project (http://openconnecto.me)
 * Written by Disa Mhembere (disa@jhu.edu)
 *
 * This file is part of FlashGraph.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../../../../libkcommon/util.hpp"
#include "../../../../libkcommon/dist_matrix.hpp"
#include "../../../../libkcommon/clusters.hpp"
#include "../../../../libkcommon/io.hpp"

namespace kbase = knor::base;
namespace kprune = knor::prune;

static kbase::prune_clusters::ptr g_clusters;
constexpr unsigned NCOL = 5;

std::vector<double> test_init_g_clusters(const size_t k=4) {
    printf("Running init g_clusters\n");
    assert(k == 4);

    const std::vector<double> v1 {1, 2, 3, 4, 5};
    const std::vector<double> v2 {6, 7, 8, 9, 10};
    const std::vector<double> v3 {6E-12, -23423.7, .82342342432, 93., 10};
    const std::vector<double> v4 {-.2342, -23.342, -.000003232, -3.234232, 1};

    const std::vector<double> v {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
        6E-12, -23423.7, .82342342432, 93., 10, -.2342, -23.342, -.000003232, -3.234232, 1};
    g_clusters = kbase::prune_clusters::create(k, NCOL, v); // ctor & init

    printf("Set clusters: \n");
    g_clusters->print_means();

    for (size_t cl = 0; cl < k; cl++) {
        printf("c:%lu =>\n", cl);
        kbase::print_arr(&(v[cl*NCOL]), NCOL);

        assert(kbase::eq_all<double>(&v[0], &(g_clusters->get_means()[0]), NCOL*k));
    }
    printf("Exiting test_init_g_clusters!\n");
    return v;
}

void test_eucl() {
    // Positive
    std::vector<double> v1 {1, 2, 3, 4, 5};
    std::vector<double> v2 {6, 7, 8, 9, 10};
    assert(kbase::eucl_dist(&v1[0], &v2[0], NCOL) == sqrt(125.0));
    assert(kbase::eucl_dist(&v2[0], &v1[0], NCOL) == sqrt(125.0));

    // Neg-pos, Pos-neg
    std::vector<double> v3 {6E-12, -23423.7, .82342342432, 93., 10};
    assert(ceil(kbase::eucl_dist(&v1[0], &v3[0], NCOL)) ==
            ceil(sqrt(548771372.227)));
    assert(ceil(kbase::eucl_dist(&v3[0], &v1[0], NCOL))
            == ceil(sqrt(548771372.227)));

    // No-op
    std::vector<double> v4 {0, 0, 0, 0, 0};
    assert(kbase::eucl_dist(&v1[0], &v4[0], NCOL) ==
            kbase::eucl_dist(&v4[0], &v1[0], NCOL));
    assert(kbase::eucl_dist(&v4[0], &v1[0], NCOL) == sqrt(55));

    // Neg-neg
    std::vector<double> v5 {-.2342, -23.342, -.000003232, -3.234232, 1};
    assert(ceil(kbase::eucl_dist(&v5[0], &v3[0], NCOL))
            == ceil(sqrt(547586097.2884537)));
    assert(ceil(kbase::eucl_dist(&v3[0], &v5[0], NCOL))
            == ceil(sqrt(547586097.2884537)));

    double arr1[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    double arr2[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    assert(kbase::eucl_dist(&arr1[0], &arr2[5], NCOL) == sqrt(125));

    printf("Exiting test_eucl ==> ");
}

void test_dist_matrix() {
    constexpr unsigned k = 4;
    test_init_g_clusters();
    kprune::dist_matrix::ptr dm = kprune::dist_matrix::create(k);

    /* Test compute_dist */
    dm->compute_dist(g_clusters, NCOL);

    printf("Clusters:\n"); g_clusters->print_means();
    printf("Cluster distance :\n"); dm->print();

    /* Test s_val */
    printf("Printing s_vals:\n");
    for (unsigned i = 0; i < k; i++) {
        assert(g_clusters->get_s_val(i) ==
                dm->get_min_dist(i));
    }
    printf("\n");
    printf("Exiting test_dist_matrix ==> ");
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        if (std::string(argv[1]) == "-e") {
            test_eucl(); std::cout << "Test eucl Success ...\n";
        } else if (std::string(argv[1]) == "-d") {
            test_dist_matrix(); std::cout << "Test distance matrix Success ...\n";
        } else {
            fprintf(stderr, "Unknown test option '%s'", argv[1]);
        }
    } else { /* Do all tests */
        test_eucl(); std::cout << "Test eucl Success ...\n";
        test_dist_matrix(); std::cout << "Test distance matrix Success ...\n";
    }
    return EXIT_SUCCESS;
}