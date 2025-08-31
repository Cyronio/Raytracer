#ifndef VEC3_H
#define VEC3_H

#include <iostream>
#include <cmath>


class vec3 {
    public:
    float data[3];
    
    vec3(float a, float b, float c) : data{a, b, c} {};
    vec3() : data{0,0,0} {};

    float getLength() const {
        return sqrt(data[0]*data[0] + data[1]*data[1] + data[2]*data[2]);
    }

    float operator[](int i) {
        return data[i];
    }

    vec3 operator-() {
        return vec3(-data[0], -data[1], -data[2]);
    }

};

vec3 operator+(const vec3& lop, const vec3& rop) {
    return vec3(lop.data[0] + rop.data[0], lop.data[1] + rop.data[1], lop.data[2] + rop.data[2]);
}

vec3 operator-(const vec3& lop, const vec3& rop) {
    return vec3(lop.data[0] - rop.data[0], lop.data[1] - rop.data[1], lop.data[2] - rop.data[2]);
}

vec3 operator*(const vec3& lop, float rop) {
    return vec3(lop.data[0] * rop, lop.data[1] * rop, lop.data[2] * rop);
}

vec3 operator*(float lop, const vec3& rop) {
    return vec3(rop.data[0] * lop, rop.data[1] * lop, rop.data[2] * lop);
}

vec3 operator/(const vec3& lop, float rop) {
    return vec3(lop.data[0] / rop, lop.data[1] / rop, lop.data[2] / rop);
}

float dot(const vec3& lop, const vec3& rop) {
    return lop.data[0] * rop.data[0] + lop.data[1] * rop.data[1] + lop.data[2] * rop.data[2];
}

vec3 cross(const vec3& lop, const vec3& rop) {
    return vec3(lop.data[1] * rop.data[2] - lop.data[2] * rop.data[1], lop.data[2] * rop.data[0] - lop.data[0] * rop.data[2], lop.data[0] * rop.data[1] - lop.data[1] * rop.data[0]);
}

vec3 normalize(const vec3& v) {
    return v / v.getLength();
}

vec3 toColor(const vec3& v) {
    vec3 uno = vec3(1,1,1);
    vec3 result = v + uno;
    result = result / 2;
    result = result * 255;
    return result;
}

float operator*(const vec3& lop, const vec3& rop) {
    return dot(lop, rop);
}

vec3 operator%(const vec3& lop, const vec3& rop) {
    return cross(lop, rop);
}

bool operator==(const vec3& lop, const vec3& rop) {
    return (lop.data[0] == rop.data[0] && lop.data[1] == rop.data[1] && lop.data[2] == rop.data[2]);
}

bool operator!=(const vec3& lop, const vec3& rop) {
    return !(lop == rop);
}


std::ostream& operator<<(std::ostream& o, const vec3& v) {
    return o << "[" << v.data[0] << ", " << v.data[1] << ", " << v.data[2] << "]";
}

#endif