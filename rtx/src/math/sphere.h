
#include "./geom.h"

namespace math {
    class Sphere : public Geom {
    private:
        float radius; 

    public:
        inline Sphere() 
            : Geom(), radius(1.0f) {}

        inline Sphere(const Material *m, const Vec3D &c, float r) 
            : Geom(m, c), radius(r) {}

        inline float getRadius() const {
            return this->radius;
        }

        virtual inline bool selfShadowable() const override {
            return false;
        }

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
