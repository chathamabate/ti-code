
#pragma once

#include "./geom.h"
#include "./material.h"
#include "./plane.h"
#include "./rect_plane.h"
#include "./vec.h"

namespace math {
    class RectPrism : public Geom {
    private:
        Vec3D nx;
        Vec3D ny;
        Vec3D nz;

        RectPlane s0;   // +/-x
        RectPlane s1;

        RectPlane s2;   // +/-y
        RectPlane s3;

        RectPlane s4;   // +/-z
        RectPlane s5;

        float rad2;

    public:
        RectPrism(const Material *m, const Vec3D &c, 
            float wid, float len, float hei,
            float theta, float phi, float ro);

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
