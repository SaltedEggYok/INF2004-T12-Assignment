#include <stdio.h>
#include <stdlib.h>

// #include <vector>

// struct Node {
//     int x;
//     int y;
//     std::vector<Node*> neighbors;
// };

// typedef struct {
// public:
//     void addNode(int x, int y);
//     void connectNodes(Node* node1, Node* node2);
//     std::vector<Node*> findPath(Node* start, Node* end);
// private:
//     std::vector<Node*> nodes;
// } Pathfinding;

struct Node {
    int x;
    int y;
    struct Node** neighbors;
    size_t neighbor_count;
};

struct Pathfinding {
    struct Node* nodes;
    size_t node_count;
};

// Function to add a new node to the Pathfinding struct.
void addNode(struct Pathfinding* pf, int x, int y) {
    pf->nodes = (struct Node*)realloc(pf->nodes, (pf->node_count + 1) * sizeof(struct Node));
    struct Node* newNode = &pf->nodes[pf->node_count];
    newNode->x = x;
    newNode->y = y;
    newNode->neighbors = NULL;
    newNode->neighbor_count = 0;
    pf->node_count++;
}

// Function to connect two nodes by adding them as neighbors.
void connectNodes(struct Pathfinding* pf, struct Node* node1, struct Node* node2) {
    size_t neighbor_count = node1->neighbor_count;
    node1->neighbors = (struct Node**)realloc(node1->neighbors, (neighbor_count + 1) * sizeof(struct Node*));
    node1->neighbors[neighbor_count] = node2;
    node1->neighbor_count++;

    neighbor_count = node2->neighbor_count;
    node2->neighbors = (struct Node**)realloc(node2->neighbors, (neighbor_count + 1) * sizeof(struct Node*));
    node2->neighbors[neighbor_count] = node1;
    node2->neighbor_count++;
}

// Function to find a path from 'start' to 'end'.
// Implement your pathfinding algorithm here.

int main() {
    // Create a Pathfinding instance.
    struct Pathfinding pf;
    pf.nodes = NULL;
    pf.node_count = 0;

    // Add nodes and connect them.
    addNode(&pf, 0, 0);
    addNode(&pf, 1, 1);
    connectNodes(&pf, &pf.nodes[0], &pf.nodes[1]);

    // Implement pathfinding logic.

    // Free allocated memory.
    for (size_t i = 0; i < pf.node_count; i++) {
        free(pf.nodes[i].neighbors);
    }
    free(pf.nodes);

    return 0;
}