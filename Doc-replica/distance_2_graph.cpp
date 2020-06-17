
/*
 DISTANCE 2 GRAPH COLORING
 COMPILE:
 	g++ -g -Wall -fopenmp -o dtest distance_2_graph.cpp
 RUN:
 	./dtest 4 < coPapersDBLP.mtx
*/

#include <fstream>
#include <iostream>
#include <omp.h>
#include "graph.h"

using namespace std;
int thread_count;
unsigned int v, e;

//-----------------      VERTEX BASED
template<class N, class E>
void CGraph<N, E>::color_queue_vertex()
{
	int sz = mnodes.size();
	vector<int> forbiddenColors;
	// resize the forbidden colors
	forbiddenColors.resize ( sz, -1);
    unsigned int id_w, id_u;
    int candidate_col=0;

    #pragma omp parallel for num_threads(thread_count) firstprivate(forbiddenColors)
	for (unsigned int w=0; w<W.size(); w++) {
        find(W[w]->value, id_w);
		// nbor(v)
		for (typename list<Node*>::iterator u=mnodes[id_w]->neighbours.begin(); u!=mnodes[id_w]->neighbours.end(); u++) {
			if((*u)->color != -1 || (*u)->color != forbiddenColors[candidate_col]) 
                forbiddenColors[(*u)->color] = (*u)->value;
            // nbor(u)
			find((*u)->value, id_u);	
			for (typename list<Node*>::iterator v=mnodes[id_u]->neighbours.begin(); v!=mnodes[id_u]->neighbours.end(); v++) {
				if((*v)->color != -1 || (*v)->color != forbiddenColors[candidate_col]) 
                    forbiddenColors[(*v)->color] = (*v)->value;
            }
        }
        candidate_col = 0;
		// first-fit coloring policy
		while (forbiddenColors[candidate_col] == W[w]->color) 
			candidate_col += 1;
		W[w]->color = candidate_col;
    }

}

template<class N, class E>
void CGraph<N, E>::remove_conflicts_vertex()
{
	unsigned int id_w, id_u;
	#pragma omp parallel for shared(mvertices_need_new_color)
	for (unsigned int w=0; w<W.size(); w++) {
		find(W[w]->value, id_w);
		for (typename list<Node*>::iterator u=mnodes[id_w]->neighbours.begin(); u!=mnodes[id_w]->neighbours.end(); u++) {
			if (mnodes[id_w]->color == (*u)->color && mnodes[id_w]->value > (*u)->value)
				#pragma omp critical
				mvertices_need_new_color.push_back(mnodes[id_w]);
				break;
            find((*u)->value, id_u);	
			for (typename list<Node*>::iterator v=mnodes[id_u]->neighbours.begin(); v!=mnodes[id_u]->neighbours.end(); v++) {
				if (mnodes[id_w]->color == (*v)->color && mnodes[id_w]->value > (*v)->value)
					#pragma omp critical
					mvertices_need_new_color.push_back(mnodes[id_w]);
					break;
			}
		}
	}
}

//----------------- 		NET BASED

template<class N, class E>
void CGraph<N, E>::color_queue_net()
{
	int sz = mnodes.size();
	vector<int> forbiddenColors;
	// resize the forbidden colors
	forbiddenColors.resize ( sz, -1);

// 		cout<<"-- "<<v->value;
    int candidate_col = 0;
	#pragma omp parallel for num_threads(thread_count) firstprivate(forbiddenColors, W)
	for (unsigned int v=0; v<sz; v++) {
        if (mnodes[v]->color != -1 )
            forbiddenColors[mnodes[v]->color] = mnodes[v]->value;
        else
            W.push_back(mnodes[v]);
		// nbor(v)
		for (typename list<Node*>::iterator u=mnodes[v]->neighbours.begin(); u!=mnodes[v]->neighbours.end(); u++) {
            // mark color of node (*u) as forbidden to vertex
			if((*u)->color != -1 || (*u)->color != forbiddenColors[candidate_col]) 
                forbiddenColors[(*u)->color] = mnodes[v]->value;
            else
                W.push_back((*u));
		}
        candidate_col = mnodes[v]->neighbours.size();
        for (unsigned int w=0; w<W.size(); w++) {
            while (forbiddenColors[candidate_col] == W[w]->color) 
                candidate_col -= 1;
            W[w]->color = candidate_col;
            candidate_col -= 1;
        }   
	}
}

