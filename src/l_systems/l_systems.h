#pragma once
#include <godot_cpp/classes/resource.hpp>
#include "l_systems_vm.h"
#include "core/lbh.h"
#include "core/aabb.h"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/dictionary.hpp>

namespace procgen::l_systems {
    class LSystem: public godot::Resource {
        GDCLASS(LSystem, godot::Resource);

        public:
            template <size_t N>
            struct GenerationResult {
                lbh::LBH<N> nodes;
                AABB<float, N> bounds;
            };

            LSystem();
            ~LSystem();          

            godot::String get_axiom();
            void set_axiom(const godot::String &p_axiom);

            godot::TypedDictionary<godot::String,godot::String> get_rules();
            void set_rules(const godot::TypedDictionary<godot::String,godot::String> &p_rules);

            int get_iterations();
            void set_iterations(int p_iterations);

            float get_angle();
            void set_angle(float p_angle);

            float get_length();
            void set_length(float p_length);

            godot::PackedByteArray get_byte_code();

            // Generate hierarchy output and execute callback methods on p_context_node.
            godot::Array generate_2d(godot::Node *p_context_node);
            godot::Array generate_3d(godot::Node *p_context_node);

            // Mapping: symbol -> method name called on the context node.
            godot::TypedDictionary<godot::String, godot::String> get_opcode_callbacks();
            void set_opcode_callbacks(const godot::TypedDictionary<godot::String, godot::String> &p_opcode_callbacks);

            // Initial callback payload; propagated to future segments as extra_data.
            godot::Dictionary get_data();
            void set_data(const godot::Dictionary &p_data);

        private:
            LSystemVM vm;
            static void _bind_methods();
            
            template <size_t N>
            GenerationResult<N> generate_leaf_nodes(godot::Node *p_context_node);

            template <size_t N>
            lbh::LBH<N> generate(godot::Node *p_context_node);

            godot::Dictionary data;
    };
}