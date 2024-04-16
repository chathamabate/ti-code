
#include "./cylinder.h"
#include "./misc.h"
#include "./ray.h"
#include "./vec.h"
#include "./material.h"

using namespace math;

Cylinder::Cylinder() 
    : Cylinder(&Material::DEFAULT_MAT, 
            Vec3D(0.0f, 0.0f, 0.0f),
            1.0f, 0.5f, 0.0f, 0.0f) {}

Cylinder::Cylinder(const Material *m, const Vec3D &c, 
        float len, float r, float theta, float phi) 
    : Geom(m, c), 
    halflen(len / 2.0f),
    radius(r), 
    axis(Vec3D::getNorm(theta, phi) * this->halflen),
    cap1(m, c + this->axis, this->axis, r),
    cap2(m, c - this->axis, -this->axis, r) {}

bool Cylinder::intersect(Ray ray, Ray *outR, float *outS) const {
    Vec3D A = ray.getPoint(); 
    Vec3D v = ray.getDir();

    Vec3D n = this->cap1.getNorm();
    Vec3D C = this->getCenter();

    Vec3D AC = C - A;

    Vec3D d = AC - AC.proj(v);

    float rad2 = this->radius * this->radius;
    float halflen2 = this->halflen * this->halflen;

    // First check simple sphere test.
    if (d * d > rad2 + halflen2) {
        return false;
    } 

    // First check our caps.
    // NOTE: since our shape is convex, an intersection with an outwards
    // facing cap, gaurantees not closer intersection is possible!
    if (this->cap1.getNorm() * -ray.getDir() > ERR) {
        if (this->cap1.intersect(ray, outR, outS)) {
            return true;
        }  
    } else if (this->cap2.intersect(ray, outR, outS)) {
        return true;
    }

    // No, cap intersections, lets go for the casing.
    
    Vec3D Ap = A + ((AC * n) * n);
    Vec3D vp = v - ((v * n) * n);

    // Ap and vp are ray projected onto the plane composed of the
    // center of the cylinder and its axis.

    // Does Ap + vp(t) intersect with our circle?? 

    Vec3D ApC = C - Ap;
    Vec3D u = ApC - ApC.proj(vp);    // NOTE: u points into the circle.
    float uMag2 = u * u;

    if (uMag2 > rad2 + ERR) {
        return false;
    }

    Vec3D I = C - u;

    // Tangent intersection!
    if (isZero(uMag2 - rad2)) {
        Vec3D ApI = I - A;
        float s = (ApI * vp) / (vp * vp);

        // Remeber the origin of our ray does not count.
        if (s < ERR) {
            return false;
        }

        Vec3D P = A + (v * s);

        // Height check.
        if ((P - I).mag2() > halflen2 + ERR) {
            return false;
        }

        *outS = s;
        *outR = Ray(P, -u);

        return true;
    }
    
    // Only ever calculate the closest.
    // If the closest is past the length, The farther is either
    // also past length, or its obstructed by a cap.
    //
    // NOTE: We our assuming a ray NEVER originates in the Cyclinder.
    
    Vec3D w = sqrt((rad2 - uMag2) / (vp * vp)) * vp;
    Vec3D Pp = I - w;
    Vec3D ApPp = Pp - Ap;
    float s = (ApPp * vp) / (vp * vp);

    if (s < ERR) {
        return false;
    }
    
    Vec3D P = A + v * s;
    if ((Pp - P).mag2() > halflen2 + ERR) {
        return false;
    }

    *outS = s;
    *outR = Ray(P, (Pp - C));

    return true;
}
