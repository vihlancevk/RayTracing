#include <iostream>
#include <cmath>
#include <vector>
#include <SFML/Graphics.hpp>
#include "../MathVector/CoordinateSystem.hpp"
#include "../MathVector/MathVector.hpp"
#include "Sphere.hpp"

struct Light {
    Vector position_;
    float intensity_;

    Light(const Vector& position, const float& intensity) : position_(position), intensity_(intensity) {}
};

const unsigned SCREEN_WEIGHT = 1920;
const unsigned SCREEN_HIGHT = 1080;
const char *SCREEN_TITLE = "RayTracing";
const unsigned FRAME_RATE_LIMIT = 60;

const float PI = 3.14159f;

void RayTracing(sf::VertexArray& pixels, CoordinateSystem& coordinateSystem,
                Vector& orig, std::vector<Light> lights, std::vector<Sphere> spheres);

bool sceneIntersect(const Vector& orig, const Vector& dir, const std::vector<Sphere>& spheres,
                    Vector& point, Vector& normal, Material& material);
Vector& reflect(const Vector& normal, const Vector& lightDir);
Vector RayCasting(Vector& orig, Vector& dir, std::vector<Light> lights, std::vector<Sphere> spheres, const int depth = 0);

int main()
{
	sf::RenderWindow window(sf::VideoMode(SCREEN_WEIGHT, SCREEN_HIGHT), SCREEN_TITLE);
	window.setFramerateLimit(FRAME_RATE_LIMIT);

	sf::Event event;

    CoordinateSystem coordinateSystem(SCREEN_WEIGHT, SCREEN_HIGHT,
                                      0, 0,
                                    -16, 16, -9, 9);
    
    Vector camera(0, 0, 1000);
    
    std::vector<Light> lights  = {
                                    Light(Vector(-20, 20,  20), 1.5f),
                                    Light(Vector( 30, 50, -25), 1.8f),
                                    Light(Vector( 30, 20,  30), 1.7f)
                                 };
    
    Material     ivory(Vector(0.6f, 0.3f, 0.1f), Vector(0.4f, 0.4f, 0.3f), 50.f);
    Material redRubber(Vector(0.9f, 0.1f, 0.f),  Vector(0.3f, 0.1f, 0.1f), 10.f);
    Material    mirror(Vector(0.f,  10.f, 0.8f), Vector(1.f, 1.f, 1.f),    1425.f);

    std::vector<Sphere> spheres = {
                                    Sphere(Vector(-3,   0,   -16), 2, ivory),
                                    Sphere(Vector(-1,  -1.5, -12), 2, mirror),
                                    Sphere(Vector(1.5, -0.5, -18), 3, redRubber),
                                    Sphere(Vector(7,    5,   -18), 4, mirror),
                                  };

    sf::VertexArray pixels(sf::Points, (size_t) SCREEN_WEIGHT * (size_t) SCREEN_HIGHT);

	while (window.isOpen()) {
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
                window.close();

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
                    window.close();

                if (event.key.code == sf::Keyboard::W)
                    continue;

                if (event.key.code == sf::Keyboard::S)
                    continue;
			}

            if (event.type == sf::Event::MouseButtonPressed) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    continue;
                }
            }
		}


        RayTracing(pixels, coordinateSystem, camera, lights, spheres);

        window.draw(pixels);
        window.display();
		window.clear();
	}

	return 0;
}

