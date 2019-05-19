// includes
#include <algorithm>  // for random_shuffle
#include <cmath>      // for log
#include <cstdlib>    // for srand
#include <ctime>      // for time
#include <iostream>   // for I/O
#include <set>        // for set for storing cut hashes
#include <sstream>    // for Input parsing
#include <vector>     // for lists of edges

using namespace std;

class Node {
 public:
  Node* parent;  //!< pointer to parent node in set
  int label;          //!< label of node
  int rank;  //!< rank of node in set (estimated number of elements in set)

  Node(int label_ = 0);
  void reset(Node* myaddress = nullptr);
  Node* my_find();
};

struct Edge {
  Node* node1;  //!< pointer to 1st node connected to edge
  Node* node2;  //!< pointer to 2nd node connected to edge
};

class Graph {
 public:
  int nnodes;  //!< # of nodes
  int nedges;  //!< # of edges

  vector<Edge> edges;        //!< list of edges
  vector<Node> nodes;   //!< list of nodes
  int mincut;                //!< value of mincut
  set<vector<bool>> hashes;  //!< hashes for different cuts

  Graph();
  int krager();
  void reset();
  vector<bool> hashCut();
};

Node::Node(int label_) : parent(this), label(label_), rank(1) {}

void Node::reset(Node* myaddress) {
  // make node seperate set
  if (myaddress == nullptr) {
    parent = this;
  } else {
    parent = myaddress;
  }
  rank = 1;  // set of only one element
  return;
}

Node* Node::my_find() {
    if(this->_parent != this)
        this->_parent = this->_parent->my_find();

    return this->_parent;
}

void sn_union(Node* node1, Node* node2) {
  node1 = node1->my_find();
  node2 = node2->my_find();

  if (node1->rank == node2->rank) {
    node2->parent = node1;
    node1->rank++;
  } else if (node1->rank < node2->rank) {
    node1->parent = node2;
  } else {
    node2->parent = node1;
  }
}

Graph::Graph() : hashes() {
  istringstream iss;
  string line;

  getline(cin, line);
  iss = istringstream(line);
  // read number of nodes and edges
  iss >> nnodes >> nedges;
  mincut = nedges;

  // initialize list of nodes and edges
  nodes = vector<Node>(nnodes);
  edges = vector<Edge>(nedges);

  // assign correct labels to nodes (supposes node labels in {1,...,n})
  for (int i = 0; i < nnodes; i++) {
    nodes[i].label = i + 1;
  }

  int label1, label2;
  for (int i = 0; i < nedges; i++) {
    getline(cin, line);
    iss = istringstream(line);
    // read edge
    iss >> label1 >> label2;
    // initialize edge
    edges[i] = {.node1 = &(nodes[label1 - 1]), .node2 = &(nodes[label2 - 1])};
  }
}

int Graph::krager() {
  // shuffle edges
  random_shuffle(edges.begin(), edges.end());
  int nsets = nnodes;
  int i(0);
  while (nsets > 2 && i < nedges) {
    // contract edge if it connects two different sets
    if (edges[i].node1->my_find() != edges[i].node2->my_find()) {
      sn_union(edges[i].node1, edges[i].node2);
      nsets--;
    }
    i++;
  }
  int sizemc(0);
  // compute cut size as number of edges between distinct sets
  for (int j(i); j < nedges; j++) {
    sizemc += (edges[j].node1->my_find() != edges[j].node2->my_find());
  }
  if (sizemc < mincut) {
    mincut = sizemc;
    hashes.clear();
    hashes.insert(hashCut());
  } else if (sizemc == mincut) {
    hashes.insert(hashCut());
  }
  reset();
  return sizemc;
}

void Graph::reset() {
  // reset all nodes
  for (int i = 0; i < nnodes; i++) {
    nodes[i].reset(&nodes[i]);
  }
  return;
}

vector<bool> Graph::hashCut() {
  vector<bool> hash(nnodes);

  for (int i = 0; i < nnodes; i++) {
    if (nodes[i].my_find() == nodes[0].my_find()) {
      hash[i] = 1;
    } else {
      hash[i] = 0;
    }
  }
  return hash;
}

int main(int argc, char** argv) {
  // set seed
  srand(time(0));
  // create graph
  Graph graph;
  // graph.print();

  // compute number of tries to reach target prob (n^2*log n) for one mincut
  // compute number of tries to reach target prob (n^2*log^3 n) for all mincuts
  size_t ntries = graph.nnodes * graph.nnodes * log(graph.nnodes) *
                  log(graph.nnodes) * log(graph.nnodes);

  // printf("pour %zu essais\n", ntries);

  for (size_t i = 0; i < ntries; i++) {
    /*if(i%10000==0) {
        printf("%zu\n", i);
    }*/

    // my_find possible min cut
    graph.krager();
  }
  // print result
  printf("%d %d", graph.mincut, (int)graph.hashes.size());
  return 0;
  // blob
}