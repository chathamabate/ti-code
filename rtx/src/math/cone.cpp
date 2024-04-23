
#include "./cone.h"
#include "./misc.h"
#include "./ray.h"
#include "./vec.h"

using namespace math;

bool Cone::intersect(Ray ray, Ray *outR, float *outS) const {
    Vec3D v = ray.getDir();
    Vec3D A = ray.getPoint();

    Vec3D C = this->getCenter();
    Vec3D AC = C - A;

    Vec3D d = AC - AC.proj(v);
    if (d * d > this->rad2) {
        return false;
    }

    Vec3D n = this->cap.getNorm();
    Vec3D T = this->tip;

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

    float s1 = -1.0f;
    float s2 = -1.0f;

    if (isZero(a)) {
        s2 = -c / b;
    } else {
        // discriminant.
        float discr = (b*b) - (4*a*c);

        if (discr < 0.0f) {
            return false;   // No real roots.
        }

        if (isZero(discr)) {
            s2 = -b / (2*a);
        } else {
            float ax2 = 2*a;

            float sp = -b / ax2;
            float root = sqrt(discr) / ax2;

            s1 = sp - root;
            s2 = sp + root;
        }
    }

    // At this point, s1 and s2 represent the 2 possible intersection
    // points on the cone. (If s1 or s2 is negative, its value should not be interpreted)

    // We must determine the closest point with a positive s value on the 
    // visible cone. 

    // Make sure s1 has the smallest value.
    if (s1 > s2) {
        float temp = s1;
        s1 = s2;
        s2 = temp;
    }

    if (s1 > 0.0f) {
        Vec3D P1 = A + (s1*v);
        Vec3D w1 = P1 - T;
        float w1Dotn = w1 * n;
        if (0.0f < w1Dotn && w1Dotn < this->length) {
            *outS = s1;
            *outR = Ray(P1, w1.cross(w1.cross(n)));
            return true;

            // NOTE: even if s2 hits the visible cone, since s1 < s2,
            // we are safe to return true here.
        } 
    }

    if (s2 > 0.0f) {
        Vec3D P2 = A + (s2*v);
        Vec3D w2 = P2 - T;
        float w2Dotn = w2 * n;
        if (0.0f < w2Dotn && w2Dotn < this->length) {
            *outS = s2;
            *outR = Ray(P2, w2.cross(w2.cross(n)));
            return true;
        } 
    }

    return false;
}
