/**
 * Copyright 2014 Open Connectome Project (http://openconnecto.me)
 * Written by Da Zheng (zhengda1936@gmail.com)
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

#include <signal.h>
#ifdef PROFILER
#include <google/profiler.h>
#endif

#include "FGlib.h"

void int_handler(int sig_num)
{
#ifdef PROFILER
	if (!graph_conf.get_prof_file().empty())
		ProfilerStop();
#endif
	exit(0);
}

std::string supported_algs[] = {
	"cycle_triangle",
	"triangle",
	"local_scan",
	"topK_scan",
	"wcc",
	"scc",
	"diameter",
	"pagerank",
	"pagerank2",
};
int num_supported = sizeof(supported_algs) / sizeof(supported_algs[0]);

void print_usage()
{
	fprintf(stderr,
			"test_algs [options] conf_file graph_file index_file algorithm\n");
	fprintf(stderr, "-c confs: add more configurations to the system\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "scan-statistics:\n");
	fprintf(stderr, "-K topK: topK vertices in topK scan\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "diameter estimation:\n");
	fprintf(stderr, "-p num_para_bfs: the number of parallel bfs to estimate diameter\n");
	fprintf(stderr, "-d: whether we respect the direction of edges\n");
	fprintf(stderr, "-s num: the number of sweeps performed in diameter estimation\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "pagerank\n");
	fprintf(stderr, "-i num: the maximum number of iterations\n");
	fprintf(stderr, "-D v: damping factor\n");

	fprintf(stderr, "supported graph algorithms:\n");
	for (int i = 0; i < num_supported; i++)
		fprintf(stderr, "\t%s\n", supported_algs[i].c_str());
	graph_conf.print_help();
	params.print_help();
}

int main(int argc, char *argv[])
{
	int opt;
	std::string confs;
	int num_opts = 0;
	int topK = 1;
	int num_para_bfs = 1;
	bool directed = false;
	int num_sweeps = 5;
	int num_iters = 30;
	float damping_factor = 0.85;
	while ((opt = getopt(argc, argv, "c:K:p:ds:i:D:")) != -1) {
		num_opts++;
		switch (opt) {
			case 'c':
				confs = optarg;
				num_opts++;
				break;
			case 'K':
				topK = atoi(optarg);
				num_opts++;
				break;
			case 'p':
				num_para_bfs = atoi(optarg);
				num_opts++;
				break;
			case 'd':
				directed = true;
				break;
			case 's':
				num_sweeps = atoi(optarg);
				num_opts++;
				break;
			case 'i':
				num_iters = atoi(optarg);
				num_opts++;
				break;
			case 'D':
				damping_factor = atof(optarg);
				num_opts++;
				break;
			default:
				print_usage();
		}
	}
	argv += 1 + num_opts;
	argc -= 1 + num_opts;

	if (argc < 4) {
		print_usage();
		exit(-1);
	}

	std::string conf_file = argv[0];
	std::string graph_file = argv[1];
	std::string index_file = argv[2];
	std::string alg = argv[3];

	config_map configs(conf_file);
	configs.add_options(confs);

	signal(SIGINT, int_handler);

	FG_graph::ptr graph = FG_graph::create(graph_file, index_file, configs);
	if (alg == "cycle_triangle") {
		FG_vector<size_t>::ptr triangles;
		triangles = compute_directed_triangles(graph,
				directed_triangle_type::CYCLE);
		printf("There are %ld cycle triangles\n", triangles->sum());
	}
	else if (alg == "triangle") {
		FG_vector<size_t>::ptr triangles;
		triangles = compute_undirected_triangles(graph);
		printf("There are %ld triangles\n", triangles->sum());
	}
	else if (alg == "local_scan") {
		FG_vector<size_t>::ptr scan = compute_local_scan(graph);
		printf("Max local scan is %ld\n", scan->max());
	}
	else if (alg == "topK_scan") {
		FG_vector<std::pair<vertex_id_t, size_t> >::ptr scan = compute_topK_scan(graph, topK);
		printf("The top %d scans:\n", topK);
		for (int i = 0; i < topK; i++)
			printf("%u\t%ld\n", scan->get(i).first, scan->get(i).second);
	}
	else if (alg == "diameter") {
		size_t diameter = estimate_diameter(graph, num_para_bfs, directed,
				num_sweeps);
		printf("The estimated diameter is %ld\n", diameter);
	}
	else if (alg == "pagerank") {
		FG_vector<float>::ptr pr = compute_pagerank(graph, num_iters,
				damping_factor);
		printf("The sum of pagerank of all vertices: %f\n", pr->sum());
	}
	else if (alg == "pagerank2") {
		FG_vector<float>::ptr pr = compute_pagerank2(graph, num_iters,
				damping_factor);
		printf("The sum of pagerank of all vertices: %f\n", pr->sum());
	}
	else if (alg == "wcc" || alg == "scc") {
		FG_vector<vertex_id_t>::ptr comp_ids;
		if (alg == "wcc")
			comp_ids = compute_wcc(graph);
		else
			comp_ids = compute_scc(graph);

		count_map<vertex_id_t> map;
		comp_ids->count_unique(map);
		std::pair<vertex_id_t, size_t> max_comp = map.get_max_count();
		int has_empty = 0;
		if (map.exists(INVALID_VERTEX_ID)) {
			printf("There are %ld empty vertices\n",
					map.get(INVALID_VERTEX_ID));
			has_empty = 1;
		}
		printf("There are %ld components (exclude empty vertices), and largest comp has %ld vertices\n",
				map.get_size() - has_empty, max_comp.second);
	}
}
