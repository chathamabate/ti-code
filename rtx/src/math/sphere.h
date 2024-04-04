
#include "./geom.h"

namespace math {
    class Sphere : public Geom {
    private:
        float radius; 

    public:
        inline Sphere(const Material *m, const Vec3D &c, float r) 
            : Geom(m, c), radius(r) {}

        inline float getRadius() const {
            return this->radius;
        }

        inline void setRadius(float r) {
            this->radius = r;
        }

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
