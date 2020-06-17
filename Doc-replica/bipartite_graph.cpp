/*
 BIPARTITE GRAPH PARTIAL COLORING
 COMPILE:
 	g++ -g -Wall -fopenmp -o test bipartite_graph.cpp
 RUN:
 	./test 4 < coPapersDBLP.mtx
*/

#include <iostream>
#include <fstream>
#include <omp.h>
#include "bipartite_graph.h"

using namespace std;
int thread_count;
unsigned int v, e;

//		VERTEX BASED

template<class N, class E>
void CBipartiteGraph<N, E>::color_queue_vertex()
{
	int tam = mleft.size();
	vector<int> forbiddenColors;
	// resize the forbidden colors
	forbiddenColors.resize ( tam, -1);
	unsigned int id_w, id_v;

	#pragma omp parallel for num_threads(thread_count) firstprivate(forbiddenColors)
	for (unsigned int w=0; w<W.size(); w++) {
		find_mleft(W[w]->value, id_w);
		// for each left vertex in adj: nets(w)
		for (typename list<Node*>::iterator v=mleft[id_w]->neighbours.begin(); v!=mleft[id_w]->neighbours.end(); v++) {
			// for each right vertex in adj: vtxs(v)
			find_mright((*v)->value, id_v);	
			for (typename list<Node*>::iterator u=mright[id_v]->neighbours.begin(); u!=mright[id_v]->neighbours.end(); u++) {
				// mark color of node (*u) as forbidden to vertex
				if((*u)->color != -1) {
					forbiddenColors[(*u)->color] = W[w]->value;
				}
			}
		}
		int candidate_col = 0;
		// first-fit coloring policy
		while (forbiddenColors[candidate_col] == W[w]->color) 
			candidate_col += 1;
		W[w]->color = candidate_col;
	}
}

template<class N, class E>
void CBipartiteGraph<N, E>::remove_conflicts_vertex()
{
	unsigned int id_w, id_v;
	#pragma omp parallel for shared(mvertices_need_new_color)
	for (unsigned int w=0; w<W.size(); w++) {
		find_mleft(W[w]->value, id_w);
		for (typename list<Node*>::iterator v=mleft[id_w]->neighbours.begin(); v!=mleft[id_w]->neighbours.end(); v++) {
			find_mright((*v)->value, id_v);	
			for (typename list<Node*>::iterator u=mright[id_v]->neighbours.begin(); u!=mright[id_v]->neighbours.end(); u++) {
				if ((*u)->color == W[w]->color && W[w]->value > (*u)->value)
					#pragma omp critical
					mvertices_need_new_color.push_back(W[w]);
					break;
			}
		}
	}
}

template<class N, class E>
void CBipartiteGraph<N, E>::greedy_graph_coloring_vertex()
{
	while(!W.empty()) {
		std::cout<<"	coloring\n";
		color_queue_vertex();
		std::cout<<"	remove conflicts\n";
		remove_conflicts_vertex();
		// clear vertices to be colored
		W.clear();
		for (unsigned int i=0; i<mvertices_need_new_color.size(); i++)
			W.push_back(mvertices_need_new_color[i]);
	}
}

// 		NET BASED

template<class N, class E>
void CBipartiteGraph<N, E>::color_queue_net()
{
	int tam = mleft.size();
	vector<int> forbiddenColors;
	// resize the forbidden colors
	forbiddenColors.resize ( tam, -1);
	unsigned int id_v;
	int candidate_col = 0;

	#pragma omp parallel for num_threads(thread_count) firstprivate(forbiddenColors, candidate_col)
	for (unsigned int v=0; v<mright.size(); v++) {
		candidate_col = 0;
		find_mleft(mright[v]->value, id_v);
		// vtxs(v)
		for (typename list<Node*>::iterator u=mright[id_v]->neighbours.begin(); u!=mright[id_v]->neighbours.end(); u++) {
			// mark color of node (*u) as forbidden to vertex
			if((*u)->color == -1 || (*u)->color==forbiddenColors[candidate_col]) {
				while (forbiddenColors[candidate_col] == mright[v]->color) 
					candidate_col += 1;
				mright[v]->color = candidate_col;
				forbiddenColors[(*u)->color] = mright[v]->value;
			}
		}
	}
}

template<class N, class E>
void CBipartiteGraph<N, E>::remove_conflicts_net()
{
	int tam = mleft.size();
	vector<int> forbiddenColors;
	// resize the forbidden colors
	forbiddenColors.resize ( tam, -1);
	unsigned int id_v;

	#pragma omp parallel for num_threads(thread_count) firstprivate(forbiddenColors) 
	for (unsigned int v=0; v<mright.size(); v++) {
		find_mleft(mright[v]->value, id_v);
		// vtxs(v)
		for (typename list<Node*>::iterator u=mright[id_v]->neighbours.begin(); u!=mright[id_v]->neighbours.end(); u++) {
			// mark color of node (*u) as forbidden to vertex
			if((*u)->color != -1 ) {
				if (forbiddenColors[(*u)->color]) 
					(*u)->color = -1;
				else
					forbiddenColors[(*u)->color] = mright[v]->value;
			}
		}
	}
}

template<class N, class E>
void CBipartiteGraph<N, E>::greedy_graph_coloring_net()
{
	std::cout<<"	coloring\n";
	color_queue_net();
	std::cout<<"	remove conflicts\n";
	remove_conflicts_net();
}

