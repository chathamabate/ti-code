
#pragma once

#include "./geom.h"
#include "./material.h"
#include "./vec.h"
#include "./disk.h"

namespace math {
    class Cylinder : public Geom {
    private: 
        float halflen;
        float radius;
        Vec3D axis;

        Disk cap1;
        Disk cap2;

    public:
        Cylinder();

        Cylinder(const Material *m, const Vec3D &c, 
                float len, float r, float theta, float phi);

        virtual inline bool selfShadowable() const override {
            return false;
        }

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
