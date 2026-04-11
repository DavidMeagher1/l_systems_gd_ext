#include "l_systems.h"
#include "core/vector.h"
#include <limits>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace procgen;
using namespace procgen::l_systems;
using namespace godot;

static void execute_callback(const String &opcode, Node *caller, Dictionary *p_extra_data);

LSystem::LSystem() {
}

LSystem::~LSystem() {
}


String LSystem::get_axiom() {
    return vm.get_axiom();
}

void LSystem::set_axiom(const String &p_axiom) {
    vm.set_axiom(p_axiom);
    emit_changed();
}

TypedDictionary<String,String> LSystem::get_rules() {
    return vm.get_rules();
}

void LSystem::set_rules(const TypedDictionary<String,String> &p_rules) {
    vm.set_rules(p_rules);
    emit_changed();
}

int LSystem::get_iterations() {
    return vm.get_iterations();
}

void LSystem::set_iterations(int p_iterations) {
    vm.set_iterations(p_iterations);
    emit_changed();
}

float LSystem::get_angle() {
    return vm.get_angle();
}

void LSystem::set_angle(float p_angle) {
    vm.set_angle(p_angle);
    emit_changed();
}

float LSystem::get_length() {
    return vm.get_length();
}

void LSystem::set_length(float p_length) {
    vm.set_length(p_length);
    emit_changed();
}

PackedByteArray LSystem::get_byte_code() {
    return vm.generate();
}

template <size_t N>
LSystem::GenerationResult<N> LSystem::generate_leaf_nodes(Node *p_context_node) {
        struct State {
            vec<float, N> position;
            vec<float, N> direction;
            Dictionary extra_data;
        };

        PackedByteArray byte_code = vm.generate();
        lbh::LBH<N> nodes;
        vec<float, N> lr = vec<float, N>(0.0f); // last position
        vec<float, N> r = vec<float, N>(0.0f); // current position
        vec<float, N> min = vec<float, N>(std::numeric_limits<float>::max());
        vec<float, N> max = vec<float, N>(std::numeric_limits<float>::lowest());
        vec<float, N> direction = vec<float, N>(0.0f);
        direction[0] = 1.0f;
        Dictionary callback_extra_data = data.duplicate(); // Start with initial data, can be modified by callbacks and reset with CLEAR_EXTRA_DATA
        std::vector<State> state_stack;
        for (int i = 0; i < byte_code.size(); i++) {
            lbh::Node<N> node;
            Opcodes opcode = static_cast<Opcodes>(byte_code[i]);
            switch (opcode){
                case DONE:
                    break;
                case MOVE:
                    lr = r;
                    r += vm.get_length() * direction.normalized();
                    break;
                case FORWARD: {
                    lr = r;
                    r += vm.get_length() * direction.normalized();
                    node.p1 = lr;
                    node.p2 = r;
                    node.extra_data = callback_extra_data.duplicate();
                    node.bounds.min = lr.min(r);
                    node.bounds.max = lr.max(r);
                    max = max.max(node.bounds.max);
                    min = min.min(node.bounds.min);
                    nodes.push_back(node);
                    break;
                }
                case LEFT:
                    direction.rotate_in_plane(0, 1, -vm.get_angle());
                    break;
                case RIGHT:
                    direction.rotate_in_plane(0, 1, vm.get_angle());
                    break;
                case UP:
                    if constexpr (N >= 3) {
                        direction.rotate_in_plane(0, 2, -vm.get_angle());
                    }
                    break;
                case DOWN:
                    if constexpr (N >= 3) {
                        direction.rotate_in_plane(0, 2, vm.get_angle());
                    }
                    break;
                case PUSH:
                    state_stack.push_back({r, direction, callback_extra_data.duplicate()});
                    break;
                case POP:
                    if (!state_stack.empty()) {
                        State state = state_stack.back();
                        state_stack.pop_back();
                        r = state.position;
                        direction = state.direction;
                        callback_extra_data = state.extra_data;
                    }
                    break;
                case CALLBACK: {
                    if (i + 1 >= byte_code.size()) {
                        UtilityFunctions::printerr("Malformed callback bytecode: missing callback id.");
                        break;
                    }

                    const uint8_t callback_id = byte_code[++i];
                    const String callback_name = vm.get_callback_name_by_id(callback_id);
                    if (callback_name.is_empty()) {
                        UtilityFunctions::printerr("Callback id out of range: ", callback_id);
                        break;
                    }

                    execute_callback(callback_name, p_context_node, &callback_extra_data);
                    break;
                }
                case CLEAR_EXTRA_DATA:
                    callback_extra_data = data.duplicate(); // Reset to initial data
                    break;
                }
        }
        GenerationResult<N> result;
        result.nodes = std::move(nodes);
        result.bounds = AABB<float, N>();
        result.bounds.min = min;
        result.bounds.max = max;
        return result;
}

