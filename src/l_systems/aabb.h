
#pragma once
#include "vector.h"
#include <limits>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/aabb.hpp>
namespace l_systems {
    template <typename T, std::size_t N>
    struct AABB {
        static_assert(std::is_arithmetic_v<T>, "AABB scalar type must be arithmetic");
        vec<T, N> min{};
        vec<T, N> max{};

        AABB() : min(std::numeric_limits<T>::max()), max(std::numeric_limits<T>::lowest()) {}

        void expand(const vec<T, N>& point) {
            min = min.min(point);
            max = max.max(point);
        }

        void expand(const AABB& other) {
            min = min.min(other.min);
            max = max.max(other.max);
        }

        static godot::AABB to_gd_aabb(const AABB<float, 3>& aabb) {
            return godot::AABB(to_gd_vector3(aabb.min), to_gd_vector3(aabb.max - aabb.min));
        }

        static godot::Rect2 to_gd_rect2(const AABB<float, 2>& aabb) {
            return godot::Rect2(to_gd_vector2(aabb.min), to_gd_vector2(aabb.max - aabb.min));
        }
    };
}