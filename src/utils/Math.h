//
// Created by Ian on 2-1-2024.
//

#ifndef ADVANCEDGRAPHICS_MATH_H
#define ADVANCEDGRAPHICS_MATH_H

#include <cstdlib>

struct alignas(8) vec2i {
    vec2i() : x(0), y(0) {};
    vec2i(const int v) : x( v ), y( v ) {};
    vec2i(const int x, const int y) : x( x ), y( y ) {};

    union {
        struct {
            int x, y;
        };

        int xy[2] {};
    };

    int& operator [] ( const int idx ) { return xy[idx]; }

    vec2i operator+ (const int v) const {
        vec2i newVec {};

        newVec.x = this->x + v;
        newVec.y = this->y + v;

        return newVec;
    };

    vec2i operator+ (const vec2i v) const {
        vec2i newVec {};

        newVec.x = this->x + v.x;
        newVec.y = this->y + v.y;

        return newVec;
    };

    vec2i operator- (const int v) const {
        vec2i newVec {};

        newVec.x = this->x - v;
        newVec.y = this->y - v;

        return newVec;
    };

    vec2i operator- (const vec2i v) const {
        vec2i newVec {};

        newVec.x = this->x - v.x;
        newVec.y = this->y - v.y;

        return newVec;
    };

    vec2i operator* (const int v) const {
        vec2i newVec {};

        newVec.x = this->x * v;
        newVec.y = this->y * v;

        return newVec;
    };

    vec2i operator* (const vec2i v) const {
        vec2i newVec {};

        newVec.x = this->x * v.x;
        newVec.y = this->y * v.y;

        return newVec;
    };

    vec2i operator/ (const int v) const {
        vec2i newVec {};

        newVec.x = this->x / v;
        newVec.y = this->y / v;

        return newVec;
    };

    vec2i operator/ (const vec2i v) const {
        vec2i newVec {};

        newVec.x = this->x / v.x;
        newVec.y = this->y / v.y;

        return newVec;
    };

    vec2i& operator += (const int v) {
        this->x += v; this->y += v;
        return *this;
    };

    vec2i& operator += (const vec2i& v) {
        this->x += v.x; this->y += v.y;
        return *this;
    };

    vec2i& operator -= (const int v) {
        this->x -= v; this->y -= v;
        return *this;
    };

    vec2i& operator -= (const vec2i v) {
        this->x -= v.x; this->y -= v.y;
        return *this;
    };

    vec2i& operator *= (const int v) {
        this->x *= v; this->y *= v;
        return *this;
    };

    vec2i& operator *= (const vec2i v) {
        this->x *= v.x; this->y *= v.y;
        return *this;
    };

    vec2i& operator /= (const int v) {
        this->x /= v; this->y /= v;
        return *this;
    };

    vec2i& operator /= (const vec2i v) {
        this->x /= v.x; this->y /= v.y;
        return *this;
    };
};

struct alignas(16) vec3i {
    union {
        struct {
            int x, y, z;
        };

        int xyz[3] {};
    };

    int& operator [] ( const int idx ) { return xyz[idx]; }

    vec3i operator+ (const int v) const {
        vec3i newVec {};

        newVec.x = this->x + v;
        newVec.y = this->y + v;
        newVec.z = this->z + v;

        return newVec;
    };

    vec3i operator+ (const vec3i v) const {
        vec3i newVec {};

        newVec.x = this->x + v.x;
        newVec.y = this->y + v.y;
        newVec.z = this->z + v.z;

        return newVec;
    };

    vec3i operator- (const int v) const {
        vec3i newVec {};

        newVec.x = this->x - v;
        newVec.y = this->y - v;
        newVec.z = this->z - v;

        return newVec;
    };

    vec3i operator- (const vec3i v) const {
        vec3i newVec {};

        newVec.x = this->x - v.x;
        newVec.y = this->y - v.y;
        newVec.z = this->z - v.z;

        return newVec;
    };

    vec3i operator* (const int v) const {
        vec3i newVec {};

        newVec.x = this->x * v;
        newVec.y = this->y * v;
        newVec.z = this->z * v;

        return newVec;
    };

