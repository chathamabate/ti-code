#pragma once

#include "./geom.h"
#include "./material.h"
#include "./vec.h"

namespace math {
    class Plane : public Geom {
    private:
        Vec3D norm;

    public:
        Plane(const Material *m, const Vec3D &c, 
                const Vec3D &n);

        inline Plane(const Plane &p) 
            : Plane(p.getMat(), p.getCenter(), p.norm) {}

        inline Vec3D getNorm() const {
            return this->norm;
        }

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
