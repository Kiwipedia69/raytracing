#pragma once
#include "vec3.h"
#include <iostream>
#include <type_traits>
#include <cstdint>

// --- Choix du type de color : par défaut float (gain d'espace) ---
#ifndef COLOR_TYPE
#define COLOR_TYPE float
#endif

using color   = vec3_t<COLOR_TYPE>; // priorité à float
using colorf  = vec3_t<float>;
using colord  = vec3_t<double>;

// Clamp utilitaire
template<typename T>
static inline T clamp01(T x) {
    return x < T(0) ? T(0) : (x > T(1) ? T(1) : x);
}

// Convertit [0..1] -> [0..255]
template<typename T>
static inline uint8_t to_byte(T x) {
    return static_cast<uint8_t>(255.999 * clamp01(x));
}

// Écrit en PPM-like (texte). Compatible float/double.
template<typename T>
inline void write_color(std::ostream& out, const vec3_t<T>& c_linear, T gamma = T(1)) {
    // gamma==1 -> pas de correction, sinon applique 1/gamma
    vec3_t<T> c = c_linear;
    if (gamma != T(1)) {
        const T inv = T(1) / gamma;
        c.e[0] = std::pow(clamp01(c.e[0]), inv);
        c.e[1] = std::pow(clamp01(c.e[1]), inv);
        c.e[2] = std::pow(clamp01(c.e[2]), inv);
    }

    const int r = to_byte(c.e[0]);
    const int g = to_byte(c.e[1]);
    const int b = to_byte(c.e[2]);
    out << r << ' ' << g << ' ' << b << '\n';
}

// Écrit dans un buffer RGB8 (binaire), pratique pour PNG/JPG.
// dst doit pointer sur 3 octets (R,G,B).
template<typename T>
inline void write_color(uint8_t* dst, const vec3_t<T>& c_linear, T gamma = T(1)) {
    vec3_t<T> c = c_linear;
    if (gamma != T(1)) {
        const T inv = T(1) / gamma;
        c.e[0] = std::pow(clamp01(c.e[0]), inv);
        c.e[1] = std::pow(clamp01(c.e[1]), inv);
        c.e[2] = std::pow(clamp01(c.e[2]), inv);
    }
    dst[0] = to_byte(c.e[0]);
    dst[1] = to_byte(c.e[1]);
    dst[2] = to_byte(c.e[2]);
}
