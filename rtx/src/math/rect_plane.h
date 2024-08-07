
#pragma once

#include "./geom.h"
#include "./material.h"
#include "./plane.h"
#include "./vec.h"


namespace math {
    class RectPrism;

    class RectPlane : public Plane {
        friend class RectPrism;
    
    private:
        // v1 X v2 should be in the direction of n.
        const Vec3D v1; 
        const float v1mag2;

        const Vec3D v2; 
        const float v2mag2;

        // theta and phi are the polar coordinates of the unit normal vector.
        // ro will be how much the rectanglar plane is rotated clockwise about the norm.

        inline RectPlane(const Vec3D &c, const Vec3D &n, const Vec3D &v1p, const Vec3D &v2p) 
            : Plane(c, n), v1(v1p), v1mag2(v1 * v1), v2(v2p), v2mag2(v2 * v2) {}

    public:
        RectPlane(const Vec3D &c, 
                float wid, float hei, float theta, float phi, float ro);

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
