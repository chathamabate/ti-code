
#pragma once

#include <cxxutil/core/data.h>
#include <cxxutil/core/mem.h>

#include "./geom.h"

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

        inline Vec3D getEye() const {
            return this->eye;
        }

        inline Vec3D getTopLeft() const {
            return this->topLeft;
        }

        inline Vec3D dh() const {
            return (topRight - topLeft) / 320.0f;
        }

        inline Vec3D dv() const {
            return (bottomLeft - topLeft) / 240.0f;
        }
    };

    class Scene : public cxxutil::core::SafeObject {
    private:
        // NOTE: Geoms will NOT be deleted by this scene.
        cxxutil::core::CoreList<Geom *> *geoms;

        Perspective per;

    public:
        Scene(uint8_t chnl, const Perspective &p);
        Scene(const Perspective &p);
        ~Scene();

        inline void addGeom(Geom *g) {
            this->geoms->add(g);
        }

        void render() const;
        
        // Trace should return a color.
        // (all components should be between 1 and 0)
        // If lim = 0, No recursion should take place.
        Vec3D trace(const Ray &r, uint8_t lim) const;
    };
}
