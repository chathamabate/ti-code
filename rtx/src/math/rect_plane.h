
#pragma once

#include "./geom.h"
#include "rtx/src/math/material.h"
#include "./plane.h"


namespace math {
    class RectPlane : public Plane {
    
    private:
        Vec3D v1; 
        Vec3D v2; 

        // theta and phi are the polar coordinates of the unit normal vector.
        // ro will be how much the rectanglar plane is rotated clockwise about the norm.
        
    public:
        RectPlane(const Material *m, const Vec3D &c, 
                float wid, float hei, float theta, float phi, float ro);

        virtual bool intersect(Ray ray, Ray *outR, float *outS) const override;
    };
}