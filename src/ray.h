#pragma once
#include "vec3.h"

template<typename T = VEC3_TYPE>
class ray_t {
public:
    using vec_type   = vec3_t<T>;
    using point_type = vec3_t<T>;

    constexpr ray_t() = default;

    constexpr ray_t(const point_type& origin, const vec_type& direction)
        : orig(origin), dir(direction) {}

    constexpr const point_type& origin() const  { return orig; }
    constexpr const vec_type&   direction() const { return dir; }

    // Calcule un point le long du rayon pour le paramètre t
    constexpr point_type at(T t) const {
        return orig + t * dir;
    }

private:
    point_type orig;
    vec_type   dir;
};

// Alias usuels
using ray  = ray_t<VEC3_TYPE>;
using rayf = ray_t<float>;
using rayd = ray_t<double>;
