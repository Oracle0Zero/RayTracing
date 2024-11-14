#pragma once
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <string>

class Sphere
{
public:
    glm::vec3 location;
    float radius;
    sf::Color color;
    std::string name;
    int specular;
    bool exists;
    float reflective;

    Sphere()
    {
        exists = false;
    };
};