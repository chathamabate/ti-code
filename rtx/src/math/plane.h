#pragma once

#include "./geom.h"
#include "./material.h"
#include "./vec.h"

namespace math {
    class Plane : public Geom {
    private:
        Vec3D norm;

    public:
        inline Plane() : Geom(), norm(1.0f, 0.0f, 0.0f) {}

        inline Plane(const Material *m, const Vec3D &c, 
                const Vec3D &n) : Geom(m, c), norm(n.norm()) {}

        inline Vec3D getNorm() const {
            return this->norm;
        }

        virtual inline bool selfShadowable() const override {
            return false;
        }

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
