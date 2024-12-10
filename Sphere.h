#pragma once
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <string>

class Sphere
{
private:
    glm::vec3 m_center;
    float m_radius;
    sf::Color m_color;
    std::string m_name;
    int m_specular;
    float m_reflective;
    bool m_null;
public:
    Sphere();
    Sphere(glm::vec3, float, sf::Color, std::string, int, float, bool);

    glm::vec3 getCenter();
    float getRadius();
    sf::Color getColor();
    std::string getName();
    int getSpecular();
    float getReflective();
    bool getNull();

    void setCenter(glm::vec3);
    void setRadius(float);
    void setColor(sf::Color);
    void setName(std::string);
    void setSpecular(int);
    void setReflective(int);
    void setNull(bool);
};