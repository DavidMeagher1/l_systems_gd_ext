#include "qtree.h"

using namespace l_systems;
using namespace l_systems::spatial;
using namespace godot;

QTree::QTree() {
    nodes.push_back(QNode()); // Start with a root node
}

QTree::~QTree() {
}

void QTree::insert(const QCell &cell) {
    // Implementation of insert function

}

void QTree::subdivide(unsigned int node_index) {
    // Implementation of subdivide function

}

godot::Rect2i QTree::get_bounds() {
    // Implementation of get_bounds function
    QNode nw_cell = nodes[0];
    QNode se_cell = nodes[0];
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

void QTree::clear() {
    nodes.clear();
}

QNode::QNode() {
}

QNode::~QNode() {
}
