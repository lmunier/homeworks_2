#include <random>

#include <cstdlib> 		// for srand
#include <ctime>		// for time
#include <iostream> 	// for I/O
#include <sstream>		// for Input parsing
#include <vector> 		// for lists of edges
#include <algorithm> 	// for random_shuffle
#include <cmath> 		// for log
#include <set> 			// for set for storing cut hashes

using namespace std;

class Node {
public:
    Node* _parent;
    int _label;
    int _rank;

    explicit Node( int label_ = 0 ) : _parent(this), _label(label_), _rank(1){};
    void reset(Node* myaddress = nullptr );
    Node* my_find();
};

struct Edge {
    Node* node1;
    Node* node2;
};

class Graph {
public:
    int _nb_v, _nb_e;
    int mincut;

    vector<Edge> edges;
    vector<Node> nodes;
    set<vector<bool>> hashes;

    Graph() : hashes() {
        istringstream iss;
        string line;

        // store number of vertices/edges and create graph
        getline(cin,line);
        iss = istringstream(line);
        iss >> this->_nb_v >> this->_nb_e;
        this->mincut = this->_nb_e;

        // initialize list of nodes and edges
        this->nodes = vector<Node>(static_cast<unsigned long>(this->_nb_v));
        this->edges = vector<Edge>(static_cast<unsigned long>(this->_nb_e));

        int source, destination;
        for (int i = 0; i < _nb_e; i++) {
            getline(cin,line);
            iss = istringstream(line);
            iss >> source >> destination;

            this->edges[i] = {.node1=&(this->nodes[source-1]), .node2=&(this->nodes[destination-1])};
        }

        for (int i = 0; i < this->_nb_v; i++)
            nodes[i]._label = i + 1;
    };

    int krager();
    void reset();
    vector<bool> hashCut();
};

void Node::reset(Node* myaddress) {
    if ( myaddress == nullptr ) {
        _parent = this;
    } else {
        _parent = myaddress;
    }

    _rank = 1;
}

Node* Node::my_find() {
    if(this->_parent != this)
        this->_parent = this->_parent->my_find();

    return this->_parent;
}

void my_union(Node* x, Node* y) {
    x = x->my_find();
    y = y->my_find();

    if (x->_rank < y->_rank) {
        x->_parent = y;
    } else {
        y->_parent = x;

        if (x->_rank == y->_rank)
            x->_rank++;
    }
}

int Graph::krager() {
    int i = 0;
    int nsets = this->_nb_v;
    random_shuffle(this->edges.begin(), this->edges.end());

    while( nsets > 2 && i < this->_nb_e ){
        if( this->edges[i].node1->my_find() != this->edges[i].node2->my_find() ){
            my_union(this->edges[i].node1, this->edges[i].node2);
            nsets--;
        }

        i++;
    }

    // Find mincut
    int size_mincut = 0;
    for(int j = i; j < this->_nb_e; j++ )
        size_mincut += ( this->edges[j].node1->my_find() != this->edges[j].node2->my_find() );

    if( size_mincut < this->mincut ) {
        this->mincut = size_mincut;
        this->hashes.clear();
        this->hashes.insert(this->hashCut());
    } else if ( size_mincut == this->mincut ) {
        this->hashes.insert(this->hashCut());
    }

    reset();
    return size_mincut;
}

vector<bool> Graph::hashCut() {
    vector<bool> hash(static_cast<unsigned long>(_nb_v));

    for (int i = 0; i < _nb_v; i++)
        hash[i] = (nodes[i].my_find() == nodes[0].my_find());

    return hash;
}

void Graph::reset() {
    for( int i = 0; i < _nb_v; i++ )
        nodes[i].reset(&nodes[i]);
}

int main(int argc, char** argv) {
    srand(static_cast<unsigned int>(time(nullptr)));
    auto graph = new Graph();

    int ntries = (int)(graph->_nb_v * graph->_nb_v * log(graph->_nb_v) * log(graph->_nb_v) * log(graph->_nb_v));
    for(int i = 0; i < ntries; i++ )
        graph->krager();

    cout << graph->mincut << ' ' << (int) graph->hashes.size() << endl;
    return 0;
}
