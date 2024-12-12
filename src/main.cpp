#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <omp.h>

#include "../include/Sphere.h"
#include "../include/Light.h"


constexpr int canvas_width = 1280;
constexpr int canvas_height = 720;

constexpr float viewport_width = 2;
constexpr float viewport_height = 1;

int d = 1;

float camera_x = 0;
float camera_y = 0;
float camera_z = 0.5;
glm::vec3 O(camera_x, camera_y, camera_z);

float inf = 10000.0f;
int recursion_depth = 1;
float closest_t = inf;


    
std::vector<Sphere> spheres;
std::vector<Light> lights;

void PutPixel(sf::RenderWindow& window, sf::RectangleShape& pixel, int x, int y, sf::Color color);
glm::vec3 CanvasToViewPort(int x, int y);
sf::Color TraceRay(glm::vec3 O, glm::vec3 D, float t_min, float t_max, int recursion_depth);
void IntersectRaySphere(glm::vec3 camera_vector, glm::vec3 view_direction_vector, Sphere& sphere, float& t1, float& t2);
float ComputeLighting(glm::vec3 p, glm::vec3 n, glm::vec3 v, int s);
void ClosestIntersection(glm::vec3 O, glm::vec3 D, float t_min, float t_max, Sphere& closest_sphere, float& closest_t);
glm::vec3 ReflectRay(glm::vec3 R, glm::vec3 N);
void ProcessEvents();

int main()
{
    sf::RenderWindow window(sf::VideoMode(canvas_width, canvas_height), "SFML works!");
    
    Sphere sphere_1{glm::vec3(0, -1, 3), 1, sf::Color::Red, "Red", 500, 0.2f, false};
    spheres.push_back(sphere_1);

    Sphere sphere_2{glm::vec3(2, 0, 4), 1, sf::Color::Blue, "Blue", 500, 0.3f, false};
    spheres.push_back(sphere_2);

    Sphere sphere_3{glm::vec3(-2, 0, 4), 1, sf::Color::Green, "Green", 10, 0.4f, false};
    spheres.push_back(sphere_3);
    
    Sphere sphere_4{glm::vec3(0, -5001, 0), 5000, sf::Color::Yellow, "Yellow", 1000,\
      0.5f, false};
    spheres.push_back(sphere_4);

    Light ambient{LightType::AMBIENT, 0.2f, glm::vec3(0.0f, 0.0f, 0.0f), \
        glm::vec3(0.0f, 0.0f, 0.0f)};
    lights.push_back(ambient);

    Light point{LightType::POINT, 0.6f, glm::vec3(2, 1, 0), \
        glm::vec3(0.0f, 0.0f, 0.0f)};
    lights.push_back(point);

    Light directional{LightType::DIRECTIONAL, 0.2f, glm::vec3(0.0f, 0.0f, 0.0f), \
        glm::vec3(1, 4, 4)};
    lights.push_back(directional);
    
    sf::RectangleShape pixel(sf::Vector2f(1, 1));

    glm::vec3 viewport_vector;
    glm::vec3 D;
    sf::Color color;

    sf::Color pixel_colors[canvas_width][canvas_height];
    sf::Color pixel_colors_1D_array[canvas_width * canvas_height];

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {

            if (event.type == sf::Event::Closed)
                window.close();
        }

        ProcessEvents();

        
        window.clear();
       
        for (int i = (- canvas_width / 2); i < (canvas_width / 2); ++i)
        {
            #pragma omp parallel for private(viewport_vector, D, color)
            for (int j = (- canvas_height / 2); (j < canvas_height / 2); ++j)
            {
                viewport_vector = CanvasToViewPort(i, j);
                D = viewport_vector - O;

                color = TraceRay(O, D, 1.0f, inf, recursion_depth);

                float c_x = (float)canvas_width / 2 + i;
                float c_y = (float)canvas_height / 2 - j;

                pixel_colors[static_cast<int>(c_x)][static_cast<int>(c_y)] = color;
            }
        }

        for (int i = 0; i < canvas_width; ++i)
        {
            for (int j = 0; j < canvas_height; ++j)
            {
                pixel.setPosition(i, j);
                pixel.setFillColor(pixel_colors[i][j]);
                window.draw(pixel);
            }
        }


        window.display();
    }

    return 0;
}

void ProcessEvents()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        lights[2].setLightIntensity(lights[2].getLightIntensity() + 0.05f);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        lights[2].setLightIntensity(lights[2].getLightIntensity() - 0.05f);
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Add))
    {
        O.z -= 0.1f;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))
    {
        O.z += 0.1f;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        lights[1].setLightIntensity(lights[1].getLightIntensity() - 0.05f);
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        lights[1].setLightIntensity(lights[1].getLightIntensity() + 0.05f);
    }
 
}

void PutPixel(sf::RenderWindow& window, sf::RectangleShape& pixel, int x, int y, sf::Color color)
{
    float c_x = (float)canvas_width / 2 + x;
    float c_y = (float)canvas_height / 2 - y;

    pixel.setPosition(c_x, c_y);
    pixel.setFillColor(color);

    window.draw(pixel);
}

