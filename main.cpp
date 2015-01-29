//
//  main.cpp
//  asa-proj2
//
// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <iostream>
#include <queue>
#include <vector>
#include <string.h>

using namespace std; //Para evitar escrever std antes dos cin
#define INF  2147483647
#define NIL  -1

class Edge
{
private:
	int   _id;
	bool  _flow;// capacidade que um arco pode ter
	Edge* _link;//proximo arco

public:
	Edge(int u, int v);//construtor
	~Edge();

	int   get_id() {
		return _id;
	}
	bool  get_flow() {
		return _flow;
	}
	Edge* get_link() {
		return _link;
	}
	void  set_link(Edge* new_edge) {
		if (_link == NULL) {
			_link = new_edge;
		}
		else {
			_link->set_link(new_edge);
		}
	}
	void  reset_link() {
		_link = NULL;
	}
	void  set_flow(bool  flow){
		_flow = flow;
	}
};
Edge::Edge(int u, int v) : _id(v), _flow(true), _link(NULL)//CONSTRUTOR DO EDGE
{
}
Edge::~Edge()
{
}

class Node
{
private:
	int   _id;//NUMERO ASSOCIADO AO NODO
	Edge* _link;//ARCO SEGUINTE

public:
	Node(int id);
	~Node();

	int   get_id() {
		return _id;
	}
	Edge* get_link() {
		return _link;
	}
	void  set_link(Edge* new_edge) {
		_link = new_edge;
	}
	void  reset_link() {
		_link = NULL;
	}
};
Node::Node(int id) : _id(id), _link(NULL)
{
}
Node::~Node()
{
}

class Edmond_Karp
{
private:
	int                 _max_node;       //NODO MAXIMO
	int                 _max_link;       //ARCO MAXIMO
	vector<Node*>       _node_list;      //LISTA DE ADJACENCIAS
	vector<Edge*>       _node_index;     //GRAVAR A POSICAO NA LISTA DE ADJACENCIAS
	vector<Edge*>       _node_index_2;   //GRAVAR A POSICAO NA LISTA DE ADJACENCIAS

	int                 _max_case;       //NUMERO DE PROBLEMAS
	vector<int>         _max_critical;   //NUMERO DE CRITICOS
	vector<vector<int> > _critical_list;  //LISTA DE CRITICOS

	int                 _max_flow;       //FLUXO MÁXIMO
	int                 _source;
	int                 _target;

	vector<int>         _precedent;      //PRECEDENCIA
	vector<bool>        _discovered;     //DESCOBERTOS
	vector<Node*>       _exclusion_list; //ARCOS SATURADOS

public:
	Edmond_Karp();
	~Edmond_Karp();

	// BASE DO KARP
	// Max Flow Algorithm

	int get_max_case(){
		return _max_case;
	}

	int     get_max_node() {
		return _max_node;
	}
	int     get_max_link() {
		return _max_link;
	}
	vector<Node*> get_node_list() {
		return _node_list;
	}
	Edge *  get_link(int key) {
		return _node_list[key]->get_link();
	}
	Edge *  get_link(int u, int v) {
		Edge* link = _node_list[u]->get_link();
		while (link->get_id() != v) {
			link = link->get_link();
		}
		return link;
	}
	Edge *  get_e_link(int u, int v) {
		Edge* link = _exclusion_list[u]->get_link();
		while (link != NULL && link->get_id() != v) {
			link = link->get_link();
		}
		return link;
	}

	/* Antes de mais,terá que verificar se existe um caminho entre os vertices problemáticos,que adaptaremos como sendo a nossa source e nosso destino(sink)
	grava em parents[] o caminho percorrido */

