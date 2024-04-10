
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
        Vec3D v1; 
        Vec3D v2; 

        // theta and phi are the polar coordinates of the unit normal vector.
        // ro will be how much the rectanglar plane is rotated clockwise about the norm.

        inline RectPlane(const Material *m, const Vec3D &c, const Vec3D &n, const Vec3D &v1p, const Vec3D &v2p) 
            : Plane(m, c, n), v1(v1p), v2(v2p) {}

    public:
        RectPlane(const Material *m, const Vec3D &c, 
                float wid, float hei, float theta, float phi, float ro);

        RectPlane(const Material *m, const Vec3D &c);

        RectPlane();

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
