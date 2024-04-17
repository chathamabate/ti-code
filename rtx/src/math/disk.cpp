
#include "./disk.h"
#include "rtx/src/math/vec.h"

using namespace math;

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
