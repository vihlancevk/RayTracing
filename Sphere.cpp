#include <cmath>
#include "Sphere.hpp"

bool Sphere::isRayIntersect(const Vector& orig, const Vector& dir, float& distSphere) const {
    float a = dir * dir;
    float b = 2 * ((orig - center_) * dir);
    float c = (orig - center_) * (orig - center_) - radius_ * radius_;
    
    float D = b * b - 4 * a * c;
    
    if (D > 0) {
        float t1 = (-b + sqrtf(D)) / (2 * a);
        float t2 = (-b - sqrtf(D)) / (2 * a);

        if (t1 > 0 && t2 > 0) {
            distSphere = std::min(t1, t2);
        
            return true;
        }
    }

    return false;
}
