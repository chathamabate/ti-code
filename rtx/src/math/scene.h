
#pragma once

#include "./geom.h"
#include "./vec.h"

namespace math {
    class Perspective {
    private:
        // eye point.

        Vec3D nx;
        Vec3D ny;
        Vec3D nz;

        // origin will be the center of the viewing plane.
        Vec3D origin;

        // origin + nx = eye.
        // origin - ny + nz = top left.
        // origin + ny + nz = top right.
        // origin - ny - nz = bottom left.

    public:

        Perspective(const Vec3D &o, 
                float theta, float phi, float ro,
                float len, float wid, float hei);

        inline Perspective() 
            : Perspective(Vec3D(0.0f, 0.0f, 0.0f), 
                    0.0f, 0.0f, 0.0f,
                    2.0f, 1.0f, 0.75f) {}

        // Legacy constructor.
        Perspective(const Vec3D &e, const Vec3D &tl, const Vec3D &tr, const Vec3D &bl);

        inline Vec3D getEye() const {
            return this->origin + this->nx;
        }

        inline Vec3D getTopLeft() const {
            return this->origin + this->nz - this->ny;
        }

        inline Vec3D dw() const {
            return (this->ny * 2.0f) / 320.0f;
        }

        inline Vec3D dh() const {
            return (this->nz * -2.0f) / 240.0f;
        }
    };

    // Point light source.
    class Light {
    private:
        Vec3D position;
        Vec3D color;

    public:
        inline Light() 
            : position(0.0f, 0.0f, 0.0f), 
            color(1.0f, 1.0f, 1.0f) {}

        inline Light(Vec3D p, Vec3D c) 
            : position(p), color(c) {}

        inline const Vec3D &getPosition() const {
            return this->position;
        }

        inline const Vec3D &getColor() const {
            return this->color;
        }
    };

    class Scene {
    private:
        Perspective per;
        Vec3D ia;   // Global ambient light.

        const Geom * const *geoms;
        const size_t geomsLen;

        const Light * const *lights; 
        size_t lightsLen;

    public:
        Scene(const Perspective &p, const Vec3D &iap, 
                const Geom * const *gs, size_t gsl, const Light * const *ls, size_t lsl);

        void render(uint8_t lim) const;
        
        // Trace should return a color.
        // (all components should be between 1 and 0)
        // If lim = 0, No recursion should take place.
        Vec3D trace(const Ray &r, uint8_t lim) const;
    };
}
