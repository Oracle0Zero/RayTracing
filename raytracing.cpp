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

int main()
{
    sf::RenderWindow window(sf::VideoMode(canvas_width, canvas_height), "SFML works!");
    
    Sphere sphere_1;
    sphere_1.center = glm::vec3(0, -1, 3);
    sphere_1.radius = 1;
    sphere_1.color = sf::Color::Red;
    sphere_1.specular = 500;
    sphere_1.reflective = 0.2f;
    spheres.push_back(sphere_1);

    Sphere sphere_2;
    sphere_2.center = glm::vec3(2, 0, 4);
    sphere_2.radius = 1;
    sphere_2.color = sf::Color::Blue;
    sphere_2.specular = 500;
    sphere_2.reflective = 0.3f;
    spheres.push_back(sphere_2);

    Sphere sphere_3;
    sphere_3.center = glm::vec3(-2, 0, 4);
    sphere_3.radius = 1;
    sphere_3.color = sf::Color::Green;
    sphere_3.specular = 10;
    sphere_3.reflective = 0.4f;
    spheres.push_back(sphere_3);
    
    Sphere sphere_4;
    sphere_4.center = glm::vec3(0, -5001, 0);
    sphere_4.radius = 5000;
    sphere_4.color = sf::Color::Yellow;
    sphere_4.specular = 1000;
    sphere_4.reflective = 0.5f;
    spheres.push_back(sphere_4);

    Light ambient;
    ambient.type = LightType::AMBIENT;
    ambient.intensity = 0.2f;
    lights.push_back(ambient);

    Light point;
    point.type = LightType::POINT;
    point.intensity = 0.6f;
    point.position = glm::vec3(2, 1, 0);
    lights.push_back(point);

    Light directional;
    directional.type = LightType::DIRECTIONAL;
    directional.intensity = 0.2f;
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
    //printf("%f\n", closest_t);
    ClosestIntersection(O, D, t_min, t_max, closest_sphere, closest_t);
    /*
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
    */

    if(closest_sphere.null == true)
    {
        return sf::Color(sf::Color::Black);
    }

    glm::vec3 p = O + closest_t * D;
    glm::vec3 n = p - closest_sphere.center;
    n = glm::normalize(n);

    float light_component = ComputeLighting(p, n, -D, closest_sphere.specular);
    sf::Color local_color = closest_sphere.color;
    float r = local_color.r * light_component;
    float g = local_color.g * light_component;
    float b = local_color.b * light_component;

    if(r > 255){local_color.r = 255;} else {local_color.r = static_cast<uint8_t>(r);}
    if(g > 255){local_color.g = 255;} else {local_color.g = static_cast<uint8_t>(g);}
    if(b > 255){local_color.b = 255;} else {local_color.b = static_cast<uint8_t>(b);}

    float reflectiveness = closest_sphere.reflective;
    if(recursion_depth <= 0 || reflectiveness <= 0)
    {
        //if(r > 255){local_color.r = 255;} else {local_color.r = static_cast<uint8_t>(r);}
        //if(g > 255){local_color.g = 255;} else {local_color.g = static_cast<uint8_t>(g);}
        //if(b > 255){local_color.b = 255;} else {local_color.b = static_cast<uint8_t>(b);}

        return local_color;
    }

    glm::vec3 R = ReflectRay(-D, n);
    sf::Color reflected_color;
    reflected_color = TraceRay(p, R, 0.1f, inf, recursion_depth - 1);
    //if(r > 255){color.r = 255;} else {color.r = static_cast<uint8_t>(r);}
    //if(g > 255){color.g = 255;} else {color.g = static_cast<uint8_t>(g);}
    //if(b > 255){color.b = 255;} else {color.b = static_cast<uint8_t>(b);}
    float r_final = local_color.r * (1 - reflectiveness) + reflected_color.r * reflectiveness;
    float g_final = local_color.g * (1 - reflectiveness) + reflected_color.g * reflectiveness;
    float b_final = local_color.b * (1 - reflectiveness) + reflected_color.b * reflectiveness;

    sf::Color final_color;
    if(r_final > 255){final_color.r = 255;} else {final_color.r = static_cast<uint8_t>(r_final);}
    if(g_final > 255){final_color.g = 255;} else {final_color.g = static_cast<uint8_t>(g_final);}
    if(b_final > 255){final_color.b = 255;} else {final_color.b = static_cast<uint8_t>(b_final);}

    return final_color;
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

float ComputeLighting(glm::vec3 p, glm::vec3 n, glm::vec3 v, int s)
{
    float intensity = 0.0f;
    float t_max = inf;
    glm::vec3 L;
    for(Light light : lights)
    {
        if(light.type == LightType::AMBIENT)
        {
            intensity += light.intensity;
        }else
        {
            if(light.type == LightType::POINT)
            {
                L = light.position - p;
                t_max = 1;
            }else
            {
                L = light.direction;
                t_max = inf;
            }

            // Shadow Check
            Sphere shadow_sphere;
            shadow_sphere.null = true;
            float shadow_t;
            ClosestIntersection(p, L, 0.001f, t_max, shadow_sphere, shadow_t);
            if(shadow_sphere.null == false)
            {
                continue;
            }

            // Diffuse
            float n_dot_l = glm::dot(n, L);
            if(n_dot_l > 0)
            {
                intensity += light.intensity * n_dot_l / (glm::length(n) * glm::length(L));
            }

            // Specular
            if(s != -1)
            {
                //glm::vec3 R = 2.0f*n*glm::dot(n, L) - L;
                glm::vec3 R = ReflectRay(L, n);
                float r_dot_v = glm::dot(R, v);
                if(r_dot_v > 0)
                {
                    intensity += light.intensity * pow(r_dot_v / (glm::length(R)*glm::length(v)), s);
                }
            }
        }
    }

    return intensity;
}

void ClosestIntersection(glm::vec3 O, glm::vec3 D, float t_min, float t_max, Sphere& closest_sphere, float& closest_t)
{
    closest_t = inf;
    //printf("%f\n", closest_t);
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
}

glm::vec3 ReflectRay(glm::vec3 R, glm::vec3 N)
{
    return 2.0f*N*glm::dot(N, R) - R;
}

