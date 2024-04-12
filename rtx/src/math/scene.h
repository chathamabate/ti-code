
#pragma once

#include "./geom.h"
#include "./vec.h"

namespace math {
    class Perspective {
    private:
        // eye point.
        const Vec3D eye;

        // 3 Corners of the screen.
        const Vec3D topLeft;
        const Vec3D topRight;
        const Vec3D bottomLeft;

    public:
        inline Perspective(const Vec3D &e, const Vec3D &tl, const Vec3D &tr, const Vec3D &bl) 
            : eye(e), topLeft(tl), topRight(tr), bottomLeft(bl) {}

        inline const Vec3D &getEye() const {
            return this->eye;
        }

        inline const Vec3D &getTopLeft() const {
            return this->topLeft;
        }

        inline Vec3D dh() const {
            return (topRight - topLeft) / 320.0f;
        }

        inline Vec3D dv() const {
            return (bottomLeft - topLeft) / 240.0f;
        }
    };

    // Point light source.
    class Light {
    private:
        const Vec3D position;
        const Vec3D color;

    public:
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
        const Perspective per;
        const Vec3D ia;   // Global ambient light.

        const Geom * const * const geoms;
        const size_t geomsLen;

        const Light * const * const lights; 
        const size_t lightsLen;

    public:
        Scene(const Perspective &p, const Vec3D &iap, 
                const Geom * const *gs, size_t gsl, const Light * const *ls, size_t lsl);

        void render() const;
        
        // Trace should return a color.
        // (all components should be between 1 and 0)
        // If lim = 0, No recursion should take place.
        Vec3D trace(const Ray &r, uint8_t lim) const;
    };
}
