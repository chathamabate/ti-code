
#include "./sphere.h"
#include "./misc.h"
#include "rtx/src/math/ray.h"
#include "rtx/src/math/vec.h"
#include <math.h>

using namespace math;

bool Sphere::intersect(Ray ray, Ray *outR, float *outS) const {
    // hmmmmmmmmm

    Vec3D c = this->getCenter();    // c = center of circle.

    Vec3D a = ray.getPoint();       // a = where ray is casted from.
    Vec3D d = ray.getDir();         // d = ray's direction.

    Vec3D ac = c - a;

    float dMagSq = d*d;

    // point b will be a + (ac proj onto d)
    // a + d*bs = b (aka parameter value to go from a to b using d)
    float bs = (ac*d) / (dMagSq);
    Vec3D b = a + (bs*d);

    Vec3D cb = b - c;

    float cbMagSq = cb * cb;
    float rSq = this->radius * this->radius;

    if (cbMagSq > (rSq + ERR)) {
        return false;   // NO INTERSECTION.
    }

    // tangent intersection!
    if (isZero(cbMagSq - rSq)) {
        if (isZero(bs)) {
            return false;
        }
        
        *outR = Ray(b, cb);
        *outS = bs;
    } 

    // Dual intersection case!
    //
    // We want the closest intersection point (in the positive direction), 
    // which itself is not a.
    
    float ks = sqrt((rSq - cbMagSq) / dMagSq);
    Vec3D kSeg = ks * d;

    // If k1 is infront of a, it must be the closest.
    Vec3D k1 = b - kSeg;
    Vec3D ak1 = k1 - a;
    float k1s = (ak1 * d) / dMagSq;

    if (k1s > ERR) {
        // We have a hit!

        *outR = Ray(k1, k1 - c);
        *outS = k1s;
        
        return true;
    }

    // Otherwise, let's try for the second point.
    Vec3D k2 = b + kSeg;
    Vec3D ak2 = k2 - a;
    float k2s = (ak2 * d) / dMagSq;

    if (k2s > ERR) {
        *outR = Ray(k2, k2 - c);
        *outS = k2s;

        return true;
    }
     
    return false;
}
