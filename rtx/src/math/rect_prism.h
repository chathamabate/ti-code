
#pragma once

#include "./geom.h"
#include "./material.h"
#include "./plane.h"
#include "./rect_plane.h"
#include "./vec.h"

namespace math {
    class RectPrism : public Geom {
    private:
        const RectPlane s0;   // +/-x
        const RectPlane s1;

        const RectPlane s2;   // +/-y
        const RectPlane s3;

        const RectPlane s4;   // +/-z
        const RectPlane s5;

        const float rad2;

    public:
        RectPrism(const Vec3D &c, 
            float len, float wid, float hei,
            float theta, float phi, float ro);

        // NOTE: Be careful with this constructor,
        // If the given vectors are not orthogonal,
        // the planes will be disconnected.
        RectPrism(const Vec3D &c, 
            const Vec3D &nxp, const Vec3D &nyp, const Vec3D &nzp);

        virtual inline bool selfShadowable() const override {
            return false;
        }

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
