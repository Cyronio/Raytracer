#ifndef LIGHT_H
#define LIGHT_H

class Light {
    public:
    vec3 position;
    vec3 color;

    Light(vec3 position, vec3 color) : position{position}, color{color} {};
};

#endif