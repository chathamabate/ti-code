
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
        Vec3D ny;
        float nyMag2;

        Vec3D nz;
        float nzMag2;

        // theta and phi are the polar coordinates of the unit normal vector.
        // ro will be how much the rectanglar plane is rotated clockwise about the norm.
        RectPlane(const Material *m, const Vec3D &c, const Vec3D &nxp, const Vec3D &nyp, const Vec3D &nzp) ;

    public:
        RectPlane();

        RectPlane(const Material *m, const Vec3D &c, 
                float wid, float hei, float theta, float phi, float ro);

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
