#ifndef H_COMMON
#define H_COMMON

const uint UINT32_MAX = 0xFFFFFFFFu;
const float FLOAT_MAX = 3.402823466e38;
const float FLOAT_MIN = 1.175494351e-38;
const float PI = 3.1415926535897932384626433832795;

#ifndef NDEBUG
#extension GL_EXT_debug_printf: enable
#define ASSERT(cond, str) if (!(cond)) debugPrintfEXT(str)
#else
#define ASSERT(cond, str)
#endif

// These must match the IDs in gi.h
#define AOV_ID_COLOR 0
#define AOV_ID_NORMAL 1
#define AOV_ID_DEBUG_NEE 2
#define AOV_ID_DEBUG_BARYCENTRICS 3
#define AOV_ID_DEBUG_TEXCOORDS 4
#define AOV_ID_DEBUG_BOUNCES 5
#define AOV_ID_DEBUG_CLOCK_CYCLES 6
#define AOV_ID_DEBUG_OPACITY 7
#define AOV_ID_DEBUG_TANGENTS 8
#define AOV_ID_DEBUG_BITANGENTS 9

vec4 uvec4AsVec4(uvec4 v)
{
    v >>= 9;
    v |= 0x3f800000;
    return vec4(uintBitsToFloat(v)) - vec4(1.0);
}

float uintAsFloat(uint v)
{
    return uintBitsToFloat(0x3f800000 | (v >> 9)) - 1.0;
}

// Enable for higher quality random numbers at the cost of performance
#define RAND_4D

#ifdef RAND_4D
// RNG producing on a four-element vector.
// From: "Hash Functions for GPU Rendering" by Jarzynski and Olano.
// Licensed under CC BY-ND 3.0: https://creativecommons.org/licenses/by-nd/3.0/
uvec4 hash_pcg4d(uvec4 v)
{
    v = v * 1664525u + 1013904223u;
    v.x += v.y * v.w; v.y += v.z * v.x; v.z += v.x * v.y; v.w += v.y * v.z;
    v ^= v >> 16u;
    v.x += v.y * v.w; v.y += v.z * v.x; v.z += v.x * v.y; v.w += v.y * v.z;
    return v;
}

vec4 rng4d_next(inout uvec4 rng_state)
{
    rng_state.w++;
    return uvec4AsVec4(hash_pcg4d(rng_state));
}

uvec4 rng4d_init(uvec2 pixel_coords, uint frame_num)
{
    return uvec4(pixel_coords.xy, frame_num, 0);
}
#else
// Hash prospector parametrization found by GH user TheIronBorn:
// https://github.com/skeeto/hash-prospector#discovered-hash-functions
uint hash_theironborn(uint x)
{
    x ^= x >> 16u;
    x *= 0x21f0aaadu;
    x ^= x >> 15u;
    x *= 0xd35a2d97u;
    x ^= x >> 15u;
    return x;
}

// https://www.shadertoy.com/view/XlGcRh
uint hash_pcg32(inout uint state)
{
    state = state * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float rng_next(inout uint rng_state)
{
    rng_state = hash_pcg32(rng_state);
    return uintAsFloat(rng_state);
}

uint rng_init(uint pixel_index, uint sampleIndex)
{
    return hash_theironborn(pixel_index * (sampleIndex + 1));
}
#endif

// Duff et al. 2017. Building an Orthonormal Basis, Revisited. JCGT.
// Licensed under CC BY-ND 3.0: https://creativecommons.org/licenses/by-nd/3.0/
void orthonormal_basis(in vec3 n, out vec3 b1, out vec3 b2)
{
    float nsign = (n.z >= 0.0 ? 1.0 : -1.0); // sign() intrinsic returns 0.0 for 0.0 :(
    float a = -1.0 / (nsign + n.z);
    float b = n.x * n.y * a;

    b1 = vec3(1.0 + nsign * n.x * n.x * a, nsign * b, -nsign * n.x);
    b2 = vec3(b, nsign + n.y * n.y * a, -n.y);
}

#if 1
// From: "A Fast and Robust Method for Avoiding Self-Intersection"
// Wächter and Binder, Ch. 6 in Ray Tracing Gems I.
// Licensed under CC BY-NC-ND 4.0: https://creativecommons.org/licenses/by-nc-nd/4.0/
vec3 offset_ray_origin(vec3 p, vec3 geom_normal)
{
    const float origin = 1.0 / 32.0;
    const float float_scale = 1.0 / 65536.0;
    const float int_scale = 256.0;

    ivec3 of_i = ivec3(
        int_scale * geom_normal.x,
        int_scale * geom_normal.y,
        int_scale * geom_normal.z
    );

    vec3 p_i = vec3(
        intBitsToFloat(floatBitsToInt(p.x) + ((p.x < 0.0) ? -of_i.x : of_i.x)),
        intBitsToFloat(floatBitsToInt(p.y) + ((p.y < 0.0) ? -of_i.y : of_i.y)),
        intBitsToFloat(floatBitsToInt(p.z) + ((p.z < 0.0) ? -of_i.z : of_i.z))
    );

    return vec3(
        abs(p.x) < origin ? (p.x + float_scale * geom_normal.x) : p_i.x,
        abs(p.y) < origin ? (p.y + float_scale * geom_normal.y) : p_i.y,
        abs(p.z) < origin ? (p.z + float_scale * geom_normal.z) : p_i.z
    );
}
#else
vec3 offset_ray_origin(vec3 p, vec3 geom_normal)
{
    const float EPS = 0.0001;
    return p + geom_normal * EPS;
}
#endif

// Octahedral normal encoding
// https://knarkowicz.wordpress.com/2014/04/16/octahedron-normal-vector-encoding/
vec2 signNonZero(vec2 v)
{
    return vec2((v.x >= 0.0) ? 1.0 : -1.0, (v.y >= 0.0) ? 1.0 : -1.0);
}

vec2 encode_octahedral(vec3 v)
{
    v /= (abs(v.x) + abs(v.y) + abs(v.z));
    vec2 e = (v.z < 0.0) ? ((1.0 - abs(v.yx)) * signNonZero(v.xy)) : v.xy;
    return e * 0.5 + 0.5;
}

vec3 decode_octahedral(vec2 e)
{
    e = e * 2.0 - 1.0;

    vec3 v = vec3(e.xy, 1.0 - abs(e.x) - abs(e.y));
    // Optimization by Rune Stubbe: https://twitter.com/Stubbesaurus/status/937994790553227264
    float t = max(-v.z, 0.0);
    v.x += v.x >= 0.0 ? -t : t;
    v.y += v.y >= 0.0 ? -t : t;
    return normalize(v);
}

uint encode_direction(vec3 d)
{
    vec2 o = encode_octahedral(d);
    return packUnorm2x16(o);
}

vec3 decode_direction(uint e)
{
    vec2 o = unpackUnorm2x16(e);
    return decode_octahedral(o);
}

// RT Gems, Shirley. Chapter 16 Sampling Transformations Zoo.
vec3 sample_hemisphere(vec2 xi)
{
    float a = sqrt(xi.x);
    float b = PI * 2.0 * xi.y;

    return vec3(
        a * cos(b),
        a * sin(b),
        sqrt(1.0 - xi.x)
    );
}

#endif
