
#pragma once

#include "./geom.h"
#include "./vec.h"
#include "rtx/src/math/material.h"

namespace math {
    class Perspective {
    private:
        // eye point.

        const Vec3D nx;
        const Vec3D ny;
        const Vec3D nz;

        // origin will be the center of the viewing plane.
        const Vec3D origin;

        // origin + nx = eye.
        // origin - ny + nz = top left.
        // origin + ny + nz = top right.
        // origin - ny - nz = bottom left.

    public:
        Perspective(const Vec3D &o, 
                float theta, float phi, float ro,
                float len, float wid, float hei);

        // Direct Constructor. 
        Perspective(const Vec3D &o, const Vec3D &nxp, const Vec3D &nyp, const Vec3D &nzp);

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

    class SceneObject {
    private:
        const Geom * const geom;
        const Material * const mat;

    public:
        inline SceneObject(const Geom *g, const Material *m) : geom(g), mat(m) {}
        inline const Geom *getGeom() const {
            return this->geom;
        }
        inline const Material *getMaterial() const {
            return this->mat;
        }
    };


    class Scene {
    private:
        const Perspective per;
        const Vec3D ia;   // Global ambient light.

        const SceneObject * const objects;
        const size_t objectsLen;

        const Light * const lights; 
        const size_t lightsLen;

    public:
        Scene(const Perspective &p, const Vec3D &iap, 
                const SceneObject *os, size_t osl, const Light *ls, size_t lsl);

        void render(uint8_t lim) const;
        
        // Trace should return a color.
        // (all components should be between 1 and 0)
        // If lim = 0, No recursion should take place.
        Vec3D trace(const Ray &r, uint8_t lim) const;
    };
}