    vec3i operator* (const vec3i v) const {
        vec3i newVec {};

        newVec.x = this->x * v.x;
        newVec.y = this->y * v.y;
        newVec.z = this->z * v.z;

        return newVec;
    };

    vec3i operator/ (const int v) const {
        vec3i newVec {};

        newVec.x = this->x / v;
        newVec.y = this->y / v;
        newVec.z = this->z / v;

        return newVec;
    };

    vec3i operator/ (const vec3i v) const {
        vec3i newVec {};

        newVec.x = this->x / v.x;
        newVec.y = this->y / v.y;
        newVec.z = this->z / v.z;

        return newVec;
    };

    vec3i& operator += (const int v) {
        this->x += v; this->y += v; this->z += v;
        return *this;
    };

    vec3i& operator += (const vec3i& v) {
        this->x += v.x; this->y += v.y; this->z += v.z;
        return *this;
    };

    vec3i& operator -= (const int v) {
        this->x -= v; this->y -= v; this->z -= v;
        return *this;
    };

    vec3i& operator -= (const vec3i v) {
        this->x -= v.x; this->y -= v.y; this->z -= v.z;
        return *this;
    };

    vec3i& operator *= (const int v) {
        this->x *= v; this->y *= v; this->z *= v;
        return *this;
    };

    vec3i& operator *= (const vec3i v) {
        this->x *= v.x; this->y *= v.y; this->z *= v.z;
        return *this;
    };

    vec3i& operator /= (const int v) {
        this->x /= v; this->y /= v; this->z /= v;
        return *this;
    };

    vec3i& operator /= (const vec3i v) {
        this->x /= v.x; this->y /= v.y; this->z /= v.z;
        return *this;
    };

};

struct alignas(16) vec4i {
    vec4i() : x(0), y(0), z(0), w(0) {};

    union {
        struct {
            int x, y, z, w;
        };

        int xyzw[4] {};
    };

    int& operator [] ( const int idx ) { return xyzw[idx]; }

    vec4i operator+ (const int v) const {
        vec4i newVec {};

        newVec.x = this->x + v;
        newVec.y = this->y + v;
        newVec.z = this->z + v;
        newVec.w = this->w + v;

        return newVec;
    };

    vec4i operator+ (const vec4i v) const {
        vec4i newVec {};

        newVec.x = this->x + v.x;
        newVec.y = this->y + v.y;
        newVec.z = this->z + v.z;
        newVec.w = this->w + v.w;

        return newVec;
    };

    vec4i operator- (const int v) const {
        vec4i newVec {};

        newVec.x = this->x - v;
        newVec.y = this->y - v;
        newVec.z = this->z - v;
        newVec.w = this->w - v;

        return newVec;
    };

    vec4i operator- (const vec4i v) const {
        vec4i newVec {};

        newVec.x = this->x - v.x;
        newVec.y = this->y - v.y;
        newVec.z = this->z - v.z;
        newVec.w = this->w - v.w;

        return newVec;
    };

    vec4i operator* (const int v) const {
        vec4i newVec {};

        newVec.x = this->x * v;
        newVec.y = this->y * v;
        newVec.z = this->z * v;
        newVec.w = this->w * v;

        return newVec;
    };

    vec4i operator* (const vec4i v) const {
        vec4i newVec {};

        newVec.x = this->x * v.x;
        newVec.y = this->y * v.y;
        newVec.z = this->z * v.z;
        newVec.w = this->w * v.w;

        return newVec;
    };

    vec4i operator/ (const int v) const {
        vec4i newVec {};

        newVec.x = this->x / v;
        newVec.y = this->y / v;
        newVec.z = this->z / v;
        newVec.w = this->w / v;

        return newVec;
    };

    vec4i operator/ (const vec4i v) const {
        vec4i newVec {};

        newVec.x = this->x / v.x;
        newVec.y = this->y / v.y;
        newVec.z = this->z / v.z;
        newVec.w = this->w / v.w;

        return newVec;
    };

    vec4i& operator += (const int v) {
        this->x += v; this->y += v; this->z += v; this->w += v;
        return *this;
    };