void RayTracing(sf::VertexArray& pixels, CoordinateSystem& coordinateSystem,
                Vector& orig, std::vector<Light> lights, std::vector<Sphere> spheres) {
    float PixelsInCoordinateX = coordinateSystem.weight_/ (coordinateSystem.xMax_ - coordinateSystem.xMin_);
    float PixelsInCoordinateY = coordinateSystem.hight_ / (coordinateSystem.yMax_ - coordinateSystem.yMin_);

    for (size_t i = 0; i < (size_t) SCREEN_HIGHT; i ++) {
        for (size_t j = 0; j < (size_t) SCREEN_WEIGHT; j ++) {
            pixels[i * (size_t) SCREEN_WEIGHT + j].position = sf::Vector2f((float) j, (float) i);

            float xLocal = ((float) j - (coordinateSystem.xLeftUp_ + coordinateSystem.weight_ / 2)) / PixelsInCoordinateX;
            float yLocal = ((coordinateSystem.yLeftUp_ + coordinateSystem.hight_  / 2) - (float) i) / PixelsInCoordinateY;
            float zLocal = 0;
            Vector point(xLocal, yLocal, zLocal);
            
            Vector dir = point - orig;
            dir.normalizeVector();

            Vector vecColor(0, 0, 0);
            vecColor += RayCasting(orig, dir, lights, spheres);

            sf::Color color(
                            (vecColor.x_ > 1.f) ? 255 : (sf::Uint8) (vecColor.x_ * 255),
                            (vecColor.y_ > 1.f) ? 255 : (sf::Uint8) (vecColor.y_ * 255),
                            (vecColor.z_ > 1.f) ? 255 : (sf::Uint8) (vecColor.z_ * 255)
                           );
            pixels[i * (size_t) SCREEN_WEIGHT + j].color = color;
        }
    }
}

bool sceneIntersect(const Vector& orig, const Vector& dir, const std::vector<Sphere>& spheres,
                    Vector& point, Vector& normal, Material& material) {
    float sceneDepth = 10e5f;
    float maxDistSphere = std::numeric_limits<float>::max();

    for (size_t i = 0; i < spheres.size(); i++) {
        float distSphere = 0;
        if (spheres[i].isRayIntersect(orig, dir, distSphere) && distSphere < maxDistSphere) {
            point = orig + dir*distSphere;
            normal = (point - spheres[i].center_).normalizeVector();
            material = spheres[i].material_;

            maxDistSphere = distSphere;
        }
    }

    return maxDistSphere < sceneDepth;
}

Vector& reflect(const Vector& lightDir, const Vector& normal) {
    return (lightDir - 2 * (normal * lightDir) * normal).normalizeVector();
}

Vector RayCasting(Vector& orig, Vector& dir, std::vector<Light> lights, std::vector<Sphere> spheres, const int depth) {
    Vector point, normal;

    Material material;
    
    if (depth > 4 || !sceneIntersect(orig, dir, spheres, point, normal, material)) {
        return Vector(0.2f, 0.7f, 0.8f);
    }

    Vector reflectDir = reflect(dir, normal);
    Vector reflectOrig = reflectDir * normal < 0.f ? point - normal * 1e-3f : point + normal * 1e-3f;
    Vector reflectColor = RayCasting(reflectOrig, reflectDir, lights, spheres, depth + 1);

    float diffuseLightIntensity = 0, specularLightIntensity = 0;
    for (size_t i = 0; i < lights.size(); i++) {
        Vector lightDir = (lights[i].position_ - point).normalizeVector();
        float lightDistance = (lights[i].position_ - point).getVectorLen();

        Vector shadowOrig = lightDir * normal < 0 ? point - normal * 1e-3f : point + normal * 1e-3f;
        Vector shadowPt, shadowNormal;
        Material tempMaterial;
        if (sceneIntersect(shadowOrig, lightDir, spheres, shadowPt, shadowNormal, tempMaterial) &&
            (shadowPt-shadowOrig).getVectorLen() < lightDistance)
            continue;

        diffuseLightIntensity += lights[i].intensity_ * std::max(0.f, lightDir * normal);
        specularLightIntensity += powf(std::max(0.f, reflect(lightDir, normal) * dir), material.specularExponent_) *
                                  lights[i].intensity_;
    }
    
    return material.diffuseColor_ * diffuseLightIntensity * material.albedo_.x_ +
           Vector(1.f, 1.f, 1.f) * specularLightIntensity * material.albedo_.y_ +
           reflectColor * material.albedo_.z_;
}
