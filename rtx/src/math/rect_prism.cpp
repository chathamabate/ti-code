
#include "./rect_prism.h"
#include "./rect_plane.h"
#include "./vec.h"

using namespace math;

RectPrism::RectPrism(const Material *m, const Vec3D &c, 
    float wid, float len, float hei,
    float theta, float phi, float ro) 
    : Geom(m, c) {
    
    Vec3D nx = Vec3D::getNorm(theta, phi);  
    nx *= (len / 2.0f);
    
    this->s0 = RectPlane(m, c + nx, wid, hei, theta, phi, ro);
    this->s1 = RectPlane(m, c - nx, wid, hei, 
            theta + M_PI, -phi, -ro);

    Vec3D ny = Vec3D::getNorm(theta + (M_PI / 2.0f), 0.0f).rotate(nx, ro);
    ny *= (wid / 2.0f);

    // This approach simply does not work...
    // NEED A better one!!
    this->s2 = RectPlane(m, c + ny, len, hei, 
            theta + (M_PI / 2.0f), ro, phi);
}

bool RectPrism::intersect(Ray ray, Ray *outR, float *outS) const {
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

    for (int i = 0; i < 3; i++) {
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
