
#pragma once

#include "./geom.h"
#include "./material.h"
#include "./plane.h"
#include "./rect_plane.h"
#include "./vec.h"

namespace math {
    class RectPrism : public Geom {
    private:
        const Vec3D nx;
        const Vec3D ny;
        const Vec3D nz;

        const RectPlane s0;   // +/-x
        const RectPlane s1;

        const RectPlane s2;   // +/-y
        const RectPlane s3;

        const RectPlane s4;   // +/-z
        const RectPlane s5;

        const float rad2;

    public:
        RectPrism(const Material *m, const Vec3D &c, 
            float len, float wid, float hei,
            float theta, float phi, float ro);

        virtual inline bool selfShadowable() const override {
            return false;
        }

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
