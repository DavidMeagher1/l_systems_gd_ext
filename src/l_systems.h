#pragma once
#include <godot_cpp/classes/resource.hpp>
#include "l_system_vm.h"

namespace l_systems{
    using namespace godot;
    class LSystem: public Resource {
        GDCLASS(LSystem, Resource);

        public:
            LSystem();
            ~LSystem();

            int get_mode();
            void set_mode(int p_mode);            

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
        
        private:
            LSystemVM vm;
            static void _bind_methods();
    };
}