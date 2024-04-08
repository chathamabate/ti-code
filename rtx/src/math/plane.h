
#include "./geom.h"
#include "rtx/src/math/material.h"

namespace math {
    class Plane : public Geom {
    private:
        Vec3D norm;

    public:
        Plane(const Material *m, const Vec3D &c, 
                const Vec3D &n);

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
