#pragma once

#include "./geom.h"

namespace math {
    class Sphere : public Geom {
    private:
        const float radius; 

    public:
        inline Sphere(const Vec3D &c, float r) 
            : Geom(c), radius(r) {}

        inline float getRadius() const {
            return this->radius;
        }

        virtual inline bool selfShadowable() const override {
            return false;
        }

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
