
#pragma once

#include "./geom.h"
#include "./material.h"
#include "./plane.h"
#include "./rect_plane.h"
#include "./vec.h"

namespace math {
    class RectPrism : public Geom {
    private:
        RectPlane s0;   // +/-x
        RectPlane s1;

        RectPlane s2;   // +/-y
        RectPlane s3;

        RectPlane s4;   // +/-z
        RectPlane s5;

        float rad2;

    public:
        RectPrism(const Material *m, const Vec3D &c, 
            float len, float wid, float hei,
            float theta, float phi, float ro);

        // NOTE: Be careful with this constructor,
        // If the given vectors are not orthogonal,
        // the planes will be disconnected.
        RectPrism(const Material *m, const Vec3D &c, 
            const Vec3D &nxp, const Vec3D &nyp, const Vec3D &nzp);

        RectPrism();

        virtual inline bool selfShadowable() const override {
            return false;
        }

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
