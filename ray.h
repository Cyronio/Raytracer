#ifndef RAY_H
#define RAY_H

#include "mesh.h"
#include "meshdata.h"
#include "light.h"

#include <algorithm>
#include <cmath>
#include <vector>
#include <map>

static map<string, vector<vec3>> tex_map;
static map<string, vector<vec3>> surf_tex_map;

float pi = M_PI;

vec3 zero = vec3(); //for checking success or failure

class Ray {
    public:
    vec3 origin;
    vec3 direction;

    Ray(vec3 origin, vec3 direction) : origin{origin}, direction{direction} {}
    Ray(vec3 direction) : origin{vec3(0,0,0)}, direction{direction} {}
    Ray(float d1, float d2, float d3) : origin{vec3(0,0,0)}, direction{vec3(d1, d2, d3)} {}
    Ray(float o1, float o2, float o3, float d1, float d2, float d3) : origin{vec3(o1, o2, o3)}, direction{vec3(d1, d2, d3)} {}

    //intersection test -> returns normal if successful, otherwise zero-vector
    vec3 intersection(Sphere& s) {
        vec3 first = origin - s.center; //cumbersome partwise calculation due to vec3 implemtentation
        float firsty = direction * first; //actually changed in vec3.h now, but left to be sure
        vec3 second = origin - s.center;
        float secondy = second * second;
        float disc = firsty * firsty - (direction * direction) * (secondy - (s.radius * s.radius));
        float t = 0;
        float t1 = 0;
        float t2 = 0;
        if (disc >= 0) { //checking whether discriminant is 0 or higher => if so: intersection test successful
            t1 = (-firsty - sqrt(disc)) / (direction * direction);
            t2 = (-firsty + sqrt(disc)) / (direction * direction);
        }
        t = t1;
        if (t < 0) t = t2;
        
        vec3 tdirect = direction * t;
        vec3 point = tdirect + origin;
        vec3 normal = point - s.center;
        if (disc >= 0 && t > 0) {
            return normal;
        }
        else return zero;
    }

    vector<vec3> intersection(Mesh& m) {
        vector<vec3> faces = m.getFaces();
        vector<vec3> result{zero, zero};
        bool found = false;
        for (unsigned i = 0; i < faces.size(); i += 3) {
            if (found == false) {               
                vec3 v1 = faces[i];
                vec3 v2 = faces[i+1];
                vec3 v3 = faces[i+2];
                vec3 s = origin - v1;
                vec3 e1 = v2 - v1;
                vec3 e2 = v3 - v1;
                direction = normalize(direction);
                float t = ((s % e1) * e2) / ((direction % e2) * e1);
                if (t > 0) {
                    float a = ((direction % e2) * s) / ((direction % e2) * e1);
                    if (a >= 0) {
                        float b = ((s % e1) * direction) / ((direction % e2) * e1);
                        if (b >= 0 && a+b <= 1) {
                            vector<vec3> norms = m.getNormals();
                            vec3 normal = norms[i];
                            normal = normalize(normal);
                            result[0] = normal;
                            vec3 point = (1 - a - b)* v1 + a * v2 + b * v3;
                            result[1] = point;
                            result.push_back(vec3(a,b,i));
                            found = true; //Abbruch nach erstem Erfolg! Beware: weird meshes!
                        }
                    }
                }
            }
        }
        return result;
    }

    //computes color with Phong (ambient, diffuse, specular)
    vector<vec3> illumination(Sphere& s, Light& l) {
        float light_factor = (l.color[0] + l.color[1] + l.color[2]) / 3; //cheesy -> siehe unten
        vec3 normal = intersection(s);
        vec3 view = -direction;
        vec3 point = s.center + normal;
        vec3 light_direction = point - l.position;
        vec3 light = -light_direction;
        normal = normalize(normal);
        view = normalize(view);
        light = normalize(light);
        double nl = normal * light;
        vec3 medi = 2.0 * nl * normal; //again: cumbersome partwise calculation due to vec3 implementation
        vec3 reflect = medi - light;
        reflect = normalize(reflect);
        vec3 color = s.color;

        if (!s.texture_file.empty()) {      
            float u = 0.5 + (atan2(normal[0], normal[2])/ (2 * pi));
            float v = 0.5 - (asin(normal[1])/ pi);           
            float width = ::surf_tex_map[s.texture_file][0][0];
            float height = ::surf_tex_map[s.texture_file][0][1];
            int x = (int) (u * width);
            int y = (int) (v * height);
            int index = y * width + x + 1;
            if (index < 1) index = 1;
            if (index > width*height) index = width*height;
            color = ::surf_tex_map[s.texture_file][index];
        }

        vec3 diffuse = static_cast<float>(std::max(nl, 0.0)) * color;
        double specularF = reflect * view;
        vec3 specular = l.color * 255;
        vec3 ambient = color; 
        specular = pow(std::max(specularF, 0.0), s.phong[3]) * specular;
        ambient = ambient * s.phong[0];
        diffuse = diffuse * s.phong[1];
        diffuse = diffuse  * light_factor;
        specular = specular * s.phong[2];
        vec3 result = specular + diffuse;

        vector<vec3> total_result;

        total_result.push_back(result);
        total_result.push_back(ambient);

        return total_result;
    }

    vector<vec3> illumination(Mesh& m, Light& l, float a, float b, float i, vec3 normal, vec3 point) {
        float light_factor = (l.color[0] + l.color[1] + l.color[2]) / 3; //cheesy! Vielleicht besser: light auf 255 und andere Farben als Faktoren
        vec3 view = -direction;
        vec3 light_direction = point - l.position;
        vec3 light = -light_direction;
        normal = normalize(normal);
        view = normalize(view);
        light = normalize(light);
        double nl = normal * light;
        vec3 medi = 2.0 * nl * normal; //again: cumbersome partwise calculation due to vec3 implementation | eigtl nicht mehr nötig -> verbessert
        vec3 reflect = medi - light;
        reflect = normalize(reflect);
        vec3 color = m.color;
        if (!m.texture_file.empty()) {
            vector<vector<float>> UVs = m.getUVs();
            float u1 = UVs[i][0];
            float v1 = UVs[i][1];
            float u2 = UVs[i+1][0];
            float v2 = UVs[i+1][1];
            float u3 = UVs[i+2][0];
            float v3 = UVs[i+2][1];

            float u = (1 - a - b) * u1 + a * u2 + b * u3;
            float v = (1 - a - b) * v1 + a * v2 + b * v3;

            float width = ::tex_map[m.texture_file][0][0];
            float height = ::tex_map[m.texture_file][0][1];
            int x = (int) (u * width);
            int y = (int) (v * height);
            int index = y * width + x + 1;
            if (index < 1) index = 1;
            if (index > width*height) index = width*height;
            color = ::tex_map[m.texture_file][index];
        }  
             
        vec3 diffuse = static_cast<float>(std::max(nl, 0.0)) * color;
        double specularF = reflect * view;
        vec3 specular = l.color * 255; //ist 178,5 -> truncation problem? seems about right
        vec3 ambient = color;
        specular = pow(std::max(specularF, 0.0), m.phong[3]) * specular;
        ambient = ambient * m.phong[0];  
        diffuse = diffuse * m.phong[1];
        diffuse = diffuse  * light_factor;
        specular = specular * m.phong[2];
        vec3 result = specular + diffuse;
        vector<vec3> total_result;
        total_result.push_back(result);
        total_result.push_back(ambient);
        return total_result;
    }
};

#endif