    vec4i& operator += (const vec4i& v) {
        this->x += v.x; this->y += v.y; this->z += v.z; this->w += v.w;
        return *this;
    };

    vec4i& operator -= (const int v) {
        this->x -= v; this->y -= v; this->z -= v; this->w -= v;
        return *this;
    };

    vec4i& operator -= (const vec4i v) {
        this->x -= v.x; this->y -= v.y; this->z -= v.z; this->w -= v.w;
        return *this;
    };

    vec4i& operator *= (const int v) {
        this->x *= v; this->y *= v; this->z *= v; this->w *= v;
        return *this;
    };

    vec4i& operator *= (const vec4i v) {
        this->x *= v.x; this->y *= v.y; this->z *= v.z; this->w *= v.w;
        return *this;
    };

    vec4i& operator /= (const int v) {
        this->x /= v; this->y /= v; this->z /= v; this->w /= v;
        return *this;
    };

    vec4i& operator /= (const vec4i v) {
        this->x /= v.x; this->y /= v.y; this->z /= v.z; this->w /= v.w;
        return *this;
    };
};

struct alignas(8) vec2f {
    vec2f() : x(0), y(0) {};
    vec2f(const float v) : x( v ), y( v ) {};
    vec2f(const float x, const float y) : x( x ), y( y ) {};

    union {
        struct {
            float x, y;
        };

        float xy[2] {};
    };

    float& operator [] ( const int idx ) { return xy[idx]; }

    vec2f operator+ (const float v) const {
        vec2f newVec {};

        newVec.x = this->x + v;
        newVec.y = this->y + v;

        return newVec;
    };
    vec2f operator+ (const vec2f v) const {
        vec2f newVec {};

        newVec.x = this->x + v.x;
        newVec.y = this->y + v.y;

        return newVec;
    };

    vec2f operator- (const float v) const {
        vec2f newVec {};

        newVec.x = this->x - v;
        newVec.y = this->y - v;

        return newVec;
    };
    vec2f operator- (const vec2f v) const {
        vec2f newVec {};

        newVec.x = this->x - v.x;
        newVec.y = this->y - v.y;

        return newVec;
    };

    vec2f operator* (const float v) const {
        vec2f newVec {};

        newVec.x = this->x * v;
        newVec.y = this->y * v;

        return newVec;
    };
    vec2f operator* (const vec2f v) const {
        vec2f newVec {};

        newVec.x = this->x * v.x;
        newVec.y = this->y * v.y;

        return newVec;
    };

    vec2f operator/ (const float v) const {
        vec2f newVec {};

        newVec.x = this->x / v;
        newVec.y = this->y / v;

        return newVec;
    };
    vec2f operator/ (const vec2f v) const {
        vec2f newVec {};

        newVec.x = this->x / v.x;
        newVec.y = this->y / v.y;

        return newVec;
    };

    vec2f& operator += (const float v) {
        this->x += v; this->y += v;
        return *this;
    };
    vec2f& operator += (const vec2f& v) {
        this->x += v.x; this->y += v.y;
        return *this;
    };

    vec2f& operator -= (const float v) {
        this->x -= v; this->y -= v;
        return *this;
    };
    vec2f& operator -= (const vec2f v) {
        this->x -= v.x; this->y -= v.y;
        return *this;
    };

    vec2f& operator *= (const float v) {
        this->x *= v; this->y *= v;
        return *this;
    };
    vec2f& operator *= (const vec2f v) {
        this->x *= v.x; this->y *= v.y;
        return *this;
    };

    vec2f& operator /= (const float v) {
        this->x /= v; this->y /= v;
        return *this;
    };
    vec2f& operator /= (const vec2f v) {
        this->x /= v.x; this->y /= v.y;
        return *this;
    };
};

struct vec3f {
    vec3f() : x(0), y(0), z(0) {};
    vec3f(const float v) : x( v ), y( v ), z( v ) {};
    vec3f(const vec2f& xyz, const float d) : x(xyz.x), y(xyz.y), z(d){};
    vec3f(const float x, const float y, const float z) : x( x ), y( y ), z( z ) {};

