#include "./rect_plane.h"
#include "./material.h"
#include "./plane.h"
#include "./vec.h"

using namespace math;


RectPlane::RectPlane(const Vec3D &c, 
    float wid, float hei, float theta, float phi, float ro) 
    : RectPlane(c, 
            Vec3D::getNorm(theta, phi),
            Vec3D::getNorm(theta + (M_PI / 2.0f), 0.0f).rotate(this->getNorm(), ro) * (wid / 2.0f),
            Vec3D::getNorm(theta, phi + (M_PI / 2.0f)).rotate(this->getNorm(), ro) * (hei / 2.0f)
    )  {
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
    float vv1 = v * v1;
    if (vv1 < -this->v1mag2 || this->v1mag2 < vv1) {
        return false;
    }

    float vv2 = v * v2;
    if (vv2 < -this->v2mag2 || this->v2mag2 < vv2) {
        return false;
    }

    *outR = tR;
    *outS = tS;

    return true;
}
