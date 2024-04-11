#pragma once

#include "./ray.h"
#include "./vec.h"
#include <stdint.h>

namespace math {
    class Material {
    private:

        // These values should all be between 0 and 1.
        const Vec3D ka;   // ambient.
        const Vec3D kd;   // diffuse.
        const Vec3D ks;   // specular
        const uint8_t alpha;  // specular exponent.
        
        const bool reflective;
        const float reflectivity;

    public:
        inline Material(Vec3D kap, Vec3D kdp, Vec3D ksp, uint8_t a, float r)
            : ka(kap), kd(kdp), ks(ksp), alpha(a), reflective(true), reflectivity(r) {}

        inline Material(Vec3D kap, Vec3D kdp, Vec3D ksp, uint8_t a) 
            : ka(kap), kd(kdp), ks(ksp), alpha(a), reflective(false), reflectivity(0.0f) {}

        inline const Vec3D &getKa() const {
            return this->ka;
        }

        inline const Vec3D &getKd() const {
            return this->kd;
        }

        inline const Vec3D &getKs() const {
            return this->ks;
        }

        inline float getAlpha() const {
            return this->alpha;
        }

        inline bool isReflective() const {
            return this->reflective;
        }

        inline float getReflectivity() const {
            return this->reflectivity;
        }
    };
}