//		ALGORITHMS
template<class N, class E>
void CBipartiteGraph<N, E>::v_v()
{
	while(!W.empty()) {
		std::cout<<"	coloring\n";
		color_queue_vertex();
		std::cout<<"	remove conflicts\n";
		remove_conflicts_vertex();
		// clear vertices to be colored
		W.clear();
		for (unsigned int i=0; i<mvertices_need_new_color.size(); i++)
			W.push_back(mvertices_need_new_color[i]);
	}
}

template<class N, class E>
void CBipartiteGraph<N, E>::v_n1()
{
	int it = 0;
	while(!W.empty()) {
		std::cout<<"	coloring\n";
		color_queue_vertex();
		std::cout<<"	remove conflicts\n";
		if (it < 1)
			remove_conflicts_net();
		// clear vertices to be colored
		W.clear();
		for (unsigned int i=0; i<mvertices_need_new_color.size(); i++)
			W.push_back(mvertices_need_new_color[i]);
	}
}

template<class N, class E>
void CBipartiteGraph<N, E>::v_n2()
{
	int it = 0;
	while(!W.empty()) {
		std::cout<<"	coloring\n";
		color_queue_vertex();
		std::cout<<"	remove conflicts\n";
		if (it < 2)
			remove_conflicts_net();
		// clear vertices to be colored
		W.clear();
		for (unsigned int i=0; i<mvertices_need_new_color.size(); i++)
			W.push_back(mvertices_need_new_color[i]);
	}
}

template<class N, class E>
void CBipartiteGraph<N, E>::n1_n2()
{
	int it1 = 0;
	int it2 = 0;
	std::cout<<"	coloring\n";
	if (it1 < 1)
		color_queue_net();
	std::cout<<"	remove conflicts\n";
	if (it2 < 2)
		remove_conflicts_net();
}

template<class N, class E>
void CBipartiteGraph<N, E>::n2_n2()
{
	int it1 = 0;
	int it2 = 0;
	std::cout<<"	coloring\n";
	if (it1 < 2)
		color_queue_net();
	std::cout<<"	remove conflicts\n";
	if (it2 < 2)
		remove_conflicts_net();
}
int main(int argc, char* argv[])
{
	int a, b;
	double start, finish;
	
	std::cout << "BIPARTITE GRAPH PARTIAL COLORING\n";
	CBipartiteGraph<int, int> bgraph;
	thread_count = strtol(argv[1], NULL, 10);
    std::cin >> v >> e;

	for(unsigned int i=1; i<=v; i++) {
		bgraph.InsNode(i);
	}
	std::cout<<"	load nodes\n";
	
	unsigned int id_a;
	unsigned int id_b;
	while(e>0) {
		cin>> a >> b;
		if (bgraph.find_mleft(a, id_a) && bgraph.find_mright(b, id_b)) {
			bgraph.mleft[id_a]->InsEdge(bgraph.mright[id_b]);
		}
		else if (bgraph.find_mright(a, id_a) && bgraph.find_mleft(b, id_b)) {
			bgraph.mright[id_a]->InsEdge(bgraph.mleft[id_b]);
		}
		e--;
	}
	std::cout<<"	load edges\n";
	// std::cout<<"	print graph\n";
	// bgraph.printGraph();
	bgraph.first_queue();

	ofstream result;
	result.open ("result_bipartite.txt", std::ios_base::app);
	result << "\nthread: " << argv[1] << " - Dataset: coPapersDBLP \n"; // test

	// cout<<"> Vertex Based: \n";
	// start = omp_get_wtime();
    // bgraph.greedy_graph_coloring_vertex();
    // finish = omp_get_wtime();
    // std::cout<<"Elapsed time: "<<finish-start<<" seconds\n";
	// result << "> Vertex Based: "<< finish-start << "\n";

	// cout<<"> Net Based: \n";
	// start = omp_get_wtime();
    // bgraph.greedy_graph_coloring_net();
    // finish = omp_get_wtime();
    // std::cout<<"Elapsed time: "<<finish-start<<" seconds\n";
	// result << "> Net Based: "<< finish-start << "\n";

	// ---------------------------------     ALGORITHMS
	cout<<"> V-V \n";
	start = omp_get_wtime();
    bgraph.v_v();
    finish = omp_get_wtime();
    std::cout<<"Elapsed time: "<<finish-start<<" seconds\n";
	result << "> V-V: "<< finish-start << "\n";

	cout<<"> V-N1 \n";
	start = omp_get_wtime();
    bgraph.v_n1();
    finish = omp_get_wtime();
    std::cout<<"Elapsed time: "<<finish-start<<" seconds\n";
	result << "> V-N1: "<< finish-start << "\n";

	cout<<"> V-N2 \n";
	start = omp_get_wtime();
    bgraph.v_n2();
    finish = omp_get_wtime();
    std::cout<<"Elapsed time: "<<finish-start<<" seconds\n";
	result << "> V-N2: "<< finish-start << "\n";

	cout<<"> N1-N2 \n";
	start = omp_get_wtime();
    bgraph.n1_n2();
    finish = omp_get_wtime();
    std::cout<<"Elapsed time: "<<finish-start<<" seconds\n";
	result << "> N1-N2: "<< finish-start << "\n";

	cout<<"> N2-N2 \n";
	start = omp_get_wtime();
    bgraph.n2_n2();
    finish = omp_get_wtime();
    std::cout<<"Elapsed time: "<<finish-start<<" seconds\n";
	result << "> N2-N2: "<< finish-start << "\n";

	// for (int i=0; i<bgraph.mleft.size(); i++)
	// 	std::cout<<bgraph.mleft[i]->color<<" ";
	// std::cout<<"\n";
	// for (int i=0; i<bgraph.mright.size(); i++)
	// 	std::cout<<bgraph.mright[i]->color<<" ";	
	result.close();
	return 0;
}
