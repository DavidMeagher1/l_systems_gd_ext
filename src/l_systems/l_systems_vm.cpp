#include "l_systems_vm.h"

using namespace l_systems;

LSystemVM::LSystemVM() {
}

LSystemVM::~LSystemVM() {
}

Mode LSystemVM::get_mode() {
    return mode;
}

void LSystemVM::set_mode(Mode p_mode) {
    mode = p_mode;
}

String LSystemVM::get_axiom() {
    return axiom;
}

void LSystemVM::set_axiom(const String &p_axiom) {
    axiom = p_axiom;
}

TypedDictionary<String,String> LSystemVM::get_rules() {
    return rules;
}

void LSystemVM::set_rules(const TypedDictionary<String,String> &p_rules) {
    rules = p_rules;
}

int LSystemVM::get_iterations() {
    return iterations;
}

void LSystemVM::set_iterations(int p_iterations) {
    iterations = p_iterations;
}

float LSystemVM::get_angle() {
    return angle;
}

void LSystemVM::set_angle(float p_angle) {
    angle = p_angle;
}

float LSystemVM::get_length() {
    return length;
}

void LSystemVM::set_length(float p_length) {
    length = p_length;
}

String LSystemVM::iterate() {
    String current_string = axiom;
    for (int i = 0; i < iterations; i++) {
        String new_string;
        for (int j = 0; j < current_string.length(); j++) {
            String symbol = current_string.substr(j, 1);
            if (rules.has(symbol)) {
                new_string += rules[symbol];
            } else {
                new_string += symbol;
            }
        }
        current_string = new_string;
    }
    return current_string;
}

bool is_only_3d_opcode(const String &symbol) {
    for (int k = 0; k < sizeof(only_3d_opcodes) / sizeof(only_3d_opcodes[0]); k++) {
        if (symbol == only_3d_opcodes[k]) {
            return true;
        }
    }
    return false;
}

bool is_structural_action(const String &symbol) {
    for (int k = 0; k < sizeof(structural_action_glyphs) / sizeof(structural_action_glyphs[0]); k++) {
        if (symbol == structural_action_glyphs[k]) {
            return true;
        }
    }
    return false;
}

PackedByteArray LSystemVM::generate() {
    String result = iterate();
    structural_action_count = 0;
    PackedByteArray byte_array;
    byte_array.resize(result.length());
    for (int i = 0; i < result.length(); i++) {
        String symbol = result.substr(i, 1);
        int opcode = 0; // Default to DONE
        if (mode == Mode::MODE_2D) {
            for (int j = 0; j < sizeof(opcode_glyphsp) / sizeof(opcode_glyphsp[0]); j++) {
                if(is_only_3d_opcode(symbol)) {
                    break;
                }
                if (symbol == opcode_glyphsp[j]) {
                    opcode = j;
                    break;
                }
            }
        } else if (mode == Mode::MODE_3D) {
            for (int j = 0; j < sizeof(opcode_glyphsp) / sizeof(opcode_glyphsp[0]); j++) {
                if (symbol == opcode_glyphsp[j]) {
                    opcode = j;
                    break;
                }
            }
        }
        if (is_structural_action(symbol)) {
            structural_action_count++;
        }
        byte_array[i] = opcode;
    }
    return byte_array;
}