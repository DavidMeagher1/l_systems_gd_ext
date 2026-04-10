#pragma once
#include <godot_cpp/variant/typed_dictionary.hpp>

namespace l_systems {
    using namespace godot;

    enum Mode {
        MODE_2D,
        MODE_3D
    };

    enum Opcodes {
        DONE = 0,
        FORWARD,
        MOVE,
        LEFT,
        RIGHT,
        UP,
        DOWN,
        PUSH,
        POP,
    };

    static const char *opcode_glyphsp[]{
        "",
        "F",
        "M",
        "-",
        "+",
        "^", // 3d only
        "v", // 3d only
        "[",
        "]"
    };

    class LSystemVM {
        public:
            LSystemVM();
            ~LSystemVM();


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
            PackedByteArray generate();

        private:
            // Add private members and methods here
            TypedDictionary<String,String> rules;
            String axiom = "";
            int iterations = 0;
            float angle = 0.0f;
            float length = 0.0f;
            String iterate();
    };
}