template <size_t N>
lbh::LBH<N> LSystem::generate(Node *p_context_node) {
    GenerationResult<N> result = generate_leaf_nodes<N>(p_context_node);
    if (result.nodes.empty()) {
        return {};
    }
    // get morton code for each node based on its centroid
    vec<float, N> size = result.bounds.max - result.bounds.min;
    // float uniform_denom = 0.0f;
    // vec<float, N> axis_offset = vec<float, N>(0.0f);
    // for (std::size_t d = 0; d < N; d++) {
    //     if (size[d] > uniform_denom) {
    //         uniform_denom = size[d];
    //     }
    // }
    // if (uniform_denom <= 0.0f) {
    //     uniform_denom = 1.0f;
    // }
    // for (std::size_t d = 0; d < N; d++) {
    //     axis_offset[d] = (uniform_denom - size[d]) * 0.5f / uniform_denom;
    // }

    std::vector<std::pair<uint32_t, lbh::Node<N>>> morton_nodes;
    morton_nodes.reserve(result.nodes.size());
    for (auto &node : result.nodes) {
        vec<float, N> centroid = (node.bounds.min + node.bounds.max) * 0.5f;
        vec<float, N> normalized_centroid;
        for (std::size_t d = 0; d < N; d++) {
            const float denom = size[d];
            normalized_centroid[d] = (denom > 0.0f) ? ((centroid[d] - result.bounds.min[d]) / denom) : 0.5f;
            node.bounds.min[d] = (denom > 0.0f) ? ((node.bounds.min[d] - result.bounds.min[d]) / denom) : 0.0f;
            node.bounds.max[d] = (denom > 0.0f) ? ((node.bounds.max[d] - result.bounds.min[d]) / denom) : 1.0f;
            node.p1[d] = (denom > 0.0f) ? ((node.p1[d] - result.bounds.min[d]) / denom) : 0.5f;
            node.p2[d] = (denom > 0.0f) ? ((node.p2[d] - result.bounds.min[d]) / denom) : 0.5f;
            // const float inv = 1.0f / uniform_denom;
            // const float off = axis_offset[d];
            // normalized_centroid[d] = (centroid[d] - result.bounds.min[d]) * inv + off;
            // node.bounds.min[d] = (node.bounds.min[d] - result.bounds.min[d]) * inv + off;
            // node.bounds.max[d] = (node.bounds.max[d] - result.bounds.min[d]) * inv + off;
            // node.p1[d] = (node.p1[d] - result.bounds.min[d]) * inv + off;
            // node.p2[d] = (node.p2[d] - result.bounds.min[d]) * inv + off;
        }
        uint32_t morton_code = lbh::morton_code<uint32_t>(normalized_centroid);
        morton_nodes.push_back({morton_code, node});
    }
    // sort leaf nodes by morton code
    std::sort(morton_nodes.begin(), morton_nodes.end(), [](const std::pair<uint32_t, lbh::Node<N>>& a, const std::pair<uint32_t, lbh::Node<N>>& b) {
        return a.first < b.first;
    });

    for (size_t i = 0; i < result.nodes.size(); i++) {
        result.nodes[i] = std::move(morton_nodes[i].second);

    }
    return lbh::build(result.nodes);
}

Array LSystem::generate_2d(Node *p_context_node) {
    lbh::LBH<2> lbh = generate<2>(p_context_node);
    return lbh::lbh_2d_to_gd_array(lbh);
}

Array LSystem::generate_3d(Node *p_context_node) {
    lbh::LBH<3> lbh = generate<3>(p_context_node);
    return lbh::lbh_3d_to_gd_array(lbh);
}

void LSystem::set_opcode_callbacks(const TypedDictionary<String, String> &p_opcode_callbacks) {
    vm.set_opcode_callbacks(p_opcode_callbacks);
}

TypedDictionary<String, String> LSystem::get_opcode_callbacks() {
    return vm.get_opcode_callbacks();
}

Dictionary LSystem::get_data() {
    return data;
}

void LSystem::set_data(const Dictionary &p_data) {
    data = p_data;
    emit_changed();
}

static void execute_callback(const String &opcode, Node *caller, Dictionary *p_extra_data) {
    if (caller == nullptr || p_extra_data == nullptr) {
        return;
    }

    Callable callback(caller, StringName(opcode));
    if (!callback.is_valid()) {
        UtilityFunctions::printerr("Invalid callback method: ", opcode);
        return;
    }

    callback.call(*p_extra_data);
}

void LSystem::_bind_methods() {
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
    ClassDB::bind_method(D_METHOD("generate_2d", "context_node"), &LSystem::generate_2d);
    ClassDB::bind_method(D_METHOD("generate_3d", "context_node"), &LSystem::generate_3d);

    ClassDB::bind_method(D_METHOD("get_opcode_callbacks"), &LSystem::get_opcode_callbacks);
    ClassDB::bind_method(D_METHOD("set_opcode_callbacks"), &LSystem::set_opcode_callbacks);
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "opcode_callbacks", PROPERTY_HINT_DICTIONARY_TYPE, "String;String"), "set_opcode_callbacks", "get_opcode_callbacks");

    ClassDB::bind_method(D_METHOD("get_data"), &LSystem::get_data);
    ClassDB::bind_method(D_METHOD("set_data", "data"), &LSystem::set_data);
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "data"), "set_data", "get_data");

}
