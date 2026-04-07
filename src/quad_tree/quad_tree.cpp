#include "quad_tree.h"

using namespace l_systems;
using namespace l_systems::spatial;
using namespace godot;

Tree::Tree() {
    nodes.push_back(Node()); // Start with a root node
}

Tree::~Tree() {
}

void Tree::insert(const Cell &cell) {
    // Implementation of insert function

}

void Tree::subdivide(unsigned int node_index) {
    // Implementation of subdivide function

}

Rect2i Tree::get_bounds() {
    // Implementation of get_bounds function
    Node nw_cell = nodes[0];
    Node se_cell = nodes[0];
    int min_x = nw_cell.data.x;
    int max_x = se_cell.data.x;
    int min_y = nw_cell.data.y;
    int max_y = se_cell.data.y;
    while (nw_cell.children[Quadrant::NW] != 0 && se_cell.children[Quadrant::SE] != 0) {
        nw_cell = nodes[nw_cell.children[Quadrant::NW]];
        se_cell = nodes[se_cell.children[Quadrant::SE]];
        min_x = std::min(min_x, nw_cell.data.x);
        min_y = std::min(min_y, nw_cell.data.y);
        max_x = std::max(max_x, se_cell.data.x);
        max_y = std::max(max_y, se_cell.data.y);
    }
    return Rect2i(Vector2i(min_x, min_y), Vector2i(max_x - min_x, max_y - min_y));
}

void Tree::clear() {
    nodes.clear();
}

Node::Node() {
}

Node::~Node() {
}
