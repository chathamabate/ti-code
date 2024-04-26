
#include "./elliptical_lights.h"
#include "cxxutil/core/mem.h"
#include "rtx/src/examples/elliptical_orbits.h"
#include "rtx/src/math/vec.h"

using namespace expls;

EllipticalLights::EllipticalLights(uint8_t chnl, cxxutil::core::U24 frame, cxxutil::core::U24 numFrames) 
    : cxxutil::core::SafeObject(chnl),
    per{
        {0.0f, 0.0f, 0.375f},
        M_PI / 12.0f, M_PI / 6.0f, 0.0f, 
        0.35f, 1.0f, 0.75f
    },
    focus{-2.0f, -0.7f, 1.0f},
    axisTheta{6.0f * M_PI / 7.0f},
    axisPhi{M_PI / 2.0f},
    axis{math::Vec3D::getNorm(this->axisTheta, this->axisPhi)},
    orient{math::Vec3D::getNorm(this->axisTheta, this->axisPhi - (M_PI / 2.0f))},

    lens{
        5.0f,
        1.5f
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
        {1.0f, 0.0f, 0.0f},
        {0.2f, 0.2f, 0.7f},
        {1.0f, 1.0f, 1.0f},
        18 
    },
    plane{
        {0.0f, 0.0f, -0.375f},
        {0.0f, 0.0f, 1.0f}
    },
    coneMat{
        {1.0f, 0.0f, 0.0f},
        {0.8f, 0.8f, 0.8f},
        {1.0f, 1.0f, 1.0f},
        12 
    },
    cone{
        {-2.0f, -0.7f, -0.375f + 0.5f},
        1.0f, 0.45f, 
        0.0f, -M_PI / 2.0f
    },
    objs{
        {&(this->plane), &(this->planeMat)},
        {&(this->cone), &(this->coneMat)}
    },
    lights{
        {
            this->points[0],
            {1.0f, 0.5f, 0.5f}
        },
        {
            this->points[1],
            {0.5f, 0.5f, 1.0f}
        }
    },
    scene{
        this->per,
        {0.15f, 0.15f, 0.15f},
        this->objs, 2,
        this->lights, 2
    } {
}

void EllipticalLights::render() const {
    this->scene.render(1);
}
