
#pragma once

#include "../types.h"
#include <vector>
#include <godot_cpp/variant/vector2i.hpp>
#include <godot_cpp/variant/rect2i.hpp>

namespace l_systems{
    namespace spatial {
        struct Cell {
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

        class Node {
            public:
                Node();
                ~Node();

                unsigned int parent = 0;
                
                unsigned int children[4] = {0, 0, 0, 0};
                Cell data;
            private:
        };

        class Tree {
            public:
                Tree();
                ~Tree();
                std::vector<Node> nodes;

                void insert(const Cell &cell);
                void subdivide(unsigned int node_index);
                godot::Rect2i get_bounds();
                void clear();

            private:
        };
    }
}