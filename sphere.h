#ifndef SPHERE_H
#define SPHERE_H

#include <vector>
#include <string>

#include "mat3.h"
#include "vec3.h"

class Sphere {
    public:
    vec3 center;
    float radius;
    vec3 color;
    float phong[4];
    float reflectance;
    float transmittance;
    float refraction;
    std::string texture_file = {};
    vec3 translate = vec3();
    mat3 transform = mat3(vec3(1,0,0), vec3(0,1,0), vec3(0,0,1));

    Sphere(vec3 center, float radius, vec3 color, float ka, float kd, float ks, float exp, float refl, float trans, float refr): center{center}, radius{radius}, color{color}, phong{ka, kd, ks, exp}, reflectance{refl}, transmittance{trans}, refraction{refr} {}
    
};

#endif