#include "./rect_plane.h"
#include "./material.h"
#include "./plane.h"
#include "./vec.h"

using namespace math;

RectPlane::RectPlane(const Material *m, const Vec3D &c, const Vec3D &nxp, const Vec3D &nyp, const Vec3D &nzp) 
    : Plane(m, c, nxp), ny(nyp), nyMag2(nyp * nyp), nz(nzp), nzMag2(nzp * nzp) {}

RectPlane::RectPlane() 
    : Plane(), 
    ny(0.0f, 1.0f, 0.0f), nyMag2(1.0f), 
    nz(0.0f, 0.0f, 1.0f), nzMag2(1.0f) {}

RectPlane::RectPlane(const Material *m, const Vec3D &c, 
    float wid, float hei, float theta, float phi, float ro) 
    : RectPlane(m, c, 
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
    float vny = v * this->ny;
    if (vny < -this->nyMag2 || this->nyMag2 < vny) {
        return false;
    }

    float vnz = v * this->nz;
    if (vnz < -this->nzMag2 || this->nzMag2 < vnz) {
        return false;
    }

    *outR = tR;
    *outS = tS;

    return true;
}
