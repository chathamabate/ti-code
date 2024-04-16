
#include "./disk.h"
#include "./vec.h"

using namespace math;
Disk::Disk(const Material *m, const Vec3D &c,
    float r, float theta, float phi) 
    : Plane(m, c, Vec3D::getNorm(theta, phi)), radius(r) {}

bool Disk::intersect(Ray ray, Ray *outR, float *outS) const {
    Ray tr;
    float ts;

    if (!Plane::intersect(ray, &tr, &ts)) {
        return false;
    }

    Vec3D v = tr.getPoint() - this->getCenter(); 
    
    if ((v*v) > this->radius * this->radius) {
        return false;
    }

    *outR = tr;
    *outS = ts;

    return true;
};
