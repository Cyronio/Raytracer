#ifndef MAT3_H
#define MAT3_H

#include "vec3.h"

#include <iostream>
#include <cmath>

class mat3 {
    public:
    float data[9];

    mat3(vec3 x, vec3 y, vec3 z) : data{x[0], x[1], x[2], y[0], y[1], y[2],z[0], z[1], z[2]} {}
    mat3() : data{1,0,0,0,1,0,0,0,1} {} //Einheitsmatrix

    vec3 mult(vec3 v) {
        float x = data[0] * v[0] + data[3] * v[1] + data[6] * v[2];
        float y = data[1] * v[0] + data[4] * v[1] + data[7] * v[2];
        float z = data[2] * v[0] + data[5] * v[1] + data[8] * v[2];
        return vec3(x,y,z);
        
    }
};

mat3 operator%(const mat3& lop, const mat3& rop) {
    vec3 first = vec3(lop.data[0] * rop.data[0] + lop.data[3] * rop.data[1] + lop.data[6] * rop.data[2],
    lop.data[1] * rop.data[0] + lop.data[4] * rop.data[1] + lop.data[7] * rop.data[2],
    lop.data[2] * rop.data[0] + lop.data[5] * rop.data[1] + lop.data[8] * rop.data[2]);
    vec3 second = vec3(lop.data[0] * rop.data[3] + lop.data[3] * rop.data[4] + lop.data[6] * rop.data[5],
    lop.data[1] * rop.data[3] + lop.data[4] * rop.data[4] + lop.data[7] * rop.data[5],
    lop.data[2] * rop.data[3] + lop.data[5] * rop.data[4] + lop.data[8] * rop.data[5]);
    vec3 third = vec3(lop.data[0] * rop.data[6] + lop.data[3] * rop.data[7] + lop.data[6] * rop.data[8],
    lop.data[1] * rop.data[6] + lop.data[4] * rop.data[7] + lop.data[7] * rop.data[8],
    lop.data[2] * rop.data[6] + lop.data[5] * rop.data[7] + lop.data[8] * rop.data[8]);
    
    return mat3(first, second, third);

}

std::ostream& operator<<(std::ostream& o, const mat3& m) {
    return o << "[" << m.data[0] << ", " << m.data[3] << ", " << m.data[6] << "]" << std::endl <<
    "[" << m.data[1] << ", " << m.data[4] << ", " << m.data[7] << "]" << std::endl <<
    "[" << m.data[2] << ", " << m.data[5] << ", " << m.data[8] << "]" << std::endl;
}

//rotation matrices
mat3 rotateX(float angle) {
    float radian = angle * M_PI / 180;
    float sinus = sin(radian);
    float cosinus = cos(radian);
    return mat3(vec3(1,0,0), vec3(0, cosinus, sinus), vec3(0, -sinus, cosinus));
}
mat3 rotateY(float angle) {
    float radian = angle * M_PI / 180;
    float sinus = sin(radian);
    float cosinus = cos(radian);
    return mat3(vec3(cosinus,0,-sinus), vec3(0, 1, 0), vec3(sinus, 0, cosinus));
}
mat3 rotateZ(float angle) {
    float radian = angle * M_PI / 180;
    float sinus = sin(radian);
    float cosinus = cos(radian);
    return mat3(vec3(cosinus,sinus,0), vec3(-sinus, cosinus, 0), vec3(0, 0, 1));
}

#endif