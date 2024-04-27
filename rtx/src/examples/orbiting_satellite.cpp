
#include "./orbiting_satellite.h"
#include "../math/misc.h"
#include "../math/vec.h"

using namespace expls;


OrbitingSatellite::OrbitingSatellite(uint8_t chnl, cxxutil::core::U24 frame, cxxutil::core::U24 numFrames) 
    : cxxutil::core::SafeObject(chnl),
    focus{0.0f, 0.0f, 1.0f},
    r{
        2.0f, 
        1.0f
    },
    axisTheta{0.0f},
    axisPhi{M_PI / 2.0f},
    axisRo{0.0f},
    axisK{
        math::Vec3D::getNorm(this->axisTheta, this->axisPhi)
    },
    axisI{
        math::Vec3D::getNorm(this->axisTheta, this->axisPhi - (M_PI / 2.0f))
            .rotate(this->axisK, this->axisRo)
    },
    axisJ{
        this->axisI.rotate(this->axisK, M_PI / 2.0f)
    },
    theta{
        (float)(2.0f * M_PI) * ((float)frame / (float)numFrames)
    },
    rTheta{
        (2.0f * this->r[0] * this->r[1]) /
            ((this->r[0] + this->r[1]) - 
             (this->r[0] - this->r[1]) * cosf(this->theta))        
    },
    dxdTheta{
        (-2.0f * this->r[0] * this->r[1] * sinf(this->theta) * (this->r[0] + this->r[1])) /
            math::intPow((this->r[0] + this->r[1]) - (this->r[0] - this->r[1]) * cosf(this->theta), 2) 
    },
    dydTheta{
        (2.0f * this->r[0] * this->r[1] * ((this->r[0] + this->r[1]) * cosf(this->theta) - (this->r[0] - this->r[1]))) /
            math::intPow((this->r[0] + this->r[1]) - (this->r[0] - this->r[1]) * cosf(this->theta), 2)
    },
    sat{
        {
            this->focus + (this->rTheta * this->axisI.rotate(this->axisK, this->theta))
        },
        {
            (this->dxdTheta * this->axisI) + (this->dydTheta * this->axisJ)
        }
    },
    per{
        this->sat.getPoint(),
        -this->sat.getDir() * 1.0f,
        this->sat.getDir().rotate(this->axisK, -M_PI / 2.0f) * 0.5f,
        this->axisK * 0.375f
    },
    floorMat{
        {1.0f, 0.0f, 0.0f},
        {0.7f, 0.2f, 0.0f},
        {1.0f, 1.0f, 1.0f},
        8
    },
    floor{
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f}
    },
    objs{
        {&(this->floor), &(this->floorMat)}
    },
    lights{
        {
            {0.0f, 0.0f, 1.0f},
            {1.0f, 1.0f, 1.0f}
        }
    },
    scene{
        this->per,
        {0.15f, 0.15f, 0.15f},
        this->objs, 1,
        this->lights, 1
    } {
}

void OrbitingSatellite::render() const {
    this->scene.render(0);
}