    union {
        struct {
            float x, y, z;
        };

        float xyz[3];
    };

    float& operator [] ( const int idx ) { return xyz[idx]; }

    vec3f operator+ (const float v) const {
        vec3f newVec {};

        newVec.x = this->x + v;
        newVec.y = this->y + v;
        newVec.z = this->z + v;

        return newVec;
    };
    vec3f operator+ (const vec3f v) const {
        vec3f newVec {};

        newVec.x = this->x + v.x;
        newVec.y = this->y + v.y;
        newVec.z = this->z + v.z;

        return newVec;
    };
    vec3f operator- (const float v) const {
        vec3f newVec {};

        newVec.x = this->x - v;
        newVec.y = this->y - v;
        newVec.z = this->z - v;

        return newVec;
    };
    vec3f operator- (const vec3f v) const {
        vec3f newVec {};

        newVec.x = this->x - v.x;
        newVec.y = this->y - v.y;
        newVec.z = this->z - v.z;

        return newVec;
    };
    vec3f operator* (const float v) const {
        vec3f newVec {};

        newVec.x = this->x * v;
        newVec.y = this->y * v;
        newVec.z = this->z * v;

        return newVec;
    };
    vec3f operator* (const vec3f v) const {
        vec3f newVec {};

        newVec.x = this->x * v.x;
        newVec.y = this->y * v.y;
        newVec.z = this->z * v.z;

        return newVec;
    };
    vec3f operator/ (const float v) const {
        vec3f newVec {};

        newVec.x = this->x / v;
        newVec.y = this->y / v;
        newVec.z = this->z / v;

        return newVec;
    };
    vec3f operator/ (const vec3f v) const {
        vec3f newVec {};

        newVec.x = this->x / v.x;
        newVec.y = this->y / v.y;
        newVec.z = this->z / v.z;

        return newVec;
    };

    vec3f& operator += (const float v) {
        this->x += v; this->y += v; this->z += v;
        return *this;
    };

    vec3f& operator += (const vec3f& v) {
        this->x += v.x; this->y += v.y; this->z += v.z;
        return *this;
    };
    vec3f& operator -= (const float v) {
        this->x -= v; this->y -= v; this->z -= v;
        return *this;
    };
    vec3f& operator -= (const vec3f v) {
        this->x -= v.x; this->y -= v.y; this->z -= v.z;
        return *this;
    };
    vec3f& operator *= (const float v) {
        this->x *= v; this->y *= v; this->z *= v;
        return *this;
    };
    vec3f& operator *= (const vec3f v) {
        this->x *= v.x; this->y *= v.y; this->z *= v.z;
        return *this;
    };
    vec3f& operator /= (const float v) {
        this->x /= v; this->y /= v; this->z /= v;
        return *this;
    };
    vec3f& operator /= (const vec3f v) {
        this->x /= v.x; this->y /= v.y; this->z /= v.z;
        return *this;
    };
};

struct alignas(16) vec4f {
    vec4f() : x(0), y(0), z(0), w(0) {};
    vec4f(const float v) : x( v ), y( v ), z( v ), w( v ) {};
    vec4f(const vec3f& xyz, const float d) : x(xyz.x), y(xyz.y), z(xyz.z), w(d) {};
    vec4f(const float x, const float y, const float z, const float w ) : x( x ), y( y ), z( z ), w( w ) {};

    union {
        struct {
            float x, y, z, w;
        };

        float xyzw[4] {};
    };

    float& operator [] ( const int idx ) { return xyzw[idx]; }

