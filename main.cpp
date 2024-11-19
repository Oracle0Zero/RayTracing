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

int camera_x = 0;
int camera_y = 0;
int camerz_z = 0;
glm::vec3 O(camera_x, camera_y, camerz_z);

float inf = 10000000.0f;
int recursion_depth = 3;
    
std::vector<Sphere> spheres;
std::vector<Light> lights;

void PutPixel(sf::RenderWindow& window, sf::RectangleShape& pixel, int x, int y, sf::Color color);
glm::vec3 CanvasToViewPort(int x, int y);
sf::Color TraceRay(glm::vec3 O, glm::vec3 D, float t_min, float t_max, int recursion_depth);
void IntersectRaySphere(glm::vec3 camera_vector, glm::vec3 view_direction_vector, Sphere& sphere, float& t1, float& t2);
float ComputeLighting(glm::vec3 point, glm::vec3 normal, glm::vec3 view_direction_vector, int shininess);
void ClosestIntersection(glm::vec3 camera_vector, glm::vec3 view_direction_vector, float t_min, float t_max, Sphere& closest_sphere, float& closest_t);
glm::vec3 ReflectRay(glm::vec3 ray, glm::vec3 normal);

int main()
{
    sf::RenderWindow window(sf::VideoMode(canvas_width, canvas_height), "SFML works!");
    
    Sphere sphere_1;
    sphere_1.location = glm::vec3(0, -1, 3);
    sphere_1.radius = 1;
    sphere_1.color = sf::Color::Red;
    sphere_1.name = "Red";
    sphere_1.specular = 500;
    sphere_1.reflective = 0.2f;
    spheres.push_back(sphere_1);

    Sphere sphere_2;
    sphere_2.location = glm::vec3(2, 0, 4);
    sphere_2.radius = 1;
    sphere_2.color = sf::Color::Blue;
    sphere_2.name = "Blue";
    sphere_2.specular = 500;
    sphere_2.reflective = 0.3f;
    spheres.push_back(sphere_2);

    Sphere sphere_3;
    sphere_3.location = glm::vec3(-2, 0, 4);
    sphere_3.radius = 1;
    sphere_3.color = sf::Color::Green;
    sphere_3.name = "Green";
    sphere_3.specular = 10;
    sphere_3.reflective = 0.4f;
    spheres.push_back(sphere_3);
    
    Sphere sphere_4;
    sphere_4.location = glm::vec3(0, -5001, 0);
    sphere_4.radius = 5000;
    sphere_4.color = sf::Color::Yellow;
    sphere_4.name = "Yellow";
    sphere_4.specular = 1000;
    sphere_4.reflective = 0.5f;
    spheres.push_back(sphere_4);

    Light ambient;
    ambient.type = LightType::AMBIENT;
    ambient.intensity = 0.2;
    lights.push_back(ambient);

    Light point;
    point.type = LightType::POINT;
    point.intensity = 0.6;
    point.position = glm::vec3(2, 1, 0);
    lights.push_back(point);

    Light directional;
    directional.type = LightType::DIRECTIONAL;
    directional.intensity = 0.2;
    directional.direction = glm::vec3(1, 4, 4);
    lights.push_back(directional);
    

    sf::RectangleShape pixel(sf::Vector2f(1, 1));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        for (int i = (- canvas_width / 2); i < (canvas_width / 2); ++i)
        {
            for (int j = (- canvas_height / 2); (j < canvas_height / 2); ++j)
            {
                glm::vec3 viewport_vector = CanvasToViewPort(i, j);
                glm::vec3 D = viewport_vector - O;
                sf::Color color = TraceRay(O, D, 1.0f, inf, recursion_depth);
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
    
    //sf::RectangleShape pixel(sf::Vector2f(1, 1));
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
    closest_sphere.null = true;

    float t1, t2;
 
    ClosestIntersection(O, D, t_min, t_max, closest_sphere, closest_t);

    if(!closest_sphere.null)
    {
        return sf::Color::Black;
    }

    //return closest_sphere.color;
    glm::vec3 point = O + closest_t * D;
    glm::vec3 normal = point - closest_sphere.center;
    normal = glm::normalize(normal);

    sf::Color final_color = closest_sphere.color;
    float light_intensity = ComputeLighting(point, normal, -D, closest_sphere.specular);


    float red = final_color.r * light_intensity;
    if(red > 255)
    {
        final_color.r = 255;
    }else
    {
        
        final_color.r = static_cast<uint8_t>(red);
        /*
        if(closest_sphere.name == "Yellow")
        {
            printf("red: %f\n", red);
            printf("final_color.r: %d\n", final_color.r);
        }
        */

    }
    
    float green = final_color.g * light_intensity;
    if(green > 255)
    {
        final_color.g = 255;
    }else
    {
        final_color.g = static_cast<uint8_t>(green);
    }
    
    float blue = final_color.b * light_intensity;
    if(blue > 255)
    {
        final_color.b = 255;
    }else
    {
        final_color.b = static_cast<uint8_t>(blue);
    }
    // ---------------------------------
    float r = closest_sphere.reflective;
    
    if(recursion_depth <= 0 || r <= 0)
    {
        return final_color;
    }
    
    glm::vec3 R = ReflectRay(-D, normal);
    sf::Color reflected_color = TraceRay(point, R, 0.001f, inf, recursion_depth - 1);

    red = final_color.r * (1 - r) + reflected_color.r * r;
    green = final_color.g * (1 - r) + reflected_color.g * r;
    blue = final_color.b * (1 - r) + reflected_color.b * r;
    
    if(red > 255)
    {
        final_color.r = 255;
    }else
    {
        final_color.r = static_cast<uint8_t>(red);
    }
    if(green > 255)
    {
        final_color.g = 255;
    }else
    {
        final_color.g = static_cast<uint8_t>(green);
    }
    if(blue > 255)
    {
        final_color.b = 255;
    }else
    {
        final_color.b = static_cast<uint8_t>(blue);
    }
    
    return final_color;
}

void IntersectRaySphere(glm::vec3 O, glm::vec3 D, Sphere& sphere, float& t1, float& t2)
{
    //std::cout << sphere.name << "\n";
    float r = sphere.radius;

    glm::vec3 CO = O - sphere.center;

    //glm::vec3 glm_viewport_vector = glm::vec3(view_direction_vector.x, view_direction_vector.y, view_direction_vector.z);

    float a = glm::dot(O, D);
    float b = 2 * glm::dot(CO, D);
    float c = glm::dot(CO, CO) - r * r;

    float discriminant = b * b - 4 * a * c;

    //std::cout << sphere.name << " " << "discriminant: " << discriminant << "\n";

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

float ComputeLighting(glm::vec3 point, glm::vec3 normal, glm::vec3 view_direction_vector, int shininess)
{
    float intensity = 0;
    float t_max = inf;
    for(Light& light : lights)
    {
        if(light.type == LightType::AMBIENT)
        {
            intensity += light.intensity;
        }else
        {
            glm::vec3 l;
            if(light.type == LightType::POINT)
            {
                l = light.position - point;
                t_max = 1;
            }else
            {
                l = light.direction;
                t_max = inf;
            }

            // Shadow Check
            Sphere shadow_sphere;
            float shadow_t;
            ClosestIntersection(point, l, 0.001f, t_max, shadow_sphere, shadow_t);
            if(shadow_sphere.null)
            {
                continue;
            }

            // Diffuse
            float n_dot_l = glm::dot(normal, l);
            if(n_dot_l > 0)
            {
                intensity += light.intensity * (n_dot_l / (glm::length(normal) * glm::length(l)));
            }

            // Specular
            if(shininess != -1)
            {
                glm::vec3 reflection_vector = ReflectRay(l, normal);
                float r_dot_v = glm::dot(reflection_vector, view_direction_vector);
                
                if(r_dot_v > 0)
                {
                    intensity = intensity + light.intensity * pow((r_dot_v / (glm::length(reflection_vector) * glm::length(view_direction_vector))), shininess);
  
                }
            }

        }
    }

    return intensity;
}

void ClosestIntersection(glm::vec3 O, glm::vec3 D, float t_min, float t_max, Sphere& closest_sphere, float& closest_t)
{
    closest_t = inf;
    float t1, t2;
    for(Sphere sphere : spheres)
    {
        IntersectRaySphere(O, D, sphere, t1, t2);
        if (t_min < t1 && t1 < t_max && t1 < closest_t)
        {
            closest_t = t1;
            closest_sphere = sphere;
            closest_sphere.null = true;
        }
        if (t_min < t2 && t2 < t_max && t2 < closest_t)
        {
            closest_t = t2;
            closest_sphere = sphere;
            closest_sphere.null = true;
        }
    }
}

glm::vec3 ReflectRay(glm::vec3 ray, glm::vec3 normal)
{
    glm::vec3 reflected_ray = (2.0f * normal * glm::dot(normal, ray)) - ray;
    return reflected_ray;
}
