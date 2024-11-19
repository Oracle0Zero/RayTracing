#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <string>
#include "Sphere.h"
#include "Light.h"

constexpr int canvas_width = 800;
constexpr int canvas_height = 800;

constexpr int viewport_width = 1;
constexpr int viewport_height = 1;

int d = 1;

float camera_x = 0;
float camera_y = 0;
float camera_z = 0.5;
glm::vec3 O(camera_x, camera_y, camera_z);

float inf = 10000.0f;
int recursion_depth = 3;
    
std::vector<Sphere> spheres;
std::vector<Light> lights;

void PutPixel(sf::RenderWindow& window, sf::RectangleShape& pixel, int x, int y, sf::Color color);
glm::vec3 CanvasToViewPort(int x, int y);
sf::Color TraceRay(glm::vec3 O, glm::vec3 D, float t_min, float t_max);
void IntersectRaySphere(glm::vec3 camera_vector, glm::vec3 view_direction_vector, Sphere& sphere, float& t1, float& t2);

int main()
{
    sf::RenderWindow window(sf::VideoMode(canvas_width, canvas_height), "SFML works!");
    
    Sphere sphere_1;
    sphere_1.center = glm::vec3(0, -1, 3);
    sphere_1.radius = 1;
    sphere_1.color = sf::Color::Red;
    spheres.push_back(sphere_1);

    Sphere sphere_2;
    sphere_2.center = glm::vec3(2, 0, 4);
    sphere_2.radius = 1;
    sphere_2.color = sf::Color::Blue;
    spheres.push_back(sphere_2);

    Sphere sphere_3;
    sphere_3.center = glm::vec3(-2, 0, 4);
    sphere_3.radius = 1;
    sphere_3.color = sf::Color::Green;
    spheres.push_back(sphere_3);
    
    Sphere sphere_4;
    sphere_4.center = glm::vec3(0, -5001, 0);
    sphere_4.radius = 5000;
    sphere_4.color = sf::Color::Yellow;
    spheres.push_back(sphere_4);
    
    sf::RectangleShape pixel(sf::Vector2f(1, 1));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            O.z -= 0.1f;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            O.z += 0.1f;
        }

        window.clear();

        for (int i = (- canvas_width / 2); i < (canvas_width / 2); ++i)
        {
            for (int j = (- canvas_height / 2); (j < canvas_height / 2); ++j)
            {
                glm::vec3 viewport_vector = CanvasToViewPort(i, j);
                glm::vec3 D = viewport_vector - O;
                sf::Color color = TraceRay(O, D, 1.0f, inf);
                PutPixel(window, pixel, i, j, color);
            }
        }

        window.display();
    }

    return 0;
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

sf::Color TraceRay(glm::vec3 O, glm::vec3 D, float t_min, float t_max)
{
    float closest_t = inf;
    Sphere closest_sphere;
    closest_sphere.null = true;

    float t1 = 0.0f, t2 = 0.0f;
    for(Sphere sphere : spheres)
    {
        IntersectRaySphere(O, D, sphere, t1, t2);
       
     
        if(t_min < t1 && t1 < t_max && t1 < closest_t)
        {
            closest_t = t1;
            closest_sphere = sphere;
            closest_sphere.null = false;
        }

        if(t_min < t2 && t2 < t_max && t2 < closest_t)
        {
            closest_t = t2;
            closest_sphere = sphere;
            closest_sphere.null = false;
        }
    }

    if(closest_sphere.null == true)
    {
        return sf::Color(sf::Color::Black);
    }

    return closest_sphere.color;
}

void IntersectRaySphere(glm::vec3 O, glm::vec3 D, Sphere& sphere, float& t1, float& t2)
{
    float r = sphere.radius;
    glm::vec3 CO = O - sphere.center;

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

