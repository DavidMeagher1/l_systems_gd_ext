#include "l_systems.h"

using namespace l_systems;

LSystem::LSystem() {
}

LSystem::~LSystem() {
}

int LSystem::get_mode() {
    return int(vm.get_mode());
}

void LSystem::set_mode(int p_mode) {
    vm.set_mode(Mode(p_mode));
}


String LSystem::get_axiom() {
    return vm.get_axiom();
}

void LSystem::set_axiom(const String &p_axiom) {
    vm.set_axiom(p_axiom);
}

TypedDictionary<String,String> LSystem::get_rules() {
    return vm.get_rules();
}

void LSystem::set_rules(const TypedDictionary<String,String> &p_rules) {
    vm.set_rules(p_rules);
}

int LSystem::get_iterations() {
    return vm.get_iterations();
}

void LSystem::set_iterations(int p_iterations) {
    vm.set_iterations(p_iterations);
}

float LSystem::get_angle() {
    return vm.get_angle();
}

void LSystem::set_angle(float p_angle) {
    vm.set_angle(p_angle);
}

float LSystem::get_length() {
    return vm.get_length();
}

void LSystem::set_length(float p_length) {
    vm.set_length(p_length);
}

PackedByteArray LSystem::get_byte_code() {
    return vm.generate();
}

void LSystem::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_mode"), &LSystem::get_mode);
    ClassDB::bind_method(D_METHOD("set_mode"), &LSystem::set_mode);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "mode", PROPERTY_HINT_ENUM, "2D,3D"), "set_mode", "get_mode");
    ClassDB::bind_method(D_METHOD("get_axiom"), &LSystem::get_axiom);
    ClassDB::bind_method(D_METHOD("set_axiom"), &LSystem::set_axiom);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "axiom"), "set_axiom", "get_axiom");
    ClassDB::bind_method(D_METHOD("get_rules"), &LSystem::get_rules);
    ClassDB::bind_method(D_METHOD("set_rules"), &LSystem::set_rules);
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "rules", PROPERTY_HINT_DICTIONARY_TYPE, "String;String"), "set_rules", "get_rules");
    ClassDB::bind_method(D_METHOD("get_iterations"), &LSystem::get_iterations);
    ClassDB::bind_method(D_METHOD("set_iterations"), &LSystem::set_iterations);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "iterations"), "set_iterations", "get_iterations");
    ClassDB::bind_method(D_METHOD("get_angle"), &LSystem::get_angle);
    ClassDB::bind_method(D_METHOD("set_angle"), &LSystem::set_angle);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "angle"), "set_angle", "get_angle");
    ClassDB::bind_method(D_METHOD("get_length"), &LSystem::get_length);
    ClassDB::bind_method(D_METHOD("set_length"), &LSystem::set_length);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "length"), "set_length", "get_length");
    ClassDB::bind_method(D_METHOD("get_byte_code"), &LSystem::get_byte_code);
}
