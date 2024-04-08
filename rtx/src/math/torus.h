
#include "./geom.h"
#include "rtx/src/math/material.h"

namespace math {
    class Torus : public Geom {
    private:
        Vec3D mainRadius;
        float tubeRadius;

    public:
        inline Torus(const Material *m, const Vec3D &c, 
                const Vec3D &mr, float tr) 
            : Geom(m, c), mainRadius(mr), tubeRadius(tr) {}

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