glm::vec3 CanvasToViewPort(int x, int y)
{
   float v_x = x * ((float)viewport_width / (float)canvas_width);
   float v_y = y * ((float)viewport_height / (float)canvas_height);

    return glm::vec3(v_x, v_y, d);
}

sf::Color TraceRay(glm::vec3 O, glm::vec3 D, float t_min, float t_max, int recursion_depth)
{
    float closest_t = inf;
    Sphere closest_sphere;
    closest_sphere.setNull(true);

    ClosestIntersection(O, D, t_min, t_max, closest_sphere, closest_t);

    if(closest_sphere.getNull() == true)
    {
        return sf::Color(sf::Color::White);
    }

    glm::vec3 p = O + closest_t * D;
    glm::vec3 n = p - closest_sphere.getCenter();
    n = glm::normalize(n);

    float light_component = ComputeLighting(p, n, -D, closest_sphere.getSpecular());
    sf::Color local_color = closest_sphere.getColor();
    float r = local_color.r * light_component;
    float g = local_color.g * light_component;
    float b = local_color.b * light_component;

    r > 255 ? local_color.r = 255 : local_color.r = static_cast<uint8_t>(r);
    g > 255 ? local_color.g = 255 : local_color.g = static_cast<uint8_t>(g);
    b > 255 ? local_color.b = 255 : local_color.b = static_cast<uint8_t>(b);

    float reflectiveness = closest_sphere.getReflective();
    if(recursion_depth <= 0 || reflectiveness <= 0)
    {
        return local_color;
    }

    glm::vec3 R = ReflectRay(-D, n);
    sf::Color reflected_color;
    reflected_color = TraceRay(p, R, 0.1f, inf, recursion_depth - 1);

    float r_final = local_color.r * (1 - reflectiveness) + reflected_color.r * reflectiveness;
    float g_final = local_color.g * (1 - reflectiveness) + reflected_color.g * reflectiveness;
    float b_final = local_color.b * (1 - reflectiveness) + reflected_color.b * reflectiveness;

    sf::Color final_color;

    r_final > 255 ? final_color.r = 255 : final_color.r = static_cast<uint8_t>(r_final);
    g_final > 255 ? final_color.g = 255 : final_color.g = static_cast<uint8_t>(g_final);
    b_final > 255 ? final_color.r = 255 : final_color.b = static_cast<uint8_t>(b_final);

    return final_color;
}

void IntersectRaySphere(glm::vec3 O, glm::vec3 D, Sphere& sphere, float& t1, float& t2)
{
    float r = sphere.getRadius();
    glm::vec3 CO = O - sphere.getCenter();

    float a = glm::dot(D, D);
    float b = 2 * glm::dot(CO, D);
    float c = glm::dot(CO, CO) - r * r;

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0)
    {
        t1 = inf;
        t2 = inf;
    }
    else
    {

        t1 = (-b + glm::sqrt(discriminant)) / (2 * a);
        t2 = (-b - glm::sqrt(discriminant)) / (2 * a);
    }
}

float ComputeLighting(glm::vec3 p, glm::vec3 n, glm::vec3 v, int s)
{
    float intensity = 0.0f;
    float t_max = inf;
    glm::vec3 L;
    for(Light light : lights)
    {
        if(light.getLightType() == LightType::AMBIENT)
        {
            intensity += light.getLightIntensity();
        }else
        {
            if(light.getLightType() == LightType::POINT)
            {
                L = light.getLightPosition() - p;
                t_max = 1;
            }else
            {
                L = light.getLightDirection();
                t_max = inf;
            }

            // Shadow Check
            Sphere shadow_sphere;
            shadow_sphere.setNull(true);
            float shadow_t;
            ClosestIntersection(p, L, 0.001f, t_max, shadow_sphere, shadow_t);
            if(shadow_sphere.getNull() == false)
            {
                continue;
            }

            // Diffuse
            float n_dot_l = glm::dot(n, L);
            if(n_dot_l > 0)
            {
                intensity += light.getLightIntensity() * n_dot_l / (glm::length(n) * glm::length(L));
            }

            // Specular
            if(s != -1)
            {
                glm::vec3 R = ReflectRay(L, n);
                float r_dot_v = glm::dot(R, v);
                if(r_dot_v > 0)
                {
                    intensity += light.getLightIntensity() * pow(r_dot_v / (glm::length(R)*glm::length(v)), s);
                }
            }
        }
    }

    return intensity;
}

void ClosestIntersection(glm::vec3 O, glm::vec3 D, float t_min, float t_max, Sphere& closest_sphere, float& closest_t)
{
    closest_t = inf;
    float t1 = 0.0f, t2 = 0.0f;
    for(Sphere sphere : spheres)
    {
        IntersectRaySphere(O, D, sphere, t1, t2);
       
        if(t_min < t1 && t1 < t_max && t1 < closest_t)
        {
            closest_t = t1;
            closest_sphere = sphere;
            closest_sphere.setNull(false);
        }

        if(t_min < t2 && t2 < t_max && t2 < closest_t)
        {
            closest_t = t2;
            closest_sphere = sphere;
            closest_sphere.setNull(false);
        }
    }
}

glm::vec3 ReflectRay(glm::vec3 R, glm::vec3 N)
{
    return 2.0f*N*glm::dot(N, R) - R;
}

