
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

    r0{0.5f}, r1{0.15f},

    axisTheta{M_PI / 6.0f},
    axisPhi{M_PI / 2.5f},
    axis{math::Vec3D::getNorm(this->axisTheta, this->axisPhi)},
    orient{math::Vec3D::getNorm(this->axisTheta, this->axisPhi - (M_PI / 2.0f))},

    orbitCenter{-2.0f, 0.25f, 0.0},
    orbitFocii{
        {
            orbitCenter + (this->r0 - this->r1) * this->orient,
            -this->orient
        },
        {
            orbitCenter + (this->r0 - this->r1) * this->orient.rotate(this->axis, 2.0 * M_PI / 3.0f),
            -this->orient.rotate(this->axis, 2.0 * M_PI / 3.0f)
        },
        {
            orbitCenter + (this->r0 - this->r1) * this->orient.rotate(this->axis, 4.0 * M_PI / 3.0f),
            -this->orient.rotate(this->axis, 4.0 * M_PI / 3.0f)
        }
    },

    theta{(float)(2.0f * M_PI * ((float)frame / (float)numFrames))},

    // Formula for ellipse with a focus at the origin in polar coordinates.
    orbitRadius{
        (2.0f * this->r0 * this->r1) / 
        (this->r0 + this->r1 - ((this->r0 - this->r1) * cosf(this->theta)))
    },

    sphereRadius{0.07f},
    sphereMat{
        {1.0f, 0.0f, 0.0f},
        {0.2f, 0.5f, 0.6f},
        {1.0f, 1.0f, 1.0f},
        10 
    },
    spheres{
        {
            this->orbitFocii[0].getPoint() + 
                this->orbitRadius * this->orbitFocii[0].getDir().rotate(this->axis, this->theta),
            this->sphereRadius
        },
        {
            this->orbitFocii[1].getPoint() + 
                this->orbitRadius * this->orbitFocii[1].getDir().rotate(this->axis, this->theta),
            this->sphereRadius
        },
        {
            this->orbitFocii[2].getPoint() + 
                this->orbitRadius * this->orbitFocii[2].getDir().rotate(this->axis, this->theta),
            this->sphereRadius
        },
    },
    planeMat{
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {0.7f, 0.7f, 0.7f},
        4 
    },
    plane{
        {0.0f, -0.5f, 0.0f},
        {0.0f, 1.0f, 0.0f}
    },
    objs{
        {&(this->spheres[0]), &(this->sphereMat)},
        {&(this->spheres[1]), &(this->sphereMat)},
        {&(this->spheres[2]), &(this->sphereMat)},
        {&(this->plane), &(this->planeMat)}
    },
    lights{
        {
            {-1.3f, 2.0f, 0.375f},
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
