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
        
        bool reflective;
        float reflectivity;

    public:
        static const Material DEFAULT_MAT;

        inline Material(Vec3D kap, Vec3D kdp, Vec3D ksp, uint8_t a, float r)
            : ka(kap), kd(kdp), ks(ksp), alpha(a), reflective(true), reflectivity(r) {}

        inline Material(Vec3D kap, Vec3D kdp, Vec3D ksp, uint8_t a) 
            : ka(kap), kd(kdp), ks(ksp), alpha(a), reflective(false), reflectivity(0.0f) {}

        inline Material() 
            : ka(1.0f, 0.0f, 0.0f), 
            kd(1.0f, 0.0f, 0.0f), 
            ks(1.0f, 1.0f, 1.0f), 
            alpha(6), 
            reflective(false), 
            reflectivity(0.0f) {}

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
