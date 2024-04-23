
#pragma once

#include "./geom.h"
#include "./material.h"
#include "./vec.h"
#include "./disk.h"

namespace math {
    class Cone : public Geom {
    private:
        const float length;
        const float length2;

        const float mag2;
        const float cos2;

        const Disk cap;     // Normal of the cap is the axis of 
                            // the cone.

        const Vec3D tip;    // The tip of the cone.

    public:
        inline Cone(const Material *m, const Vec3D &c,
                const Vec3D &n, float l, float r) 
            : Geom(m, c),
            length(l),
            length2(this->length * this->length),
            mag2(r*r + this->length2),
            cos2(this->length2 / this->mag2),
            cap(m, c + (n.norm() * (this->length / 2.0f)), n.norm(), r),
            tip(this->cap.getCenter() - (this->length * this->cap.getNorm())) {}

        virtual inline bool selfShadowable() const override {
            return false;
        }

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
