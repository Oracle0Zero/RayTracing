#include "Sphere.h"

Sphere::Sphere()
{
    m_center = glm::vec3(0, 0, 0);
    m_radius = 0;
    m_color = sf::Color::Black;
    m_name = "";
    m_specular = 0;
    m_reflective = 0;
    m_null = true;
}

Sphere::Sphere(glm::vec3 center, float radius, sf::Color color, \
std::string name, int specular, float reflective, bool null) : m_center{center}, \
m_radius{radius}, m_color{color}, m_name{name}, m_specular{specular}, \
m_reflective{reflective}, m_null{null}
{

}

glm::vec3 Sphere::getCenter()
{
    return m_center;
}

float Sphere::getRadius()
{
    return m_radius;
}

sf::Color Sphere::getColor()
{
    return m_color;
}

std::string Sphere::getName()
{
    return m_name;
}

int Sphere::getSpecular()
{
    return m_specular;
}

float Sphere::getReflective()
{
    return m_reflective;
}

bool Sphere::getNull()
{
    return m_null;
}

void Sphere::setCenter(glm::vec3 center)
{
    m_center = center;
}
void Sphere::setRadius(float radius)
{
    m_radius = radius;
}

void Sphere::setColor(sf::Color color)
{
    m_color = color;
}

void Sphere::setName(std::string name)
{
    m_name = name;
}

void Sphere::setSpecular(int specular)
{
    m_specular = specular;
}

void Sphere::setReflective(int reflective)
{
    m_reflective = reflective;
}

void Sphere::setNull(bool null)
{
    m_null = null;
}