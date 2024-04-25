
#include "./elliptical_orbits.h"
#include "cxxutil/core/mem.h"
#include "rtx/src/math/scene.h"
#include "rtx/src/math/vec.h"

using namespace expls;

EllipticalOrbits::EllipticalOrbits(uint8_t chnl, cxxutil::core::U24 frame, cxxutil::core::U24 numFrames) 
    : cxxutil::core::SafeObject{chnl},
    per{
        {0.0f, 0.0f, 0.0f},
        0.0f, 0.0f, 0.0f, 
        2.0f, 1.0f, 0.75f
    },
    axisTheta{M_PI / 6.0f},
    axisPhi{M_PI / 2.5f},
    axis{math::Vec3D::getNorm(this->axisTheta, this->axisPhi)},
    orbitCenter{-2.0f, 0.25f, 0.0},

    theta{(float)(2.0f * M_PI * ((float)frame / (float)numFrames))},
    r0{0.5f}, r1{0.15f},

    // Formula for ellipse with a focus at the origin in polar coordinates.
    orbitRadius{
        (2.0f * this->r0 * this->r1) / 
        (this->r0 + this->r1 - ((this->r0 - this->r1) * cosf(this->theta)))
    },

    sphereRadius{0.07f},
    mats{
        {
            {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
            4 
        }, {
            {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
            4 
        }, {
            {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
            4 
        }
    }, 
    spheres{
        {
            this->orbitCenter + (math::Vec3D::getNorm(this->axisTheta, this->axisPhi - (M_PI / 2.0f))
                .rotate(this->axis, this->theta) * this->orbitRadius),
            this->sphereRadius
        },
        {
            this->orbitCenter + (math::Vec3D::getNorm(this->axisTheta, this->axisPhi - (M_PI / 2.0f))
                .rotate(this->axis, this->theta + (2 * M_PI / 3)) * this->orbitRadius),
            this->sphereRadius
        },
        {
            this->orbitCenter + (math::Vec3D::getNorm(this->axisTheta, this->axisPhi - (M_PI / 2.0f))
                .rotate(this->axis, this->theta + (4 * M_PI / 3)) * this->orbitRadius),
            this->sphereRadius
        },
    },
    planeMat{
        {1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        6 
    },
    plane{
        {0.0f, -0.5f, 0.0f},
        {0.0f, 1.0f, 0.0f}
    },
    objs{
        {&(this->spheres[0]), &(this->mats[0])},
        {&(this->spheres[1]), &(this->mats[1])},
        {&(this->spheres[2]), &(this->mats[2])},
        {&(this->plane), &(this->planeMat)}
    },
    lights{
        {
            {-1.75f, 2.0f, 0.375f},
            {1.0f, 1.0f, 1.0f}
        }
    },
    scene{
        this->per, 
        math::Vec3D(0.15f, 0.15f, 0.15f),
        this->objs, 4, 
        this->lights, 1
    } {
}

void EllipticalOrbits::render() const {
    this->scene.render(1);
}
