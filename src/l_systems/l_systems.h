#pragma once
#include <godot_cpp/classes/resource.hpp>
#include "l_systems_vm.h"
#include "../spatial/lbh/lbh.h"
#include "aabb.h"

namespace l_systems{
    using namespace godot;
    class LSystem: public Resource {
        GDCLASS(LSystem, Resource);

        public:
            template <size_t N>
            struct GenerationResult {
                spatial::LBH<N> nodes;
                AABB<float, N> bounds;
            };

            LSystem();
            ~LSystem();          

            String get_axiom();
            void set_axiom(const String &p_axiom);

            TypedDictionary<String,String> get_rules();
            void set_rules(const TypedDictionary<String,String> &p_rules);

            int get_iterations();
            void set_iterations(int p_iterations);

            float get_angle();
            void set_angle(float p_angle);

            float get_length();
            void set_length(float p_length);

            PackedByteArray get_byte_code();

            godot::Array generate_2d();
            godot::Array generate_3d();
        
        private:
            LSystemVM vm;
            static void _bind_methods();
            
            template <size_t N>
            GenerationResult<N> generate_leaf_nodes();

            template <size_t N>
            spatial::LBH<N> generate();
    };
}