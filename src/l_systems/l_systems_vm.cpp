#include "l_systems_vm.h"

using namespace procgen::l_systems;
using namespace godot;

LSystemVM::LSystemVM() {
}

LSystemVM::~LSystemVM() {
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

TypedDictionary<String, String> LSystemVM::get_opcode_callbacks() {
    return opcode_callbacks;
}

void LSystemVM::set_opcode_callbacks(const TypedDictionary<String, String> &p_opcode_callbacks) {
    opcode_callbacks = p_opcode_callbacks;
}

Array LSystemVM::get_callback_names_by_id() {
    Array keys = opcode_callbacks.keys();
    keys.sort();
    return keys;
}

String LSystemVM::get_callback_name_by_id(int p_callback_id) {
    Array callback_symbols = get_callback_names_by_id();
    if (p_callback_id < 0 || p_callback_id >= callback_symbols.size()) {
        return String();
    }

    // callback table is symbol -> method_name
    const String callback_symbol = callback_symbols[p_callback_id];
    return opcode_callbacks.get(callback_symbol, String());
}

int LSystemVM::get_callback_index(const String &glyph) {
    // callback table is symbol -> method_name
    Array callback_symbols = get_callback_names_by_id();
    for (int i = 0; i < callback_symbols.size(); i++) {
        if (callback_symbols[i] == glyph) {
            return i;
        }
    }
    return -1; // Not found
}


PackedByteArray LSystemVM::generate() {
    String result = iterate();
    PackedByteArray byte_array;
    for (int i = 0; i < result.length(); i++) {
        String symbol = result.substr(i, 1);
        int opcode = 0; // Default to DONE
        for (int j = 0; j < sizeof(opcode_glyphsp) / sizeof(opcode_glyphsp[0]); j++) {
            if (symbol == opcode_glyphsp[j]) {
                opcode = j;
                break;
            }
        }
        int callback_index = get_callback_index(symbol);
        if (callback_index != -1) {
            byte_array.push_back(CALLBACK);
            byte_array.push_back(callback_index);
        } else {
            byte_array.push_back(opcode);
        }
    }
    return byte_array;
}