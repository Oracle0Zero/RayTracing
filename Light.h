#pragma once
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <string>

enum LightType{
    AMBIENT,
    POINT,
    DIRECTIONAL
};

class Light 
{
public:
    LightType type;
    float intensity;
    glm::vec3 position;
    glm::vec3 direction;
};