
#include "./elliptical_lights.h"
#include "cxxutil/core/mem.h"
#include "rtx/src/examples/elliptical_orbits.h"
#include "rtx/src/math/vec.h"

using namespace expls;

EllipticalLights::EllipticalLights(uint8_t chnl, cxxutil::core::U24 frame, cxxutil::core::U24 numFrames) 
    : cxxutil::core::SafeObject(chnl),
    per{
        {0.0f, 0.0f, 0.0f},
        0.0f, 0.0f, 0.0f, 
        2.0f, 1.0f, 0.75f
    },
    focus{0.0f, 0.0f, 0.0f},
    axisTheta{},
    axisPhi{},
    axis{math::Vec3D::getNorm(this->axisTheta, this->axisPhi)},
    orient{math::Vec3D::getNorm(this->axisTheta, this->axisPhi - (M_PI / 2.0f))},

    lens{
        0.0f,
        0.0f
    },
    thetas{
        (float)((2 * M_PI * (float)frame / (float)numFrames)),
        (float)(((2 * M_PI * (float)frame / (float)numFrames)) + M_PI)
    },
    radii{
        (2 * this->lens[0] * this->lens[1]) /
            ((this->lens[0] + this->lens[1]) - (this->lens[0] - this->lens[1]) * cosf(thetas[0])),
        (2 * this->lens[0] * this->lens[1]) /
            ((this->lens[0] + this->lens[1]) - (this->lens[0] - this->lens[1]) * cosf(thetas[1]))
    },
    points{
        this->focus + 
            (this->radii[0] * this->orient.rotate(this->axis, this->thetas[0])),
        this->focus + 
            (this->radii[1] * this->orient.rotate(this->axis, this->thetas[1]))
    },
    planeMat{
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        8
    },
    plane{
        {0.0f, 0.0f, -0.375f},
        {0.0f, 0.0f, 1.0f}
    },
    objs{
        {&(this->plane), &(this->planeMat)}
    },
    lights{
        {
            this->points[0],
            {0.0f, 0.0f, 0.0f}
        },
        {
            this->points[1],
            {0.0f, 0.0f, 0.0f}
        }
    },
    scene{
        this->per,
        {0.15f, 0.15f, 0.15f},
        this->objs, 1,
        this->lights, 2
    }
    {

}
