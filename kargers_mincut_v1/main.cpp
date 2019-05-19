#include <iostream>
#include "vector"
#include <random>
#include <array>
#include <algorithm>
#include <set>

#define NB_ITERATION 200

using namespace std;

class Node {
public:
    explicit Node(int label){
        _parent = this;
        _rank = 0;
        _label = label;
    }

    Node* _parent;
    int _label;
    unsigned int _rank;
};

class Graph {
public:
    int _nb_v, _nb_e;
    int mincut;

    set< vector<bool> > hashes;
    vector< array<int, 2> > edges;


    // Creates a graph with V vertices and E edges
    Graph(int V, int E)
    {
        this->_nb_v = V;
        this->_nb_e = E;

        this->mincut = INT16_MAX;
    }
};

Node* my_find(Node* x) {
    if (x->_parent != x)
        x->_parent = my_find(x->_parent);

    return x->_parent;
}

void my_union(Node* x, Node* y) {
    Node* x_prev = my_find(x);
    Node* y_prev = my_find(y);

    if(x_prev != y_prev) {
        if (x_prev->_rank < y_prev->_rank) {
            x_prev->_parent = y_prev;
        } else {
            y_prev->_parent = x_prev;

            if (x_prev->_rank == y_prev->_rank)
                x_prev->_rank++;
        }
    }
}

int karger_mincut(Graph* graph){
    int size_mincut = 0;
    int nb_vertex = graph->_nb_v, nb_edges = graph->_nb_e;

    vector<bool> hash((unsigned long) nb_vertex);
    vector<array<int, 2>> edges = graph->edges;

    int p = 400;
    int rnd = 0;
    random_device rd;
    uniform_int_distribution<int> dist(0, p);

    vector<Node*> list_nodes;

    for(int v = 0; v < nb_vertex; v++) {
        list_nodes.push_back(new Node(v));
    }

    while(nb_vertex > 2) {
        rnd = dist(rd) % nb_edges;

        Node* v1 = my_find(list_nodes.at(static_cast<unsigned long>(edges[rnd][0])));
        Node* v2 = my_find(list_nodes.at(static_cast<unsigned long>(edges[rnd][1])));

        if(v1 != v2) {
            nb_vertex--;
            my_union(v1, v2);
        }
    }

    // Find mincut
    for(int e = 0; e < nb_edges; e++) {
        Node *v1 = my_find(list_nodes.at(static_cast<unsigned long>(edges[e][0])));
        Node *v2 = my_find(list_nodes.at(static_cast<unsigned long>(edges[e][1])));

        if (v1 != v2)
            size_mincut++;
    }

    // Find nb of mincut
    for(int v = 0; v < graph->_nb_v; v++) {
        Node *v0 = my_find(list_nodes.at(0));
        Node *v1 = my_find(list_nodes.at(static_cast<unsigned long>(v)));

        hash[v] = v1 == v0;
    }


    if (size_mincut < graph->mincut) {
        graph->mincut = size_mincut;
        graph->hashes.clear();
        graph->hashes.insert(hash);
    } else if (size_mincut == graph->mincut){
        graph->hashes.insert(hash);
    }

    return graph->mincut;
};

int main() {
    vector<int> results;
    int n_vertices, m_edges;

    // store number of vertices/edges and create graph
    cin >> n_vertices >> m_edges;
    auto graph = new Graph(n_vertices, m_edges);

    for(int i = 0; i < m_edges; i++) {
        array<int, 2> edge{};
        int source, destination;
        cin >> source >> destination;

        edge[0] = source - 1;
        edge[1] = destination - 1;
        graph->edges.push_back(edge);
    }

    // Karger's mincut part
    vector<array<int, 2>> rest_edge{};

    for (int iter = 0; iter < 2; iter++){
        rest_edge.clear();

        for(int i = 0; i < NB_ITERATION; i++)
            results.push_back(karger_mincut(graph));
    }

    cout << graph->mincut << ' ' << graph->hashes.size() << endl;

    return 0;
}
