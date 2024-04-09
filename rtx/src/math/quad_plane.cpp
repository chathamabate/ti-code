#include "./quad_plane.h"
#include "rtx/src/math/vec.h"

using namespace math;

QuadPlane::QuadPlane(const Material *m, const Vec3D &tl, 
        const Vec3D &tr, const Vec3D &bl) 
    : Plane(m, 0.5f * (tr + bl), (bl-tl).cross(tr-tl)),
        right(0.5f * (tr-tl)), up(-0.5f * (bl-tl)) {
    this->getCenter().display();
    right.display();
    up.display();
}

bool QuadPlane::intersect(Ray ray, Ray *outR, float *outS) const {
    Ray tR;
    float tS;

    if (!Plane::intersect(ray, &tR, &tS)) {
        return false;
    }
    
    // Vector from center to intersection point.
    Vec3D v = tR.getPoint() - this->getCenter();

    // Is this point within the bounds of our quad?
    float rightS = (v * this->right) / (this->right * this->right);
    if (rightS < -1.0 || 1.0 < rightS) {
        return false;
    }

    float upS = (v * this->up) / (this->up * this->up);
    if (upS < -1.0 || 1.0 < upS) {
        return false;
    }

    *outR = tR;
    *outS = tS;

    return true;
}
