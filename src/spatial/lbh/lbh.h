#pragma once

#include "vector.h"
#include "aabb.h"
#include "types.h"
#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <utility>
#include <vector>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/variant.hpp>

// Linearly Bounded Hierarchy (LBH) implementation for spatial partitioning

namespace l_systems::spatial {
    static godot::PackedByteArray to_gd_packed_byte_array(const std::vector<uint8_t> &data) {
        godot::PackedByteArray bytes;
        const int64_t size = static_cast<int64_t>(data.size());
        if (size == 0) {
            return bytes;
        }

        if (bytes.resize(size) != 0) {
            return godot::PackedByteArray();
        }

        std::memcpy(bytes.ptrw(), data.data(), static_cast<size_t>(size));
        return bytes;
    }

    template <std::size_t N>
    struct Node {
        AABB<float,N> bounds;
        vec<float, N> p1;
        vec<float, N> p2;
        unsigned int left_child = 0;
        unsigned int right_child = 0;
        std::vector<uint8_t> extra_data; // Placeholder for any additional data needed for the node
    };

    // an lbh is just a list of nodes, where the first node is the root
    template <std::size_t N>
    using LBH = std::vector<Node<N>>;

    static godot::Array lbh_2d_to_gd_array(const LBH<2>& lbh) {
        godot::Array gd_array;
        for (const auto& node : lbh) {
            godot::Dictionary dict;
            dict["bounds"] = AABB<float, 2>::to_gd_rect2(node.bounds);
            dict["p1"] = to_gd_vector2(node.p1);
            dict["p2"] = to_gd_vector2(node.p2);
            dict["left_child"] = node.left_child;
            dict["right_child"] = node.right_child;
            dict["extra_data"] = to_gd_packed_byte_array(node.extra_data);
            gd_array.append(dict);
        }
        return gd_array;
    }

    static godot::Array lbh_3d_to_gd_array(const LBH<3>& lbh) {
        godot::Array gd_array;
        for (const auto& node : lbh) {
            godot::Dictionary dict;
            dict["bounds"] = AABB<float, 3>::to_gd_aabb(node.bounds);
            dict["p1"] = to_gd_vector3(node.p1);
            dict["p2"] = to_gd_vector3(node.p2);
            dict["left_child"] = node.left_child;
            dict["right_child"] = node.right_child;
            dict["extra_data"] = to_gd_packed_byte_array(node.extra_data);
            gd_array.append(dict);
        }
        return gd_array;
    }

    template <std::size_t N>
    LBH<N> build(const std::vector<Node<N>>& leafs) {
        if (leafs.empty()) {
            return {};
        }
        if (leafs.size() % 2 == 1) {
            return build_odd(leafs);
        } else {
            return build_even(leafs);
        }
    }

    template <std::size_t N>
    LBH<N> build_odd(const std::vector<Node<N>>& leafs){
        assert(leafs.size() > 0);
        assert(leafs.size() % 2 == 1); // Ensure the number of leaf nodes is odd
        LBH<N> tree(2 * leafs.size());
        tree[0].bounds.min = vec<float, N>(std::numeric_limits<float>::max()); // Start with max bounds for the root
        tree[0].bounds.max = vec<float, N>(std::numeric_limits<float>::lowest()); // Start with min bounds for the root
        for (size_t i = 0; i < leafs.size(); i++) {
            tree[leafs.size() + i] = leafs[i]; // Place leaf nodes at the end of the tree, leaving one extra node for the root
        }
        int j = leafs.size() - 1; // Start from the last leaf node
        int i = static_cast<int>(tree.size()); // Start from the last node in the tree
        while (i > 0) {
            Node<N> internal_node;
            internal_node.left_child = i - 1;
            internal_node.right_child = (i == static_cast<int>(tree.size())) ? 0 : i; // If it's the first internal node, it only has one child
            // Compute the bounding box for the internal node based on its children
            internal_node.bounds.min = tree[internal_node.left_child].bounds.min.min(tree[internal_node.right_child].bounds.min);
            internal_node.bounds.max = tree[internal_node.left_child].bounds.max.max(tree[internal_node.right_child].bounds.max);
            tree[j] = internal_node; // Add the internal node to the tree
            j -= 1;
            i -= 2; // Move back two nodes for the next internal node
        }
        return tree;
    }

    template <std::size_t N>
    std::vector<Node<N>> build_even(const std::vector<Node<N>>& leafs) {
        assert(leafs.size() > 0);
        assert(leafs.size() % 2 == 0); // Ensure the number of leaf nodes is even
        LBH<N> tree(2 * leafs.size() - 1);
        tree[0].bounds.min = vec<float, N>(std::numeric_limits<float>::max()); // Start with max bounds for the root
        tree[0].bounds.max = vec<float, N>(std::numeric_limits<float>::lowest()); // Start with min bounds for the root
        for (size_t i = 0; i < leafs.size(); i++) {
            tree[leafs.size() - 1 + i] = leafs[i]; // Place leaf nodes at the end of the tree
        }
        int j = static_cast<int>(leafs.size() - 2); // Start from the last leaf node
        int i = static_cast<int>(tree.size() - 1); // Start from the last node in the tree
        while (i > 0) {
            Node<N> internal_node;
            internal_node.left_child = i - 1;
            internal_node.right_child = i;
            // Compute the bounding box for the internal node based on its children
            internal_node.bounds.min = tree[internal_node.left_child].bounds.min.min(tree[internal_node.right_child].bounds.min);
            internal_node.bounds.max = tree[internal_node.left_child].bounds.max.max(tree[internal_node.right_child].bounds.max);
            tree[j] = internal_node; // Add the internal node to the tree
            j -= 1;
            i -= 2; // Move back two nodes for the next internal node
        }
        return tree;
    };


    template <typename T, std::size_t N, std::size_t... Bits>
    static constexpr T expand_bits_impl(T x, std::index_sequence<Bits...>) {
        return (... | (((x >> Bits) & T(1)) << (Bits * N)));
    }

    template <typename T, std::size_t N>
    static constexpr T expand_bits(T x) {
        static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value,
            "expand_bits requires an unsigned integral type");
        return expand_bits_impl<T, N>(x, std::make_index_sequence<sizeof(T) * 8 / N>{});
    }

    template <typename T, std::size_t N, std::size_t... Dims>
    static constexpr T morton_code_impl(const vec<T, N>& point, std::index_sequence<Dims...>) {
        return (... | (expand_bits<T, N>(point.data[Dims]) << Dims));
    }

    template <typename T, std::size_t N>
    static constexpr T morton_code(const vec<T, N>& point) {
        return morton_code_impl(point, std::make_index_sequence<N>{});
    }

    template <typename T, std::size_t N>
    static T morton_code(const vec<float, N>& point) {
        static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value,
            "morton_code(float) requires an unsigned integral output type");
        constexpr uint32_t bits_per_axis = static_cast<uint32_t>((sizeof(T) * 8) / N);
        constexpr T max_coord = (bits_per_axis >= (sizeof(T) * 8)) ? ~T(0) : (T(1) << bits_per_axis) - T(1);

        vec<T, N> q;
        for (std::size_t i = 0; i < N; i++) {
            float c = point.data[i];
            if (c < 0.0f) c = 0.0f;
            if (c > 1.0f) c = 1.0f;
            q.data[i] = static_cast<T>(c * static_cast<float>(max_coord) + 0.5f);
        }

        return morton_code<T, N>(q);
    }

    

}