
#pragma once

#include <cxxutil/core/data.h>
#include <cxxutil/core/mem.h>

#include "./geom.h"
#include "rtx/src/math/vec.h"

namespace math {
    class Perspective {
    private:
        // eye point.
        Vec3D eye;

        // 3 Corners of the screen.
        Vec3D topLeft;
        Vec3D topRight;
        Vec3D bottomLeft;

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
        Vec3D position;
        Vec3D color;

    public:
        inline Light() : position(0, 0, 0), color(0, 0, 0) {}
        inline Light(Vec3D p, Vec3D c) 
            : position(p), color(c) {}

        inline const Vec3D &getPosition() const {
            return this->position;
        }

        inline const Vec3D &getColor() const {
            return this->color;
        }
    };

    class Scene : public cxxutil::core::SafeObject {
    private:

        // NOTE: Geoms will NOT be deleted by this scene.
        cxxutil::core::CoreList<const Geom *> *geoms;
        cxxutil::core::CoreList<Light> *lights;

        Perspective per;
        Vec3D ia;   // Global ambient light.

    public:
        Scene(uint8_t chnl, const Perspective &p, const Vec3D &iap);
        Scene(const Perspective &p, const Vec3D &iap);
        ~Scene();

        inline void addGeom(const Geom *g) {
            this->geoms->add(g);
        }

        inline void addLight(const Light &l) {
            this->lights->add(l);
        }

        void render() const;
        
        // Trace should return a color.
        // (all components should be between 1 and 0)
        // If lim = 0, No recursion should take place.
        Vec3D trace(const Ray &r, uint8_t lim) const;
    };
}
