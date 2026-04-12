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

            int get_sdf_type();
            void set_sdf_type(int p_sdf_type);

            float get_sdf_width();
            void set_sdf_width(float p_sdf_width);

            godot::PackedByteArray get_byte_code();

            // Generate hierarchy output and execute callback methods on p_context_node.
            // Callback signature: method_name(l_system_info: Dictionary, extra_data: Dictionary)
            // l_system_info carries mutable runtime values (currently angle, length, sdf_type, and sdf_width).
            // Changes are applied to following operations and are preserved/restored by PUSH/POP.
            // Returned node dictionaries include bounds, p1, p2, sdf_type, sdf_width, extra_data, and child indices.
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
            lbh::SDFType sdf_type = lbh::SDFType::LINE;
            float sdf_width = 0.0f;
    };
}