    vec4f operator+ (const float v) const {
        vec4f newVec {};

        newVec.x = this->x + v;
        newVec.y = this->y + v;
        newVec.z = this->z + v;
        newVec.w = this->w + v;

        return newVec;
    };
    vec4f operator+ (const vec4f v) const {
        vec4f newVec {};

        newVec.x = this->x + v.x;
        newVec.y = this->y + v.y;
        newVec.z = this->z + v.z;
        newVec.w = this->w + v.w;

        return newVec;
    };
    vec4f operator- (const float v) const {
        vec4f newVec {};

        newVec.x = this->x - v;
        newVec.y = this->y - v;
        newVec.z = this->z - v;
        newVec.w = this->w - v;

        return newVec;
    };
    vec4f operator- (const vec4f v) const {
        vec4f newVec {};

        newVec.x = this->x - v.x;
        newVec.y = this->y - v.y;
        newVec.z = this->z - v.z;
        newVec.w = this->w - v.w;

        return newVec;
    };
    vec4f operator* (const float v) const {
        vec4f newVec {};

        newVec.x = this->x * v;
        newVec.y = this->y * v;
        newVec.z = this->z * v;
        newVec.w = this->w * v;

        return newVec;
    };
    vec4f operator* (const vec4f v) const {
        vec4f newVec {};

        newVec.x = this->x * v.x;
        newVec.y = this->y * v.y;
        newVec.z = this->z * v.z;
        newVec.w = this->w * v.w;

        return newVec;
    };
    vec4f operator/ (const float v) const {
        vec4f newVec {};

        newVec.x = this->x / v;
        newVec.y = this->y / v;
        newVec.z = this->z / v;
        newVec.w = this->w / v;

        return newVec;
    };
    vec4f operator/ (const vec4f v) const {
        vec4f newVec {};

        newVec.x = this->x / v.x;
        newVec.y = this->y / v.y;
        newVec.z = this->z / v.z;
        newVec.w = this->w / v.w;

        return newVec;
    };

    vec4f& operator += (const float v) {
        this->x += v; this->y += v; this->z += v; this->w += v;
        return *this;
    };
    vec4f& operator += (const vec4f& v) {
        this->x += v.x; this->y += v.y; this->z += v.z; this->w += v.w;
        return *this;
    };
    vec4f& operator -= (const float v) {
        this->x -= v; this->y -= v; this->z -= v; this->w -= v;
        return *this;
    };
    vec4f& operator -= (const vec4f v) {
        this->x -= v.x; this->y -= v.y; this->z -= v.z; this->w -= v.w;
        return *this;
    };
    vec4f& operator *= (const float v) {
        this->x *= v; this->y *= v; this->z *= v; this->w *= v;
        return *this;
    };
    vec4f& operator *= (const vec4f v) {
        this->x *= v.x; this->y *= v.y; this->z *= v.z; this->w *= v.w;
        return *this;
    };
    vec4f& operator /= (const float v) {
        this->x /= v; this->y /= v; this->z /= v; this->w /= v;
        return *this;
    };
    vec4f& operator /= (const vec4f v) {
        this->x /= v.x; this->y /= v.y; this->z /= v.z; this->w /= v.w;
        return *this;
    };
};

inline int dot(const vec2i& a, const vec2i& b) { return a.x * b.x + a.y * b.y; }
inline int dot(const vec3i& a, const vec3i& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline int dot(const vec4i& a, const vec4i& b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;}

inline float dot(const vec2f& a, const vec2f& b) { return a.x * b.x + a.y * b.y; }
inline float dot(const vec3f& a, const vec3f& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline float dot(const vec4f& a, const vec4f& b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;}

inline vec3f cross( const vec3f& a, const vec3f& b ) {return vec3f( a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x );}

inline float length( const vec2i& v ) { return sqrtf( (float) dot( v, v ) ); }
inline float length( const vec3i& v ) { return sqrtf( (float) dot( v, v ) ); }
inline float length( const vec4i& v ) { return sqrtf( (float) dot( v, v ) ); }

inline float length(const vec2f& v) { return sqrtf(dot( v, v )); }
inline float length(const vec3f& v) { return sqrtf(dot( v, v )); }
inline float length(const vec4f& v) { return sqrtf(dot( v, v )); }

inline vec2f normalize( const vec2f& v ) { float invLen = 1.0f / dot( v, v ); return v * invLen; }
inline vec3f normalize( const vec3f& v ) { float invLen = 1.0f / dot( v, v ); return v * invLen; }
inline vec4f normalize( const vec4f& v ) { float invLen = 1.0f / dot( v, v ); return v * invLen; }

#endif //ADVANCEDGRAPHICS_MATH_H
