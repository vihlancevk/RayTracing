#ifndef SPHERE_HPP_
#define SPHERE_HPP_

#include "../Vectors/Vector.hpp"

struct Material {
    Vector albedo_;
    Vector diffuseColor_;
    float specularExponent_;

    Material():
        albedo_(Vector(0, 0, 0)),
        diffuseColor_(Vector(0, 0, 0)),
        specularExponent_(0.f)
        {}
    Material(const Vector& albedo, const Vector& diffuseColor, const float specularExponent): 
        albedo_(albedo),
        diffuseColor_(diffuseColor),
        specularExponent_(specularExponent)
        {}
};

class Sphere {
    public:
        Vector center_;
        float radius_;

        Material material_;
    public:
        Sphere(const Vector& center, const float& radius, const Material& material) : center_(center),
                                                                                      radius_(radius),
                                                                                      material_(material) {}
        ~Sphere() {}

        bool isRayIntersect(const Vector& orig, const Vector& dir, float& distSphere) const;
};

#endif // SPHERE_HPP_
