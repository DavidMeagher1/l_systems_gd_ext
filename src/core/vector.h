#pragma once
#include <cstddef>
#include <type_traits>
#include <cmath>
#include <utility>
#include <algorithm>
#include <cassert>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace procgen {
    template <typename T, const std::size_t N>
    struct vec {
        static_assert(N > 0, "Size of vector must be greater than 0");
        static_assert(std::is_arithmetic_v<T>, "Vector scalar type must be arithmetic");
        T data[N]{};

        constexpr vec() = default;

        template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) == N && (std::is_convertible_v<Args, T> && ...))>>
        constexpr vec(Args&&... args) : data{std::forward<Args>(args)...} {}

        constexpr explicit vec(const T& value) {
            for (std::size_t i = 0; i < N; i++) {
                data[i] = value;
            }
        }

        constexpr T& operator[](std::size_t index) {return data[index];}

        constexpr const T& operator[](std::size_t index) const {return data[index];}

        constexpr vec& operator+=(const vec& rhs) {
            for (std::size_t i = 0; i < N; i++) {
                data[i] += rhs.data[i];
            }
            return *this;
        }

        constexpr vec& operator-=(const vec& rhs) {
            for (std::size_t i = 0; i < N; i++) {
                data[i] -= rhs.data[i];
            }
            return *this;
        }

        constexpr vec& operator*=(const vec& rhs) {
            for (std::size_t i = 0; i < N; i++) {
                data[i] *= rhs.data[i];
            }
            return *this;
        }

        constexpr vec& operator*=(T scalar) {
            for (std::size_t i = 0; i < N; i++) {
                data[i] *= scalar;
            }
            return *this;
        }

        constexpr vec& operator/=(const vec& rhs) {
            for (std::size_t i = 0; i < N; i++) {
                data[i] /= rhs.data[i];
            }
            return *this;
        }

        constexpr vec& operator/=(T scalar) {
            for (std::size_t i = 0; i < N; i++) {
                data[i] /= scalar;
            }
            return *this;
        }

        friend constexpr vec operator+(vec lhs, const vec& rhs) {return lhs += rhs;}
        friend constexpr vec operator-(vec lhs, const vec& rhs) {return lhs -= rhs;}
        friend constexpr vec operator*(vec lhs, const vec& rhs) {return lhs *= rhs;}
        friend constexpr vec operator*(T scalar, vec v) {return v *= scalar;}
        friend constexpr vec operator*(vec v, T scalar) {return v *= scalar;}
        friend constexpr vec operator/(vec lhs, const vec& rhs) {return lhs /= rhs;}
        friend constexpr vec operator/(vec v, T scalar) {return v /= scalar;}

        friend constexpr bool operator==(const vec& lhs, const vec& rhs) {
            for (std::size_t i = 0; i < N; i++) {
                if (lhs.data[i] != rhs.data[i]) return false;
            }
            return true;
        }

        friend constexpr bool operator!=(const vec& lhs, const vec& rhs) {
            return !(lhs == rhs);
        }

        friend constexpr bool operator<(const vec& lhs, const vec& rhs) {
            for (std::size_t i = 0; i < N; i++) {
                if (lhs.data[i] >= rhs.data[i]) return false;
            }
            return true;
        }

        friend constexpr bool operator>(const vec& lhs, const vec& rhs) {
            for (std::size_t i = 0; i < N; i++) {
                if (lhs.data[i] <= rhs.data[i]) return false;
            }
            return true;
        }

        friend constexpr bool operator<=(const vec& lhs, const vec& rhs) {
            for (std::size_t i = 0; i < N; i++) {
                if (lhs.data[i] > rhs.data[i]) return false;
            }
            return true;
        }

        friend constexpr bool operator>=(const vec& lhs, const vec& rhs) {
            for (std::size_t i = 0; i < N; i++) {
                if (lhs.data[i] < rhs.data[i]) return false;
            }
            return true;
        }

        constexpr T dot(const vec& other) const {
            T result{};
            for (std::size_t i = 0; i < N; i++) {
                result += data[i] * other.data[i];
            }
            return result;
        }

        T length() const {
            static_assert(std::is_floating_point_v<T>, "length() requires floating-point vector type");
            return std::sqrt(this->dot(*this));
        }

        constexpr vec normalized() const {
            static_assert(std::is_floating_point_v<T>, "normalized() requires floating-point vector type");
            T len = length();
            if (len == 0) return *this;
            return *this / len;
        }

        constexpr vec abs() const {
            vec result;
            for (std::size_t i = 0; i < N; i++) {
                result.data[i] = std::abs(data[i]);
            }
            return result;
        }

        constexpr vec sign() const {
            vec result;
            for (std::size_t i = 0; i < N; i++) {
                result.data[i] = (data[i] > 0) - (data[i] < 0);
            }
            return result;
        }

        constexpr vec floor() const {
            static_assert(std::is_floating_point_v<T>, "floor() requires floating-point vector type");
            vec result;
            for (std::size_t i = 0; i < N; i++) {
                result.data[i] = std::floor(data[i]);
            }
            return result;
        }

        constexpr vec fract() const {
            static_assert(std::is_floating_point_v<T>, "fract() requires floating-point vector type");
            vec result;
            for (std::size_t i = 0; i < N; i++) {
                result.data[i] = data[i] - std::trunc(data[i]);
            }
            return result;
        }

        constexpr vec round() const {
            static_assert(std::is_floating_point_v<T>, "round() requires floating-point vector type");
            vec result;
            for (std::size_t i = 0; i < N; i++) {
                result.data[i] = std::round(data[i]);
            }
            return result;
        }

        constexpr vec ceil() const {
            static_assert(std::is_floating_point_v<T>, "ceil() requires floating-point vector type");
            vec result;
            for (std::size_t i = 0; i < N; i++) {
                result.data[i] = std::ceil(data[i]);
            }
            return result;
        }

        constexpr vec clamp(const vec& min, const vec& max) const {
            vec result;
            for (std::size_t i = 0; i < N; i++) {
                result.data[i] = std::max(min.data[i], std::min(max.data[i], data[i]));
            }
            return result;
        }

        constexpr vec mix(const vec& other, T t) const {
            static_assert(std::is_floating_point_v<T>, "mix() requires floating-point vector type");
            vec result;
            for (std::size_t i = 0; i < N; i++) {
                result.data[i] = data[i] * (1 - t) + other.data[i] * t;
            }
            return result;
        }

        constexpr vec min(const vec& other) const {
            vec result;
            for (std::size_t i = 0; i < N; i++) {
                result.data[i] = std::min(data[i], other.data[i]);
            }
            return result;
        }

        constexpr vec max(const vec& other) const {
            vec result;
            for (std::size_t i = 0; i < N; i++) {
                result.data[i] = std::max(data[i], other.data[i]);
            }
            return result;
        }

        vec& rotate_in_plane(std::size_t axis1, std::size_t axis2, T theta) {
            static_assert(N >= 2, "rotate_in_plane() requires N >= 2");
            static_assert(std::is_floating_point_v<T>, "rotate_in_plane() requires floating-point vector type");
            assert(axis1 < N && axis2 < N && axis1 != axis2);
            const T c = static_cast<T>(std::cos(theta));
            const T s = static_cast<T>(std::sin(theta));
            const T a = data[axis1];
            const T b = data[axis2];
            data[axis1] = a * c - b * s;
            data[axis2] = a * s + b * c;
            return *this;
        }

    };

    template <typename T>
    struct vec2 : public vec<T, 2> {
        using vec<T, 2>::vec; // Inherit constructors

        constexpr T cross(const vec2& other) const {
            return this->data[0] * other.data[1] - this->data[1] * other.data[0];
        }
    };

    template <typename T>
    struct vec3 : public vec<T, 3> {
        using vec<T, 3>::vec; // Inherit constructors

        constexpr vec3 cross(const vec3& other) const {
            return vec3{
                this->data[1] * other.data[2] - this->data[2] * other.data[1],
                this->data[2] * other.data[0] - this->data[0] * other.data[2],
                this->data[0] * other.data[1] - this->data[1] * other.data[0]
            };
        }
    };

    template <typename T>
    struct vec4 : public vec<T, 4> {
        using vec<T, 4>::vec; // Inherit constructors
    };

    static godot::Vector2 to_gd_vector2(const vec<float, 2>& v) {
        return godot::Vector2(v.data[0], v.data[1]);
    }

    static godot::Vector3 to_gd_vector3(const vec<float, 3>& v) {
        return godot::Vector3(v.data[0], v.data[1], v.data[2]);
    }
}