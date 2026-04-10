#include "l_systems.h"
#include "vector.h"
using namespace l_systems;

LSystem::LSystem() {
}

LSystem::~LSystem() {
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

template <size_t N>
LSystem::GenerationResult<N> LSystem::generate_leaf_nodes() {
        struct State {
            vec<float, N> position;
            vec<float, N> direction;
        };

        PackedByteArray byte_code = vm.generate();
        spatial::LBH<N> nodes;
        vec<float, N> lr; // last position
        vec<float, N> r; // current position
        vec<float, N> min;
        vec<float, N> max;
        vec<float, N> direction;
        direction[0] = 1.0f;
        std::vector<State> state_stack;
        for (int i = 0; i < byte_code.size(); i++) {
            spatial::Node<N> node;
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
                    state_stack.push_back({r, direction});
                    break;
                case POP:
                    if (!state_stack.empty()) {
                        State state = state_stack.back();
                        state_stack.pop_back();
                        r = state.position;
                        direction = state.direction;
                    }
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
spatial::LBH<N> LSystem::generate() {
    GenerationResult<N> result = generate_leaf_nodes<N>();
    // get morton code for each node based on its centroid
    vec<float, N> size = result.bounds.max - result.bounds.min;
    for (auto &node : result.nodes) {
        vec<float, N> centroid = (node.bounds.min + node.bounds.max) * 0.5f;
        vec<float, N> normalized_centroid;
        for (std::size_t d = 0; d < N; d++) {
            const float denom = size[d];
            normalized_centroid[d] = (denom > 0.0f) ? ((centroid[d] - result.bounds.min[d]) / denom) : 0.5f;
        }
        uint32_t morton_code = spatial::morton_code<uint32_t>(normalized_centroid);
        node.morton_code = morton_code;
    }
    // sort leaf nodes by morton code
    std::sort(result.nodes.begin(), result.nodes.end(), [](const spatial::Node<N>& a, const spatial::Node<N>& b) {
        return a.morton_code < b.morton_code;
    });
    return spatial::build(result.nodes);
}

godot::Array LSystem::generate_2d() {
    spatial::LBH<2> lbh = generate<2>();
    return spatial::lbh_2d_to_gd_array(lbh);
}

godot::Array LSystem::generate_3d() {
    spatial::LBH<3> lbh = generate<3>();
    return spatial::lbh_3d_to_gd_array(lbh);
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
    ClassDB::bind_method(D_METHOD("generate_2d"), &LSystem::generate_2d);
    ClassDB::bind_method(D_METHOD("generate_3d"), &LSystem::generate_3d);
}
