#include <iostream>
#include "vector"
#include <random>
#include <array>
#include <algorithm>

#define NB_ITERATION 100

using namespace std;

class Node {
public:
    explicit Node(){
        _parent = this;
        _rank = 0;
    }

    Node* _parent;
    unsigned int _rank;
};

struct Graph {
    int _nb_v, _nb_e;
    vector<array<int, 2>> edges;
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

// Creates a graph with V vertices and E edges
struct Graph* create_graph(int V, int E)
{
    Graph* graph = new Graph;

    graph->_nb_v = V;
    graph->_nb_e = E;
    return graph;
}

int karger_mincut(struct Graph* graph){
    int mincut = 0;
    int nb_vertex = graph->_nb_v, nb_edges = graph->_nb_e;
    vector<array<int, 2>> edges = graph->edges;

    int p = 400;
    int rnd = 0;
    random_device rd;
    uniform_int_distribution<int> dist(0, p);

    vector<Node*> list_nodes;

    for(int v = 0; v < nb_vertex; v++) {
        list_nodes.push_back(new Node());
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

    mincut = 0;
    for(int e = 0; e < nb_edges; e++) {
        Node* v1 = my_find(list_nodes.at(static_cast<unsigned long>(edges[e][0])));
        Node* v2 = my_find(list_nodes.at(static_cast<unsigned long>(edges[e][1])));

        if (v1 != v2)
            mincut++;
    }

    return mincut;
};

int main() {
    vector<int> results;
    int n_vertices, m_edges;

    // store number of vertices/edges and create graph
    cin >> n_vertices >> m_edges;
    struct Graph* graph = create_graph(n_vertices, m_edges);

    for(int i = 0; i < m_edges; i++) {
        array<int, 2> edge{};
        int source, destination;
        cin >> source >> destination;

        edge[0] = source - 1;
        edge[1] = destination - 1;
        graph->edges.push_back(edge);
    }

    // Karger's mincut part
    for(int i = 0; i < NB_ITERATION; i++) {
        int rest_mincut, nb_mincut = 0, tmp_nb_mincut = 0, mincut = 0, idx = 0;
        results.push_back(karger_mincut(graph));

        rest_mincut = (int) results.size();
        while(rest_mincut > 0) {
            tmp_nb_mincut = (int) count(results.begin(), results.end(), idx);

            if(tmp_nb_mincut > nb_mincut) {
                nb_mincut = tmp_nb_mincut;
                mincut = idx;
            }

            rest_mincut -= nb_mincut;
            idx++;
        }

        cout << mincut << ' ' << 10 << endl;
    }

    return 0;
}
