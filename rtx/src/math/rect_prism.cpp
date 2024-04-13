
#include "./rect_prism.h"
#include "./rect_plane.h"
#include "./vec.h"

using namespace math;

RectPrism::RectPrism(const Material *m, const Vec3D &c, 
    float len, float wid, float hei,
    float theta, float phi, float ro) 
    : RectPrism(m, c,
            Vec3D::getNorm(theta, phi) * (len / 2.0f),
            Vec3D::getNorm(theta + (M_PI / 2.0f), 0.0f).rotate(nx, ro) * (wid / 2.0f),
            Vec3D::getNorm(theta, phi + (M_PI / 2.0f)).rotate(nx, ro) * (hei / 2.0f)
    ) {
}

RectPrism::RectPrism(const Material *m, const Vec3D &c, 
    const Vec3D &nxp, const Vec3D &nyp, const Vec3D &nzp) 
    : Geom(m, c),
    nx(nxp), ny(nyp), nz(nzp), 
    s0(m, c + nx, nx, ny, nz),
    s1(m, c - nx, -nx, ny, -nz),
    s2(m, c + ny, ny, -nx, nz),
    s3(m, c - ny, -ny, -nx, -nz),
    s4(m, c + nz, nz, ny, -nx),
    s5(m, c - nz, -nz, ny, nx),
    rad2((nx + ny + nz).mag2()) {
}

bool RectPrism::intersect(Ray ray, Ray *outR, float *outS) const {
    // First we determine if our ray even comes close enough
    // to our rectangular prism to intersect...
    
    Vec3D o2c = this->getCenter() - ray.getPoint();
    Vec3D perp = o2c - o2c.proj(ray.getDir());
    float dist2 = perp * perp;

    if (dist2 > rad2 + ERR) {
        return false;
    }

    // NOTE: if we see s0, it's impossible we see s1.
    // if we see s1, its impossible we see s0... 
    //
    // Same condition holds for (s2, s3) and (s4, s5)

    const RectPlane *sides[6] = {
        &(this->s0),
        &(this->s1),
        &(this->s2),
        &(this->s3),
        &(this->s4),
        &(this->s5)
    };

    bool hit = false;
    Ray r;
    float s;    

    for (size_t i = 0; i < 6; i++) {
        // Because a rectangular prism is convex, 
        // The back of a face will always be obstructed by another face.
        //
        // NOTE: THIS ASSUME OUR RAY DOES NOT ORIGINATE IN 
        // THE PRISM!
        if (sides[i]->getNorm() * (-ray.getDir()) < ERR) {
            continue;
        }

        Ray tr;
        float ts;    

        if (sides[i]->intersect(ray, &tr, &ts) && (!hit || ts < s)) {
            hit = true;

            s = ts;
            r = tr;
        }
    }

    if (hit) {
        *outR = r;
        *outS = s;

        return true;
    }

    return false;
}