template<class N, class E>
void CGraph<N, E>::remove_conflicts_net()
{
	int sz = mnodes.size();
	vector<int> forbiddenColors;
	// resize the forbidden colors
	forbiddenColors.resize ( sz, -1);

	#pragma omp parallel for num_threads(thread_count) firstprivate(forbiddenColors) 
	for (unsigned int v=0; v<sz; v++) {
		if(mnodes[v]->color != -1 )
            forbiddenColors[mnodes[v]->color] = mnodes[v]->value;
		for (typename list<Node*>::iterator u=mnodes[v]->neighbours.begin(); u!=mnodes[v]->neighbours.end(); u++) {
			// mark color of node (*u) as forbidden to vertex
			if((*u)->color != -1){
                if ((*u)->color == forbiddenColors[(*u)->color])
                    (*u)->color = -1;
                else
                    forbiddenColors[(*u)->color] = (*u)->value;
            }
		}
	}
}

template<class N, class E>
void CGraph<N, E>::greedy_graph_coloring_net()
{
	std::cout<<"	coloring\n";
	color_queue_net();
	std::cout<<"	remove conflicts\n";
    W.clear();
	remove_conflicts_net();
}

//-----------------		ALGORITHMS
template<class N, class E>
void CGraph<N, E>::v_n1()
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
void CGraph<N, E>::v_n2()
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
void CGraph<N, E>::n1_n2()
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

int main(int argc, char* argv[])
{
	int a, b;
	double start, finish;
	
	std::cout << "DISTANCE 2 GRAPH COLORING\n";
	CGraph<int, int> graph;
	thread_count = strtol(argv[1], NULL, 10);
    std::cin >> v >> e;

	for(unsigned int i=1; i<=v; i++) {
		graph.InsNode(i);
	}
	std::cout<<"	load nodes\n";

    unsigned int id_a;
	unsigned int id_b;
	while(e>0) {
		cin >> a >> b;
        graph.find(a, id_a);
        graph.find(b, id_b);
        graph.mnodes[id_a]->InsEdge(graph.mnodes[id_b]);
		e--;
	}
	std::cout<<"	load edges\n";
	// std::cout<<"	print graph\n";
	// graph.printGraph();
	graph.first_queue();

    ofstream result;
	result.open ("result_distance2.txt", std::ios_base::app);
	result << "\nthread: " << argv[1] << " - Dataset: coPapersDBLP \n"; // coPapersDBLP

	// cout<<"> Net Based: "<<endl;
	// start = omp_get_wtime();
    // graph.greedy_graph_coloring_net();
    // finish = omp_get_wtime();
    // std::cout<<"Elapsed time: "<<finish-start<<" seconds\n";
    // result << "> Net Based: "<< finish-start << "\n";

    // ---------------------------------     ALGORITHMS
	// cout<<"> V-N1 \n";
	// start = omp_get_wtime();
    // graph.v_n1();
    // finish = omp_get_wtime();
    // std::cout<<"Elapsed time: "<<finish-start<<" seconds\n";
	// result << "> V-N1: "<< finish-start << "\n";

	// cout<<"> V-N2 \n";
	// start = omp_get_wtime();
    // graph.v_n2();
    // finish = omp_get_wtime();
    // std::cout<<"Elapsed time: "<<finish-start<<" seconds\n";
	// result << "> V-N2: "<< finish-start << "\n";

	// cout<<"> N1-N2 \n";
	// start = omp_get_wtime();
    // graph.n1_n2();
    // finish = omp_get_wtime();
    // std::cout<<"Elapsed time: "<<finish-start<<" seconds\n";
	// result << "> N1-N2: "<< finish-start << "\n";

    result.close();
	return 0;
}