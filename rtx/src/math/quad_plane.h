
#pragma once

#include "./geom.h"
#include "rtx/src/math/material.h"
#include "./plane.h"


namespace math {
    class QuadPlane : public Plane {
    
        // NOTE: this is objectively confusing...
        // You specify the quad plane by giving three points.
        // Each point represents a corner of the plane.
        //
        // What is stored is, the center of the quadralateral specified
        // a normal vector, and two other vectors.
        //
        // center + right + up = the top right of the quad plane.
        // center - right - up = the bottom left of the quad plane.

    private:
        Vec3D right;
        Vec3D up;

    public:
        QuadPlane(const Material *m, const Vec3D &tl, 
                const Vec3D &tr, const Vec3D &bl);

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}
