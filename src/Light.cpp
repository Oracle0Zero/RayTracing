#include "../include/Light.h"

Light::Light()
{
    m_type = LightType::AMBIENT;
    m_intensity = 0.0f;
    m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    m_direction = glm::vec3(0.0f, 0.0f, 0.0f);
}


Light::Light(LightType type, float intensity, glm::vec3 position, glm::vec3 direction) \
: m_type{type}, m_intensity{intensity}, m_position{position}, m_direction{direction}
{

}

LightType Light::getLightType()
{
    return m_type;
}

float Light::getLightIntensity()
{
    return m_intensity;
}

glm::vec3 Light::getLightPosition()
{
    return m_position;
}

glm::vec3 Light::getLightDirection()
{
    return m_direction;
}

void Light::setLightIntensity(float intensity)
{
    m_intensity = intensity;
}

