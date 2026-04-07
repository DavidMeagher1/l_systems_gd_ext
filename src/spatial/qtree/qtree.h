
#pragma once

#include "../../types.h"
#include <vector>
#include <godot_cpp/variant/vector2i.hpp>
#include <godot_cpp/variant/rect2i.hpp>

namespace l_systems::spatial {
    struct QCell {
        int x;
        int y;
        q16norm_t u;
        q16norm_t v;
    };

    enum Quadrant {
        NW = 0,
        NE = 1,
        SW = 2,
        SE = 3
    };

    class QNode {
        public:
            QNode();
            ~QNode();

            unsigned int parent = 0;
            
            unsigned int children[4] = {0, 0, 0, 0};
            QCell data;
        private:
    };

    class QTree {
        public:
            QTree();
            ~QTree();
            std::vector<QNode> nodes;

            void insert(const QCell &cell);
            void subdivide(unsigned int node_index);
            godot::Rect2i get_bounds();
            void clear();

        private:
    };
}