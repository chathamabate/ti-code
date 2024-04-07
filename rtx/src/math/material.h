#pragma once

#include "./ray.h"
#include "./vec.h"
#include <stdint.h>

namespace math {
    class Material {
    private:

        // These values should all be between 0 and 1.
        Vec3D ka;   // ambient.
        Vec3D kd;   // diffuse.
        Vec3D ks;   // specular
        uint8_t alpha;  // specular exponent.

    public:
        inline Material(Vec3D kap, Vec3D kdp, Vec3D ksp, uint8_t a) 
            : ka(kap), kd(kdp), ks(ksp), alpha(a) {}

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
    };
}