	bool Breath_First_Search()
	{
		queue< int >   stack;
		int u,v;

		for( int aux = 0 ; aux < _max_node ; aux++ ) _precedent[aux]=NIL;
		_precedent [_source] = -2;
		for( int aux = 0 ; aux < _max_node ; aux++ ) _discovered[aux]=false;
		_discovered[_source] = true;

		stack.push(_source);
		do {
			u = stack.front();
			stack.pop();
			for (Edge* aux = get_link(u); aux != NULL; aux = aux->get_link()) {
				v = aux->get_id();
				if (_discovered[v] == false && get_e_link(u,v) == NULL) {
					_precedent [v] = u;
					_discovered[v] = true;
					stack.push(v);
				}
			}
		} while (!stack.empty());

		return _discovered[_target];
	}
	int magiKarp(int index) {
		int critical_index = _max_critical[index];
		_max_flow = INF;
		_source = _critical_list[index][0];
		for (int aux = 1; aux < critical_index; aux++) {
			_target = _critical_list[index][aux];
			_max_flow = min(Karp(), _max_flow);
		}
		return _max_flow;
	}
	int Karp()
	{
		int max_flow = 0;
		Edge* aux = NULL;
		bool path_flow;
		
		for( int aux2 = 0 ; aux2 < _max_node ; aux2++ ) {
			aux = _node_list[aux2]->get_link();
			while( aux != NULL ) {
				aux->set_flow( true );
				aux = aux->get_link();
			}
		}

		for( int aux2 = 0 ; aux2 < _max_node ; aux2++ ) {
			_exclusion_list[aux2]->reset_link();
		}
		_node_index_2.clear();
		_node_index_2.resize(_max_node);

		while (Breath_First_Search()) {
			// A partir daqui procura a maximum flow dado o caminho previamente encontrado.
			path_flow = true;
			for (int v = _target; v != _source; v = _precedent[v]) {
				int u = _precedent[v];

				aux = get_link(u, v);
				if(aux->get_flow()) continue;
				path_flow = false;
				break;
			}

			// Actualiza a capacidade dos edges, e das edges invertidas (1 para 2 e 2 para 1)
			if (path_flow) {
				for (int v = _target; v != _source; v = _precedent[v]) {
					int u = _precedent[v];

					aux = get_link(u, v);
					aux->set_flow(true);
					if (_node_index_2[u] == NULL) {
						_exclusion_list[u]->set_link(new Edge(u, v));
						_node_index_2[u] = _exclusion_list[u]->get_link();
					}
					else {
						_node_index_2[u]->set_link(new Edge(u, v));
					}

					aux = get_link(v, u);
					aux->set_flow(false);
					if (_node_index_2[v] == NULL) {
						_exclusion_list[v]->set_link(new Edge(u, v));
						_node_index_2[v] = _exclusion_list[v]->get_link();
					}
					else {
						_node_index_2[v]->set_link(new Edge(u, v));
					}
					
				}
				max_flow++;
			}
			if(max_flow == _max_flow){
				break;
			}
		}
		// Retorna o flow maximo ...
		return max_flow;
	}
};
Edmond_Karp::Edmond_Karp()
{
	int     u, v;

	cin >> _max_node >> _max_link;

	_node_list.resize(_max_node);
	_exclusion_list.resize(_max_node);
	_node_index.resize(_max_node);
	_node_index_2.resize(_max_node);
	_precedent.resize(_max_node);
	_discovered.resize(_max_node);

	for( int aux = 0 ; aux < _max_node ; aux++ ) {
		_node_list [ aux ] = new Node( aux );
		_exclusion_list [ aux ] = new Node( aux );
	}

	_node_index.clear();
	_node_index_2.clear();
	for( int aux = 0 ; aux < _max_node ; aux++ ) _precedent[aux]=NIL;
	for( int aux = 0 ; aux < _max_node ; aux++ ) _discovered[aux]=false;
	
	
	for (int aux = 0; aux < _max_link; aux++) {
		cin >> u >> v;

		if (_node_index[u] == NULL) {
			_node_list[u]->set_link(new Edge(u, v));
			_node_index[u] = _node_list[u]->get_link();
		}
		else {
			_node_index[u]->set_link(new Edge(u, v));
		}

		if (_node_index[v] == NULL) {
			_node_list[v]->set_link(new Edge(v, u));
			_node_index[v] = _node_list[v]->get_link();
		}
		else {
			_node_index[v]->set_link(new Edge(v, u));
		}
	}

	cin >> _max_case;
	_max_critical.resize(_max_case);
	_critical_list.resize(_max_case);
	for (int aux = 0; aux < _max_case; aux++) {
		cin >> _max_critical[aux];
		_critical_list[aux].resize(_max_critical[aux]);
		for (int aux2 = 0; aux2 < _max_critical[aux]; aux2++) {
			cin >> _critical_list[aux][aux2];
		}
	}

	for( int aux = 0 ; aux < _max_node ; aux++ ) _precedent[aux]=NIL;
}
Edmond_Karp::~Edmond_Karp()
{
}

int main(int argc, char* argv[])
{
	Edmond_Karp* graph = new Edmond_Karp();
	int max_case = graph->get_max_case();
	for (int aux = 0; aux < max_case; aux++) {
		cout << graph->magiKarp(aux) << endl;
	}
	return 0;
}
