#pragma once
#include <cmath>
#include <iostream>
#include <type_traits>
#include <limits>

// -------------------------------------------------------------------------------
// Choix du type par défaut (double). Possible compilation avec -DVEC3_TYPE=float.
// -------------------------------------------------------------------------------
#ifndef VEC3_TYPE
#define VEC3_TYPE double
#endif

template<typename T = VEC3_TYPE>
class vec3_t {
    static_assert(std::is_floating_point_v<T>, "vec3_t must use float or double");
public:
    T e[3];

    // --- Ctors de base ---
    constexpr vec3_t() : e{0,0,0} {}
    constexpr vec3_t(T e0, T e1, T e2) : e{e0, e1, e2} {}

    // --- Conversion entre vec3_t<U> et vec3_t<T> ---
    // Implicite si conversion 'safe' (élargissement de précision)
    template<typename U,
             std::enable_if_t<
                std::is_floating_point_v<U> &&
                (std::numeric_limits<T>::digits >= std::numeric_limits<U>::digits), int> = 0>
    constexpr vec3_t(const vec3_t<U>& o) : e{ static_cast<T>(o.e[0]),
                                              static_cast<T>(o.e[1]),
                                              static_cast<T>(o.e[2]) } {}

    // Explicite si conversion potentiellement perte de précision (rétrécissement)
    template<typename U,
             std::enable_if_t<
                std::is_floating_point_v<U> &&
                (std::numeric_limits<T>::digits < std::numeric_limits<U>::digits), int> = 0>
    explicit constexpr vec3_t(const vec3_t<U>& o) : e{ static_cast<T>(o.e[0]),
                                                       static_cast<T>(o.e[1]),
                                                       static_cast<T>(o.e[2]) } {}

    // --- Accès coordonnées ---
    constexpr T x() const { return e[0]; }
    constexpr T y() const { return e[1]; }
    constexpr T z() const { return e[2]; }

    // --- Opérateurs élémentaires ---
    constexpr vec3_t operator-() const { return vec3_t(-e[0], -e[1], -e[2]); }
    constexpr T operator[](int i) const { return e[i]; }
    constexpr T& operator[](int i) { return e[i]; }

    constexpr vec3_t& operator+=(const vec3_t& v) { e[0]+=v.e[0]; e[1]+=v.e[1]; e[2]+=v.e[2]; return *this; }
    constexpr vec3_t& operator*=(T t) { e[0]*=t; e[1]*=t; e[2]*=t; return *this; }
    constexpr vec3_t& operator/=(T t) { return *this *= T(1)/t; }

    // Assignation depuis un autre vec3_t<U> (mêmes règles que les ctors)
    template<typename U,
             std::enable_if_t<
                std::is_floating_point_v<U> &&
                (std::numeric_limits<T>::digits >= std::numeric_limits<U>::digits), int> = 0>
    constexpr vec3_t& operator=(const vec3_t<U>& o) {
        e[0]=static_cast<T>(o.e[0]); e[1]=static_cast<T>(o.e[1]); e[2]=static_cast<T>(o.e[2]); return *this;
    }
    template<typename U,
             std::enable_if_t<
                std::is_floating_point_v<U> &&
                (std::numeric_limits<T>::digits < std::numeric_limits<U>::digits), int> = 0>
    constexpr vec3_t& operator=(const vec3_t<U>& o) {
        e[0]=static_cast<T>(o.e[0]); e[1]=static_cast<T>(o.e[1]); e[2]=static_cast<T>(o.e[2]); return *this;
    }

    // --- Normes ---
    T length() const { return std::sqrt(length_squared()); }
    constexpr T length_squared() const { return e[0]*e[0] + e[1]*e[1] + e[2]*e[2]; }
};

// Alias usuels
using vec3   = vec3_t<VEC3_TYPE>;
using point3 = vec3_t<VEC3_TYPE>;

// -----------------------------------------------------------------------------
// Opérations mixtes : déduisent le type commun (float ⨁ double → double)
// -----------------------------------------------------------------------------
template<typename T>
inline std::ostream& operator<<(std::ostream& out, const vec3_t<T>& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

template<typename T, typename U>
using vec_ct = std::common_type_t<T,U>;

template<typename T, typename U>
inline constexpr vec3_t<vec_ct<T,U>> operator+(const vec3_t<T>& a, const vec3_t<U>& b) {
    using R = vec_ct<T,U>;
    return vec3_t<R>(R(a.e[0])+R(b.e[0]), R(a.e[1])+R(b.e[1]), R(a.e[2])+R(b.e[2]));
}

template<typename T, typename U>
inline constexpr vec3_t<vec_ct<T,U>> operator-(const vec3_t<T>& a, const vec3_t<U>& b) {
    using R = vec_ct<T,U>;
    return vec3_t<R>(R(a.e[0])-R(b.e[0]), R(a.e[1])-R(b.e[1]), R(a.e[2])-R(b.e[2]));
}

template<typename T, typename U>
inline constexpr vec3_t<vec_ct<T,U>> operator*(const vec3_t<T>& a, const vec3_t<U>& b) {
    using R = vec_ct<T,U>;
    return vec3_t<R>(R(a.e[0])*R(b.e[0]), R(a.e[1])*R(b.e[1]), R(a.e[2])*R(b.e[2]));
}

template<typename T, typename S,
         std::enable_if_t<std::is_arithmetic_v<S>, int> = 0>
inline constexpr vec3_t<vec_ct<T,S>> operator*(const vec3_t<T>& v, S s) {
    using R = vec_ct<T,S>;
    return vec3_t<R>(R(v.e[0])*R(s), R(v.e[1])*R(s), R(v.e[2])*R(s));
}

template<typename T, typename S,
         std::enable_if_t<std::is_arithmetic_v<S>, int> = 0>
inline constexpr vec3_t<vec_ct<T,S>> operator*(S s, const vec3_t<T>& v) {
    return v * s;
}

template<typename T, typename S,
         std::enable_if_t<std::is_arithmetic_v<S>, int> = 0>
inline constexpr vec3_t<vec_ct<T,S>> operator/(const vec3_t<T>& v, S s) {
    using R = vec_ct<T,S>;
    return vec3_t<R>(R(v.e[0]) / R(s), R(v.e[1]) / R(s), R(v.e[2]) / R(s));
}

template<typename T, typename U>
inline constexpr vec_ct<T,U> dot(const vec3_t<T>& a, const vec3_t<U>& b) {
    using R = vec_ct<T,U>;
    return R(a.e[0])*R(b.e[0]) + R(a.e[1])*R(b.e[1]) + R(a.e[2])*R(b.e[2]);
}

template<typename T, typename U>
inline constexpr vec3_t<vec_ct<T,U>> cross(const vec3_t<T>& a, const vec3_t<U>& b) {
    using R = vec_ct<T,U>;
    return vec3_t<R>(R(a.e[1])*R(b.e[2]) - R(a.e[2])*R(b.e[1]),
                     R(a.e[2])*R(b.e[0]) - R(a.e[0])*R(b.e[2]),
                     R(a.e[0])*R(b.e[1]) - R(a.e[1])*R(b.e[0]));
}

template<typename T>
inline vec3_t<T> unit_vector(const vec3_t<T>& v) {
    return v / v.length();
}
