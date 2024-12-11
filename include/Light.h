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
private:
    LightType m_type;
    float m_intensity;
    glm::vec3 m_position;
    glm::vec3 m_direction;
public:
    Light();
    Light(LightType, float, glm::vec3, glm::vec3);
    LightType getLightType();
    float getLightIntensity();
    glm::vec3 getLightPosition();
    glm::vec3 getLightDirection();

    void setLightIntensity(float);
};