#include "./rect_plane.h"
#include "./material.h"
#include "./plane.h"
#include "./vec.h"

using namespace math;


RectPlane::RectPlane(const Material *m, const Vec3D &c, 
    float wid, float hei, float theta, float phi, float ro) 
    : Plane(m, c, Vec3D::getNorm(theta, phi)), 
    v1(Vec3D::getNorm(theta + (M_PI / 2.0f), 0.0f).rotate(this->getNorm(), ro) * (wid / 2.0f)),
    v2(Vec3D::getNorm(theta, phi + (M_PI / 2.0f)).rotate(this->getNorm(), ro) * (hei / 2.0f)) {
}

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
