
#include "./plane.h"
#include "rtx/src/math/material.h"
#include "rtx/src/math/misc.h"

using namespace math;

Plane::Plane(const Material *m, const Vec3D &c, const Vec3D &n) 
    : Geom(m, c), norm(n) {
    this->norm.normalize();
}

bool Plane::intersect(Ray ray, Ray *outR, float *outS) const {
    float denom = ray.getDir() * this->norm;
    if (isZero(denom)) {
        // If our ray vector is perp to the norm,
        // always report no intersection.
        
        return false;
    }

    float nume = (this->getCenter() - ray.getPoint()) * this->norm;
    if (isZero(nume)) {
        // If our ray originates on the plane,
        // always report no intersection.
        
        return false;
    }

    float s = nume / denom;

    // negative hit = no hit.
    if (s < ERR) {
        return false;
    }
    
    *outS = s;
    *outR = Ray(ray.getPoint() + (s*ray.getDir()), this->norm);

    return true;
}

