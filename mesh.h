#ifndef MESH_H
#define MESH_H

#include "meshdata.h"
#include "vec3.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <string>

using namespace std;

class Mesh {
    public:
    Meshdata data;
    vec3 color = vec3();
    float phong[4];
    float reflectance;
    float transmittance;
    float refraction;
    string texture_file = {};
    vec3 translate = vec3();
    mat3 transform = mat3(vec3(1,0,0), vec3(0,1,0), vec3(0,0,1));

    Mesh(Meshdata data, vec3 color, float ka, float kd, float ks, float exp, float refl, float trans, float refr): data{data}, color{color}, phong{ka, kd, ks, exp}, reflectance{refl}, transmittance{trans}, refraction{refr} {};

    vector<vec3> getFaces() {
        vector<vec3> vertices = this->data.vertices;
        vector<vec3> face_vertices = this->data.face_vertices;
        vector<vec3> faces; 
        for (unsigned j = 0; j < face_vertices.size(); j++) {
            for (int i = 0; i < 3; i++) {
                faces.push_back(vertices[(face_vertices[j][i])-1]);
            }
        }
        return faces;
    }

    vector<vector<float>> getUVs() {
        vector<vector<float>> textures = this->data.textures;
        vector<vec3> face_textures = this->data.face_textures;
        vector<vector<float>> UVs;
        for (unsigned j = 0; j < face_textures.size(); j++) {
            for (int i = 0; i < 3; i++) {
                UVs.push_back(textures[(face_textures[j][i])-1]);
            }
        }
        return UVs;
    }

    vector<vec3> getNormals() {
        vector<vec3> normals = this->data.normals;
        vector<vec3> face_normals = this->data.face_normals;
        vector<vec3> norms;
        for (unsigned j = 0; j < face_normals.size(); j++) {
            for (int i = 0; i < 3; i++) {
                norms.push_back(normals[(face_normals[j][i])-1]);
            }
        }
        return norms;
    }
};

#endif

