#pragma once
#include <godot_cpp/variant/typed_dictionary.hpp>

namespace procgen::l_systems {
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
        CLEAR_EXTRA_DATA, // Special opcode to clear the callback extra data without invoking a callback
        CALLBACK,
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
        "]",
        "!", // Clear extra data
    };

    class LSystemVM {
        public:
            LSystemVM();
            ~LSystemVM();


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

            godot::PackedByteArray generate();

            godot::TypedDictionary<godot::String, godot::String> get_opcode_callbacks();
            void set_opcode_callbacks(const godot::TypedDictionary<godot::String, godot::String> &p_opcode_callbacks);
            godot::String get_callback_name_by_id(int p_callback_id);

        private:
            // Add private members and methods here
            godot::TypedDictionary<godot::String,godot::String> rules;
            godot::String axiom = "";
            int iterations = 0;
            float angle = 0.0f;
            float length = 0.0f;
            godot::TypedDictionary<godot::String, godot::String> opcode_callbacks;
            godot::String iterate();
            int get_callback_index(const godot::String &glyph);
            godot::Array get_callback_names_by_id();
    };
}