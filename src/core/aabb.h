
#pragma once
#include "vector.h"
#include <limits>
#include <type_traits>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/aabb.hpp>
namespace procgen{
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

        void grow(T amount) {
            grow(vec<T, N>(amount));
        }

        void grow(const vec<T, N> &amount) {
            for (std::size_t i = 0; i < N; i++) {
                if (amount[i] >= T{}) {
                    min[i] -= amount[i];
                    max[i] += amount[i];
                    continue;
                }

                const T shrink_amount = -amount[i];
                const T half_extent = (max[i] - min[i]) / static_cast<T>(2);
                const T clamped = shrink_amount > half_extent ? half_extent : shrink_amount;
                min[i] += clamped;
                max[i] -= clamped;
            }
        }

        AABB grown(T amount) const {
            AABB result = *this;
            result.grow(amount);
            return result;
        }

        AABB grown(const vec<T, N> &amount) const {
            AABB result = *this;
            result.grow(amount);
            return result;
        }

        void shrink(T amount) {
            shrink(vec<T, N>(amount));
        }

        void shrink(const vec<T, N> &amount) {
            for (std::size_t i = 0; i < N; i++) {
                if (amount[i] < T{}) {
                    min[i] += amount[i];
                    max[i] -= amount[i];
                    continue;
                }

                const T half_extent = (max[i] - min[i]) / static_cast<T>(2);
                const T clamped = amount[i] > half_extent ? half_extent : amount[i];
                min[i] += clamped;
                max[i] -= clamped;
            }
        }

        AABB shrunk(T amount) const {
            AABB result = *this;
            result.shrink(amount);
            return result;
        }

        AABB shrunk(const vec<T, N> &amount) const {
            AABB result = *this;
            result.shrink(amount);
            return result;
        }

        static godot::AABB to_gd_aabb(const AABB<float, 3>& aabb) {
            return godot::AABB(to_gd_vector3(aabb.min), to_gd_vector3(aabb.max - aabb.min));
        }

        static godot::Rect2 to_gd_rect2(const AABB<float, 2>& aabb) {
            return godot::Rect2(to_gd_vector2(aabb.min), to_gd_vector2(aabb.max - aabb.min));
        }
    };
}