#ifndef MESHDATA_H
#define MESHDATA_H

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

class Meshdata {
    public:
    vector<vec3> vertices;
    vector<vec3> normals;
    vector<vector<float>> textures;
    
    vector<vec3> face_vertices;
    vector<vec3> face_textures;
    vector<vec3> face_normals;

    Meshdata(): vertices{vec3()}, normals{vec3()}, textures{}, face_vertices{vec3()}, face_textures{vec3()}, face_normals{vec3()} {};
    Meshdata(vector<vec3> vertices, vector<vec3> normals, vector<vector<float>> textures, vector<vec3> face_vertices, vector<vec3> face_textures, vector<vec3> face_normals) :
    vertices{vertices}, normals{normals}, textures{textures}, face_vertices{face_vertices}, face_textures{face_textures}, face_normals{face_normals} {};


};

//Parser
Meshdata read_Meshdata(string filename) {
    ifstream file(filename);
    string line;
    

    vector<vec3> vertices;
    vector<vec3> normals;
    vector<vector<float>> textures;
    
    vector<vec3> face_vertices;
    vector<vec3> face_textures;
    vector<vec3> face_normals;

    //parsing obj file
    if (file.is_open()) {
        while (getline (file, line)) {
            if (line[0] == 'v' && line[1] != 'n' && line[1] != 't') {
                vector<string> parts;
                istringstream is(line);
                string part;
                while (getline(is, part, ' ')) {
                    parts.push_back(part);
                }
                vec3 vertex = vec3(stof(parts[1]), stof(parts[2]), stof(parts[3]));
                vertices.push_back(vertex);
            }
            if (line[0] == 'v' && line[1] == 'n') {
                vector<string> parts;
                istringstream is(line);
                string part;
                while (getline(is, part, ' ')) {
                    parts.push_back(part);
                }
                vec3 normal = vec3(stof(parts[1]), stof(parts[2]), stof(parts[3]));
                normals.push_back(normal);
            }
            if (line[0] == 'v' && line[1] == 't') {
                vector<string> parts;
                istringstream is(line);
                string part;
                while (getline(is, part, ' ')) {
                    parts.push_back(part);
                }
                vector<float> texture;
                texture.push_back(stof(parts[1]));
                texture.push_back(stof(parts[2]));
                textures.push_back(texture);
            }
            if (line[0] == 'f') {
                vector<string> parts;
                istringstream is(line);
                string part;
                while (getline(is, part, ' ')) {
                    parts.push_back(part);
                }
                vector<string> first, second, third;
                string e1, e2, e3;
                istringstream f_is(parts[1]), s_is(parts[2]), t_is(parts[3]);
                while (getline(f_is, e1, '/')) {
                    first.push_back(e1);
                }
                while (getline(s_is, e2, '/')) {
                    second.push_back(e2);
                }
                while (getline(t_is, e3, '/')) {
                    third.push_back(e3);
                }
                vec3 face = vec3(stof(first[0]), stof(second[0]), stof(third[0])); 
                face_vertices.push_back(face);
                vec3 tex_face = vec3(stof(first[1]), stof(second[1]), stof(third[1])); 
                face_textures.push_back(tex_face);
                vec3 normalFace = vec3(stof(first[2]), stof(second[2]), stof(third[2]));
                face_normals.push_back(normalFace); 
            }
        }
        file.close();
    }
    Meshdata result = Meshdata(vertices, normals, textures, face_vertices, face_textures, face_normals);
    return result;
}

std::ostream& operator<<(std::ostream& o, const Meshdata& m) {
    cout << "Meshdata:\nVertices:\n";
    for (auto a : m.vertices) cout << a << endl;
    cout << "Normals:\n";
    for (auto a : m.normals) cout << a << endl;
    cout << "Textures:\n";
    for (auto a : m.textures) {
        cout << "[" << a[0] << ", " << a[1] << "]" << endl;
    }
    cout << "Face_Vertices:\n";
    for (auto a : m.face_vertices) cout << a << endl;
    cout << "Face_Textures:\n";
    for (auto a : m.face_textures) cout << a << endl;
    cout << "Face_Normals:\n";
    for (auto a : m.face_normals) cout << a << endl;
    return o;
}

#endif