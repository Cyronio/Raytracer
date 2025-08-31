#include "lodepng.h"
#include "pugixml.hpp"

#include "vec3.h"
#include "sphere.h"
#include "ray.h"
#include "meshdata.h"
#include "mesh.h"
#include "light.h"
#include "mat3.h"
#include "gif.h"

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <cmath>
#include <numbers>
#include <map>

using namespace std;

vec3 trace(Ray ray, vector<Sphere>& spheres, vector<Mesh>& meshes, vector<Light>& lights, vec3& background, int depth /*, map<string, vector<vec3>>& tex_map*/) {
    vec3 color = background;
    vec3 point = zero;
    vec3 refl_direction = zero;
    vec3 ref_data = zero;
    vec3 normal = zero;
    mat3 scaleY = mat3(vec3(1,0,0), vec3(0,2,0), vec3(0,0,1));
    mat3 scaleYback = mat3(vec3(1,0,0), vec3(0,0.5,0), vec3(0,0,1));
    vec3 buffer = ray.direction;
            bool found = false;
            for (auto a : spheres) {
                if (found == false) {
                    ray.direction = a.transform.mult(ray.direction);
                    ray.direction = ray.direction - a.translate;
                    vec3 intersect = ray.intersection(a); //checking for intersection with sphere
                    if (intersect != zero) {
                        vec3 illum_color = zero;
                        for (auto l : lights) {
                            bool foundy = false;
                            point = a.center + intersect;
                            vec3 light_direct = point - l.position;
                            vec3 shadow_direct = -light_direct;
                            shadow_direct = normalize(shadow_direct);
                            Ray shadow = Ray(point, shadow_direct); //creating shadow ray from intersection point to light source
                            
                            for (auto s: spheres) {
                                if (foundy == false) {
                                    vec3 shadow_result = vec3();
                                    if (s.center != a.center) shadow_result = shadow.intersection(s);
                                    vec3 shadow_point = s.center + shadow_result;
                                    vec3 shadow_distance = shadow_point - point;
                                    vec3 light_distance = l.position - point;
                                    float shadow_dist = shadow_distance.getLength();
                                    float light_dist = light_distance.getLength();
                                    if (shadow_result != zero && shadow_dist < light_dist) {
                                        foundy = true;
                                    }
                                }                
                            }
                            if (foundy == false) {
                                illum_color = ray.illumination(a, l)[0];
                                color = color + illum_color;
                                refl_direction = 2 * (-ray.direction * intersect) * intersect + ray.direction;
                                refl_direction = normalize(refl_direction);
                                ref_data = vec3(a.reflectance, a.transmittance, a.refraction);
                                normal = intersect;
                            }     
                        }
                        color = color + (a.texture_file.empty() ? a.color * a.phong[0] : ray.illumination(a, lights[0])[1]);
                        found = true;
                    }   
                }
                ray.direction = buffer; 
            }
            for (auto m : meshes) {
                if (found == false) {
                    ray.direction = m.transform.mult(ray.direction);
                    ray.direction = ray.direction - m.translate;
                    vector<vec3> intersect_data = ray.intersection(m); //checking for intersection with sphere
                    if (intersect_data[0] != zero) {
                        vec3 illum_color = zero;                       
                        for (auto l : lights) {
                            bool foundy = false;
                            point = intersect_data[1];
                            vec3 light_direct = point - l.position;
                            vec3 shadow_direct = -light_direct;
                            shadow_direct = normalize(shadow_direct);
                            Ray shadow = Ray(point+0.01*shadow_direct, shadow_direct); //creating shadow ray from intersection point to light source  
                            for (auto s: spheres) {
                                if (foundy == false) {
                                    vec3 shadow_result = vec3();
                                    shadow_result = shadow.intersection(s);
                                    vec3 shadow_point = s.center + shadow_result;
                                    vec3 shadow_distance = shadow_point - point;
                                    vec3 light_distance = l.position - point;
                                    float shadow_dist = shadow_distance.getLength();
                                    float light_dist = light_distance.getLength();
                                    if (shadow_result != zero && shadow_dist < light_dist) {
                                        foundy = true;
                                    }
                                }               
                            }
                            for (auto h: meshes) {
                                if (foundy == false && h.data.vertices != m.data.vertices) {
                                    vector<vec3> sh_intersect = shadow.intersection(h);
                                    vec3 shadow_point = sh_intersect[1];
                                    vec3 shadow_distance = shadow_point - point;
                                    vec3 light_distance = l.position - point;
                                    float shadow_dist = shadow_distance.getLength();
                                    float light_dist = light_distance.getLength();
                                    if (sh_intersect[0] != zero && shadow_dist < light_dist) {
                                        foundy = true;
                                    }
                                }                
                            }
                            if (foundy == false) {
                                illum_color = ray.illumination(m, l, intersect_data[2][0], intersect_data[2][1], intersect_data[2][2], intersect_data[0], intersect_data[1])[0];
                                color = color + illum_color;
                                refl_direction = 2 * (-ray.direction * intersect_data[0]) * intersect_data[0] + ray.direction;
                                refl_direction = normalize(refl_direction);
                                ref_data = vec3(m.reflectance, m.transmittance, m.refraction);
                                normal = intersect_data[0];   
                            }                 
                        }
                        color = color + (m.texture_file.empty() ? m.color * m.phong[0] : ray.illumination(m, lights[0], intersect_data[2][0], intersect_data[2][1], intersect_data[2][2], intersect_data[0], intersect_data[1])[1]);
                        found = true;    
                    }
                }
                ray.direction = buffer;
            }

            if (found == false) color = background;

            if (depth == 0) return color;

            vec3 refl_color = zero;
            vec3 refr_color = zero;

            if (normal != zero && ref_data[0] > 0) {
                Ray reflector = Ray(point+ 0.01*refl_direction, refl_direction);
                refl_color = ref_data[0] * trace(reflector, spheres, meshes, lights, background, depth-1);               
            }

            if (normal != zero && ref_data[1] > 0) {
                vec3 v = normalize(ray.direction);
                vec3 n = normalize(normal);
                vec3 refr_direction;
                bool total = false;

                if (v*n < 0) {                    
                    float snell = 1/ref_data[2];                   
                    float vn = -(v * n);
                    refr_direction = snell * (v + n * (vn)) - n * sqrt(1 - snell * snell * (1 - ((vn) * (vn))));
                }

                if (v*n > 0) {
                    float snello = ref_data[2];                   
                    float disc = 1 - snello * snello * (1 - ((v * n) * (v * n)));
                    if (disc >= 0) {
                        refr_direction = snello * (v - n * (v * n)) + n * sqrt(disc);       
                    }
                    if (disc < 0) {
                        refr_direction = 2 * (-refr_direction * n) * n + refr_direction;
                        total = true;                       
                    }               
                }

                refr_direction = normalize(refr_direction);
                Ray refractor = Ray(point+ 0.01*refr_direction, refr_direction);
                if (total == false) refr_color = ref_data[1] * trace(refractor, spheres, meshes, lights, background, depth-1);
                else refr_color = ref_data[0] * trace(refractor, spheres, meshes, lights, background, depth-1);
            }
    
        color = color * (1 - ref_data[0] - ref_data[1]) + refl_color + refr_color;

    return color;
}

