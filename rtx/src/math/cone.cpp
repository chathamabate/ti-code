
#include "./cone.h"
#include "./misc.h"
#include "./ray.h"
#include "./vec.h"

using namespace math;

bool Cone::intersect(Ray ray, Ray *outR, float *outS) const {
    Vec3D n = this->cap.getNorm();
    Vec3D T = this->tip;

    Vec3D v = ray.getDir();
    Vec3D A = ray.getPoint();

    float vDotn = v * n;

    // Does our ray intersect with the front side of the cap?
    if (vDotn < 0.0f &&
            this->cap.intersect(ray, outR, outS)) {
        return true;
    }

    // Does our ray originate inside the cone?? 
    // If so, NO INTERSECTION!
    Vec3D u = A - T;
    float uDotn = u * n; 
    float uDotn2 = uDotn * uDotn;
    float uMag2 = u * u;

    if (uDotn > 0.0f && uDotn2 / uMag2 >= this->cos2 - ERR) {
        return false;
    }

    // NOW... time for our quadratic!!
    float a = (this->mag2 * vDotn * vDotn) - (this->length2 * (v * v));
    float b = 2 * (
            (this->mag2 * vDotn * uDotn) -
            (this->length2 * (u * v))
    );
    float c = (this->mag2 * uDotn2) - (this->length2 * uMag2);

    float s = -1.0f;

    if (isZero(a)) {
        s = -c / b;
    } else {
        // discriminant.
        float discr = (b*b) - (4*a*c);

        if (discr < 0.0f) {
            return false;   // No real roots.
        }

        float ax2 = 2*a;

        s = -b / ax2; 

        if (!isZero(discr)) {
            float root = sqrt(discr) / ax2;
            float s1 = s + root;
            float s2 = s - root;

            s = s1;
            if (s2 > 0.0f && s2 < s1) {
                s = s2;
            }
        }
    }

    // Negative intersection = NO intersection.
    if (s < ERR) {
        return false;
    }

    // finally, is our point in bounds? And, what is our norm?
    Vec3D P = A + (v*s); 
    Vec3D w = P - T;

    float wDotn = w * n;
    
    if (wDotn < 0.0f || this->length < wDotn) {
        return false;   // Out of bounds!
    }

    *outS = s;
    *outR = Ray(P, w.cross(w.cross(n)));

    return true;
}
