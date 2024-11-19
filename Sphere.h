#pragma once
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <string>

class Sphere
{
public:
    glm::vec3 center;
    float radius;
    sf::Color color;
    std::string name;
    int specular;
    bool null;
    float reflective;

    Sphere()
    {
        null = false;
    };
};