vector<vec3> getTexture(string filename) {
    std::vector<unsigned char> texture;
    unsigned w = 0;
    unsigned h = 0;
    lodepng::decode(texture, w, h, filename);

    vector<vec3> bitmap;
    vec3 ratio = vec3(w,h,0);
    bitmap.push_back(ratio);
    for (ulong i = 0; i < texture.size(); i+=4) {
        float r = texture[i];
        float g = texture[i+1];
        float b = texture[i+2];
        vec3 pixel = vec3(r,g,b);
        bitmap.push_back(pixel);
    }
    return bitmap; //attention: retuns vec3 with width and height as first element
}


int main(int argc, char *argv[]) {

    unsigned frame_count = 1;
    bool anim = false; 
    bool blur = false;
    bool super = false;
    
    GifWriter writer = {};

    //checking for correct input and grabbing xml-filename
    const char* xml_file;
    if (argc > 1) xml_file = argv[1];
    else return 1;

    if (argc > 2) {
        string second =argv[2];
        if (!second.compare("-anim")) {
            anim = true;
        }
        if (!second.compare("-blur")) {
            blur = true;
        }
        if (!second.compare("-super")) {
            super = true;
        }
    } 

    if (anim == true) frame_count = 12;
    if (blur == true) frame_count = 3;
    if (super == true) frame_count = 4;

    //loading xml file
    pugi::xml_document doc;
    doc.load_file(xml_file);

    //parsing xml data and implementing it
    
    //background
    vector<float> scene_back;
    for (auto a: doc.child("scene").child("background_color").attributes()) {
        scene_back.push_back(stof(a.value()));
    }
    vec3 background = vec3(scene_back[0], scene_back[1], scene_back[2]);

    //camera
    vector<float> camera_pos;
    for (auto a: doc.child("scene").child("camera").child("position").attributes()) {
        camera_pos.push_back(stof(a.value()));
    }
    vec3 camera_position = vec3(camera_pos[0], camera_pos[1], camera_pos[2]);

    vector<float> camera_look;
    for (auto a: doc.child("scene").child("camera").child("lookat").attributes()) {
        camera_look.push_back(stof(a.value()));
    }
    vec3 camera_lookat = vec3(camera_look[0], camera_look[1], camera_look[2]);

    vector<float> camera_u;
    for (auto a: doc.child("scene").child("camera").child("up").attributes()) {
        camera_u.push_back(stof(a.value()));
    }
    vec3 camera_up = vec3(camera_u[0], camera_u[1], camera_u[2]);

    //camera transformation
    vec3 Z = camera_position - camera_lookat;
    Z = normalize(Z);
    vec3 X = camera_up % Z;
    X = normalize(X);
    vec3 Y = Z % X;
    Y = normalize(Y);

    mat3 cam_trans = mat3(X, Y, Z);

    //output file
    string file;
    string output_file = doc.child("scene").attribute("output_file").value();

    file = output_file;

    //adjusting output file for flags
    string giffy;
    string blurry;

    if (anim == true) {
        giffy = file.substr(0, file.size()-4) + ".gif";
    }

    if (blur == true) {
        blurry = file.substr(0, file.size()-4) + "_blur.png";
        file = blurry;
    }

    if (super == true) {
        blurry = file.substr(0, file.size()-4) + "_super.png";
        file = blurry;
    }

    const char* gif_filename = giffy.c_str();

    //resolution
    unsigned scene_width = stof(doc.child("scene").child("camera").child("resolution").attribute("horizontal").value());
    unsigned scene_height = stof(doc.child("scene").child("camera").child("resolution").attribute("vertical").value());

    unsigned width = scene_width;
    unsigned height = scene_height;

    //angle
    float angle = stof(doc.child("scene").child("camera").child("horizontal_fov").attribute("angle").value());

    float fovx = angle * pi/180;
    float fovy = height / width * fovx;

    //reflection/refraction depth
    int max_bounce = static_cast<int>(stof(doc.child("scene").child("camera").child("max_bounces").attribute("n").value()));

    //parsing spheres data
    vector<vector<float>> sphere_data;
    vector<vector<float>> s_translate_data;
    vector<mat3> s_rotate_data;

    vector<string> surf_tex_files;
    string surf_tex_file = {};

    vector<vector<float>> s_scale_data;

    for (auto c : doc.child("scene").child("surfaces").children("sphere")) {
        vector<float> s;
        vector<float> translate;
        vector<float> scale;
        mat3 rotate = mat3();
        if (c.first_child().next_sibling() == c.child("material_solid")) {
            for (auto a: c.child("position").attributes()) {
            s.push_back(stof(a.value()));
            }
            s.push_back(stof(c.attribute("radius").value()));
            for (auto a: c.child("material_solid").child("color").attributes()) {
            s.push_back(roundf(255*stof(a.value())));
            }
            for (auto a: c.child("material_solid").child("phong").attributes()) {
            s.push_back(stof(a.value()));
            }
            s.push_back(stof(c.child("material_solid").child("reflectance").attribute("r").value()));
            s.push_back(stof(c.child("material_solid").child("transmittance").attribute("t").value()));
            s.push_back(stof(c.child("material_solid").child("refraction").attribute("iof").value()));
            sphere_data.push_back(s);
        }

        else if (c.first_child().next_sibling() == c.child("material_textured")) {
            for (auto a: c.child("position").attributes()) {
            s.push_back(stof(a.value()));
            }
            s.push_back(stof(c.attribute("radius").value()));
            for (int i = 0; i < 3; i++) {
                s.push_back(0.0);
            }
            for (auto a: c.child("material_textured").child("phong").attributes()) {
            s.push_back(stof(a.value()));
            }
            s.push_back(stof(c.child("material_textured").child("reflectance").attribute("r").value()));
            s.push_back(stof(c.child("material_textured").child("transmittance").attribute("t").value()));
            s.push_back(stof(c.child("material_textured").child("refraction").attribute("iof").value()));
            sphere_data.push_back(s);

            surf_tex_file = surf_tex_file + c.child("material_textured").child("texture").attribute("name").value();
        }

        for (auto a: c.child("transform").child("translate").attributes()) {
            translate.push_back(stof(a.value()));
        }

        for (auto a: c.child("transform").child("scale").attributes()) {
            scale.push_back(stof(a.value()));
        }
        for (auto a: c.child("transform").child("rotateX").attributes()) {
            rotate = rotateX(-stof(a.value()));
        }
        for (auto a: c.child("transform").child("rotateY").attributes()) {
            rotate = rotateY(-stof(a.value()));
        }
        for (auto a: c.child("transform").child("rotateZ").attributes()) {
            rotate = rotateZ(-stof(a.value()));
        }

        surf_tex_files.push_back(surf_tex_file);

        s_translate_data.push_back(translate);
        s_scale_data.push_back(scale);
        s_rotate_data.push_back(rotate);
    }

    //parsing mesh data
    vector<vector<float>> mesh_cpdata;
    vector<Meshdata> mesh_meshdata;

    vector<string> tex_files;
    string tex_file = {}; //nicht besser unten rein?

    vector<vector<float>> m_translate_data;
    vector<vector<float>> m_scale_data;
    vector<mat3> m_rotate_data;

    for (auto c : doc.child("scene").child("surfaces").children("mesh")) {
        vector<float> m;
        vector<float> translate;
        vector<float> scale;
        mat3 rotate = mat3();
        if (c.first_child() == c.child("material_solid")) {
            for (auto a: c.child("material_solid").child("color").attributes()) {
                m.push_back(roundf(255*stof(a.value())));
            }
            for (auto a: c.child("material_solid").child("phong").attributes()) {
                m.push_back(stof(a.value()));
            }
            m.push_back(stof(c.child("material_solid").child("reflectance").attribute("r").value()));
            m.push_back(stof(c.child("material_solid").child("transmittance").attribute("t").value()));
            m.push_back(stof(c.child("material_solid").child("refraction").attribute("iof").value()));
            mesh_cpdata.push_back(m);
        }

        else if (c.first_child() == c.child("material_textured")) {
            for (int i = 0; i < 3; i++) {
                m.push_back(0.0);
            }
            for (auto a: c.child("material_textured").child("phong").attributes()) {
                m.push_back(stof(a.value()));
            }
            m.push_back(stof(c.child("material_textured").child("reflectance").attribute("r").value()));
            m.push_back(stof(c.child("material_textured").child("transmittance").attribute("t").value()));
            m.push_back(stof(c.child("material_textured").child("refraction").attribute("iof").value()));
            mesh_cpdata.push_back(m);
            
            tex_file = tex_file + c.child("material_textured").child("texture").attribute("name").value();
        }

        for (auto a: c.child("transform").child("translate").attributes()) {
            translate.push_back(stof(a.value()));
        }
        for (auto a: c.child("transform").child("scale").attributes()) {
            scale.push_back(stof(a.value()));
        }
        for (auto a: c.child("transform").child("rotateX").attributes()) {
            rotate = rotateX(-stof(a.value()));
        }
        for (auto a: c.child("transform").child("rotateY").attributes()) {
            rotate = rotateY(-stof(a.value()));
        }
        for (auto a: c.child("transform").child("rotateZ").attributes()) {
            rotate = rotateZ(-stof(a.value()));
        }

        m_translate_data.push_back(translate);
        m_scale_data.push_back(scale);
        m_rotate_data.push_back(rotate);
        tex_files.push_back(tex_file);
    }

    for (auto c : doc.child("scene").child("surfaces").children("mesh")) {
        Meshdata m = read_Meshdata(c.attribute("name").value());
        mesh_meshdata.push_back(m);
    }

    //parallel light - not working!
    vector<float> light_direction;
    for (auto a: doc.child("scene").child("lights").child("parallel_light").child("direction").attributes()) {
        light_direction.push_back(stof(a.value()));
    }
    vec3 light_direct;
    if (!light_direction.empty()) light_direct = vec3(light_direction[0], light_direction[1], light_direction[2]);

    //point light
    vector<Light> lights;
    vector<vector<float>> light_data;

    for (auto c: doc.child("scene").child("lights").children("point_light")) {
        vector<float> l;
        for (auto a: c.child("position").attributes()) {
            l.push_back(stof(a.value()));
        }
        for (auto a: c.child("color").attributes()) {
            l.push_back(stof(a.value()));
        }
        light_data.push_back(l);
    }
    
    for (auto l : light_data) {
        lights.push_back(Light(vec3(l[0], l[1], l[2]), vec3(l[3], l[4], l[5])));
    }


    //creating spheres
    vector<Sphere> spheres;

    map<string, vector<vec3>> surf_te_map;

    for (unsigned i = 0; i < sphere_data.size(); i++) {
        vec3 position = vec3(sphere_data[i][0], sphere_data[i][1], sphere_data[i][2]);
        float radius = sphere_data[i][3];
        vec3 color = vec3(sphere_data[i][4], sphere_data[i][5], sphere_data[i][6]);
        Sphere entry = Sphere(position, radius, color, sphere_data[i][7], sphere_data[i][8], sphere_data[i][9], sphere_data[i][10], sphere_data[i][11], sphere_data[i][12], sphere_data[i][13]);
        if (!surf_tex_files[i].empty()) {
            vector<vec3> textures = getTexture(surf_tex_files[i]);
            surf_te_map[surf_tex_files[i]] = textures;
            entry.texture_file = surf_tex_files[i];
        }
        if (!s_translate_data[i].empty()) {
            vec3 transl = vec3(s_translate_data[i][0], s_translate_data[i][1], s_translate_data[i][2]);
            entry.translate = transl;
        }
        if (!s_scale_data[i].empty()) {
            mat3 scalo = mat3(vec3(1.0/s_scale_data[i][0],0,0), vec3(0, 1.0/s_scale_data[i][1], 0), vec3(0,0, 1.0/s_scale_data[i][2]));
            entry.transform = scalo;
        }
        entry.transform =  entry.transform % s_rotate_data[i];
        spheres.push_back(entry);
    }

    //creating meshes
    vector<Mesh> meshe;

    map<string, vector<vec3>> te_map;

    for (unsigned i = 0; i < mesh_meshdata.size(); i++) {
        Meshdata data = mesh_meshdata[i];
        vec3 color = vec3(mesh_cpdata[i][0], mesh_cpdata[i][1], mesh_cpdata[i][2]);
        Mesh entry = Mesh(data, color, mesh_cpdata[i][3], mesh_cpdata[i][4], mesh_cpdata[i][5], mesh_cpdata[i][6], mesh_cpdata[i][7], mesh_cpdata[i][8], mesh_cpdata[i][9]);
        if (!tex_files[i].empty()) {
            vector<vec3> textures = getTexture(tex_files[i]);
            te_map[tex_files[i]] = textures;
            entry.texture_file = tex_files[i];
        }
        if (!m_translate_data[i].empty()) {
            vec3 transl = vec3(m_translate_data[i][0], m_translate_data[i][1], m_translate_data[i][2]);
            entry.translate = transl;
        }
        if (!m_scale_data[i].empty()) {
            mat3 scalo = mat3(vec3(1.0/m_scale_data[i][0],0,0), vec3(0, 1.0/m_scale_data[i][1], 0), vec3(0,0, 1.0/m_scale_data[i][2]));
            entry.transform = scalo; 
        }
        entry.transform = m_rotate_data[i] % entry.transform;
        meshe.push_back(entry); 
    }

    ::tex_map = te_map; //for performance increase: globale variable
    ::surf_tex_map = surf_te_map;
    static vector<Mesh> meshes {meshe}; //also tried to increase persormance by that - didn't change afterwards

    //for animation: setting width and height down to 256 for performance
    if (anim == true) {

        width = 256;
        height = 256;
        
        //gif.h
        GifBegin( &writer, gif_filename, width, height, 8, 8, false);
        
    } 
    
    //image -> RGBA
    vector<unsigned char> image;
    image.resize(width * height * 4);

    //for multi-frame renderings
    vector<vector<unsigned char>> images;

    //for animation to set radius back to default
    vector<float> og_radius;

    //for super-sampling
    //difference between two pixels
    float pix_diff_x = 2.0 / width * tan(fovx);
    float pix_diff_y = 2.0 / height * tan(fovy);

    float s_diff_x = pix_diff_x * 0.25;
    float s_diff_y = pix_diff_y * 0.25;

    //coordinates for points inside each pixel
    vector<float> pixel_x_tilts {s_diff_x,-s_diff_x, s_diff_x, -s_diff_x};
    vector<float> pixel_y_tilts {s_diff_y, s_diff_y, -s_diff_y,-s_diff_y};
    
    //main render loop
    while (frame_count > 0) {
        
        if (anim == true) { 
            for (auto &s: spheres) {
                og_radius.push_back(s.radius);
                s.radius = s.radius + s.radius * 0.5* (sin((12-frame_count) * pi / 6));
            } 
        }

        if (blur == true) {   
            for (auto &s : spheres) {
                if (frame_count == 2) s.center = s.center + vec3(0.1,0.0,0.05);
                if (frame_count == 1) s.center = s.center + vec3(-0.2,0.05,-0.1);
            }
        }
        
        for(unsigned y = 0; y < height; y++) {
            for(unsigned x = 0; x < width; x++) {
                float xn = (x + 0.5) / width;
                float yn = (y + 0.5) / height;
                float xi = 2 * xn - 1;
                float yi = 2 * yn - 1;
                xi = xi * tan(fovx) + (super == true ? pixel_x_tilts[frame_count-1] : 0); //supersampling
                yi = yi * tan(fovy) + (super == true ? pixel_y_tilts[frame_count-1] : 0); //supersampling
                vec3 direction = vec3(xi, yi, -1); 
                direction = normalize(direction);
                direction = cam_trans.mult(direction);
                Ray ray = Ray(camera_position, direction); //creating ray from camera position through pixel
                int depth = max_bounce;
                vec3 color = trace(ray, spheres, meshes, lights, background, depth); 
          
                //mapping color information: height-y-1 due to flipped image, capping color at 255 (structure copied from lodepng-example)           
                image[4 * width * (height-y-1) + 4 * x + 0] = color[0] > 255 ? 255 : color[0];
                image[4 * width * (height-y-1) + 4 * x + 1] = color[1] > 255 ? 255 : color[1];
                image[4 * width * (height-y-1) + 4 * x + 2] = color[2] > 255 ? 255 : color[2];
                image[4 * width * (height-y-1) + 4 * x + 3] = 255;            
            }
        }

    //encoding file
    if (anim == false && blur == false && super == false) {
        lodepng::encode(file, image, width, height);
    }
    else if (anim == true) {
        uint8_t gif_image[262144]; //= width*height*4 for width, height = 256
        for (unsigned u = 0; u < width*height*4; u++) {
            gif_image[u] = static_cast<uint8_t>(image[u]);
        }
        GifWriteFrame( &writer, gif_image, width, height, 8, 8, false);
        for (unsigned i = 0; i < spheres.size(); i++) {
            spheres[i].radius = og_radius[i];
        }
        og_radius.clear();
        cout << 13-frame_count << "/" << 12 << endl;
        
    }

    else if (blur == true) {
        images.push_back(image);
        cout << 4-frame_count << "/" << 3 << endl;
    }

    else if (super == true) {
        images.push_back(image);
        cout << 5-frame_count << "/" << 4 << endl;
    }
    frame_count--;
    }
    
    if (blur == true) {
        vector<unsigned char> blur_image;
        for (unsigned i = 0; i < width*height*4; i++) {
            unsigned char pixel = (images[0][i] + images[1][i] + images[2][i])/3;
            blur_image.push_back(pixel);
        }
        lodepng::encode(file, blur_image, width, height);
    }

    if (super == true) {
        vector<unsigned char> blur_image;
        for (unsigned i = 0; i < width*height*4; i++) {
            unsigned char pixel = (images[0][i] + images[1][i] + images[2][i]  + images[3][i])/4;
            blur_image.push_back(pixel);
        }
        lodepng::encode(file, blur_image, width, height);
    }

    GifEnd( &writer );

    return 0;
}