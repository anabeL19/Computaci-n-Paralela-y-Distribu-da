#ifndef GRAPH_H
#define GRAPH_H
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
class CGraph{
	public:
	typedef CGraph<N,E>  self;
	typedef CNode<self>  Node;
	typedef CEdge<self>  Edge;
	typedef N n_;
	typedef E e_;
	vector<Node*> mvertices_need_new_color;
    vector<Node*> mnodes;
	deque<Node*> W;
	// queue<Node*> W;
	
	bool find(N, unsigned int &id);
	void InsNode(N);
	// void dfs();
	void printGraph();
	virtual ~CGraph(){
		unsigned int t = mnodes.size(), i;
        for(i=0; i<t; i++)  delete mnodes[i];
	}

	// VERTEX BASED- COLORING
	void first_queue(){W.push_back(mnodes[0]);}
	// void first_queue(){W.push(mnodes[0]);}
	void color_queue_vertex();
	void remove_conflicts_vertex();

	// NET BASED
	void greedy_graph_coloring_net();
	void color_queue_net();
	void remove_conflicts_net();

	// ALGORITHMS
	void v_n1();
	void v_n2();
	void n1_n2();
};

template<class N, class E>
bool CGraph<N,E>::find(N d, unsigned int &id){
	for(id=0; id< mnodes.size(); id++){
		if(mnodes[id]->value==d)  return 1;
	}
	return 0;
}

template<class N, class E>	
void CGraph<N,E>::InsNode(N value){
	Node* new_node= new Node(value);
	mnodes.push_back(new_node);
}

// template<class N>	
// void CGraph<N>::dfs(){
// 	if(!q.empty()){
// 		Node *v= q.front();
// 		cout<<"-- "<<v->value;
// 		q.pop_front();
// 		v->color= Node::b;
// 		for (typename list<Node*>::iterator it= v->neighbours.begin(); it != v->neighbours.end(); ++it){
// 			if((*it)->color== Node::w){
// 				(*it)->color== Node::g;
// 				q.push_back(*it);
// 			}
// 		}
// 		dfs();
// 	}
// }
	
template<class N, class E>	
void CGraph<N,E>::printGraph(){
	for(unsigned int i=0; i<mnodes.size(); i++) {//ingreso al vector de nodos del grafo
		Node *tmp_node = mnodes[i]; // cada nodo
		//cout<<tmp_node->value;// etiqueta-valor de cada nodo

		for (typename list<Node*>::iterator it= tmp_node->neighbours.begin(); it != tmp_node->neighbours.end(); ++it){
			cout<<" ---> "<< (tmp_node)->value<<"("<<(*it)->value<<")";
			cout<<endl;
		}
		cout<<"\n";
	}
}

#endif //GRAPH_H