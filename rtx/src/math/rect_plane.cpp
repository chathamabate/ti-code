#include "./rect_plane.h"
#include "rtx/src/math/material.h"
#include "rtx/src/math/plane.h"
#include "rtx/src/math/vec.h"

using namespace math;


RectPlane::RectPlane(const Material *m, const Vec3D &c, 
    float wid, float hei, float theta, float phi, float ro) 
    : Plane(m, c, Vec3D::getNorm(theta, phi)) {
    this->v1 = Vec3D(-sin(theta), cos(theta), 0);
    this->v1 = this->v1.rotate(this->getNorm(), ro);
    this->v2 = this->getNorm().cross(this->v1);

    this->v1 *= wid / 2.0f;
    this->v2 *= hei / 2.0f;
}

RectPlane::RectPlane(const Material *m, const Vec3D &c) 
    : Plane(m, c, Vec3D(1, 0, 0)), v1(0, 0.5f, 0), v2(0, 0, 0.5f) {
}

RectPlane::RectPlane() : RectPlane(Material::DEF_MATERIAL, Vec3D(0, 0, 0)) {}

bool RectPlane::intersect(Ray ray, Ray *outR, float *outS) const {
    Ray tR;
    float tS;

    if (!Plane::intersect(ray, &tR, &tS)) {
        return false;
    }
    
    // Vector from center to intersection point.
    Vec3D v = tR.getPoint() - this->getCenter();

    // Is this point within the bounds of our quad?
    float v1S = (v * this->v1) / (this->v1 * this->v1);
    if (v1S < -1.0 || 1.0 < v1S) {
        return false;
    }

    float v2S = (v * this->v2) / (this->v2 * this->v2);
    if (v2S < -1.0 || 1.0 < v2S) {
        return false;
    }

    *outR = tR;
    *outS = tS;

    return true;
}
