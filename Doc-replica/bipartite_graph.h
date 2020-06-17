#ifndef BIPARTITE_GRAPH_H
#define BIPARTITE_GRAPH_H
#include <iostream>
#include <list>
#include <vector>
#include <deque>
#include <queue>
 
 using namespace std;


template<class G>
class CEdge
{
	public:
	typedef typename G::Node Node;
	typedef typename G::e_ E;
	//typedef typename G::Edge E;
	E evalue;
	bool in_S;
	Node *enodo[2]; //Node extremos
	bool mdir; //0[bidireccional] 1[unidireccional]
	CEdge(E date, bool dir){
		this->evalue=date;
		this->mdir=dir;
		enodo[0]=enodo[1]=nullptr;
		in_S=0;
	}
	~CEdge(){};
};

template <class T>
class CNode{
	public:
	typedef typename T::n_ N;
	N value;
	int color;
	list<CNode*> neighbours;
	
	CNode(N date){
		this->value= date;
		color=-1;
	}
	void InsEdge(CNode *b){
		neighbours.push_back(b);
	}
};

template<class N, class E>
class CBipartiteGraph{
	public:
	typedef CBipartiteGraph<N,E>  self;
	typedef CNode<self>  Node;
	typedef CEdge<self>  Edge;
	typedef N n_;
	typedef E e_;
    vector<Node*> mvertices_need_new_color;

    vector<Node*> mleft;
    vector<Node*> mright;
	deque<Node*> W;
	// queue<Node*> W;
	
    bool find_mleft(N, unsigned int &id);
    bool find_mright(N, unsigned int &id);
	void InsNode(N);
	// void dfs();
	void printGraph();
	virtual ~CBipartiteGraph(){
		unsigned int t = mleft.size(), i;
        for(i=0; i<t; i++)  delete mleft[i];
	}

	// VERTEX BASED- COLORING
	void first_queue(){W.push_back(mleft[0]);}
	void greedy_graph_coloring_vertex();
	void color_queue_vertex();
	void remove_conflicts_vertex();

	// NET BASED
	void greedy_graph_coloring_net();
	void color_queue_net();
	void remove_conflicts_net();

	// ALGORITHMS
	void v_v();
	void v_n1();
	void v_n2();
	void n1_n2();
	void n2_n2();
};

template<class N, class E>
bool CBipartiteGraph<N,E>::find_mleft(N d, unsigned int &id) {
	for(id=0; id< mleft.size(); id++){
		if(mleft[id]->value==d)  return 1;
	}
	return 0;
}

template<class N, class E>
bool CBipartiteGraph<N,E>::find_mright(N d, unsigned int &id) {
    for(id=0; id< mright.size(); id++){
		if(mright[id]->value==d)  return 1;
	}
	return 0;
}

template<class N, class E>	
void CBipartiteGraph<N,E>::InsNode(N value){
	Node* new_node= new Node(value);
    if (value % 2 == 0) 
	    mleft.push_back(new_node);
    else
        mright.push_back(new_node);
}

// template<class N>	
// void CBipartiteGraph<N>::dfs(){
// 	if(!q.empty()){
// 		Node *v= q.front();
// 		cout<<"-- "<<v->value;
// 		q.pop_front();
// 		v->color= Node::b;
// 		for (typename list<Node*>::iterator it= v->nets.begin(); it != v->nets.end(); ++it){
// 			if((*it)->color== Node::w){
// 				(*it)->color== Node::g;
// 				q.push_back(*it);
// 			}
// 		}
// 		dfs();
// 	}
// }
	
template<class N, class E>	
void CBipartiteGraph<N,E>::printGraph(){
	for(unsigned int i=0; i<mleft.size(); i++) {//ingreso al vector de nodos del grafo
		Node *tmp_node_left = mleft[i]; // cada nodo
		//cout<<tmp_node->value;// etiqueta-valor de cada nodo

		for (typename list<Node*>::iterator it= tmp_node_left->neighbours.begin(); it != tmp_node_left->neighbours.end(); ++it){
			cout<<" ---> "<< (tmp_node_left)->value<<"("<<(*it)->value<<")";
			cout<<endl;
		}
		cout<<"\n";
	}

	for(unsigned int i=0; i<mright.size(); i++) {//ingreso al vector de nodos del grafo
		Node *tmp_node_right = mright[i]; // cada nodo
		//cout<<tmp_node->value;// etiqueta-valor de cada nodo

        for (typename list<Node*>::iterator it= tmp_node_right->neighbours.begin(); it != tmp_node_right->neighbours.end(); ++it){
			cout<<" ---> "<< (tmp_node_right)->value<<"("<<(*it)->value<<")";
			cout<<endl;
		}
		cout<<"---\n";
	}
}

#endif //BIPARTITE_GRAPH_H