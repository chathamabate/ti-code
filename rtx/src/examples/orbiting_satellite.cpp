
#include "./orbiting_satellite.h"
#include "../math/misc.h"
#include "../math/vec.h"

using namespace expls;


OrbitingSatellite::OrbitingSatellite(uint8_t chnl, cxxutil::core::U24 frame, cxxutil::core::U24 numFrames) 
    : cxxutil::core::SafeObject(chnl),
    frameRatio{(float)frame / (float)numFrames},
    focus{0.0f, 0.0f, 0.0f},
    r{
        2000.0f,
        80.0f
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
        (float)(2.0f * M_PI * this->frameRatio)
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
            ((this->dxdTheta * this->axisI) + (this->dydTheta * this->axisJ)).norm()
        }
    },
    per{
        this->sat.getPoint(),
        -this->sat.getDir() * 1.0f,
        this->sat.getDir().rotate(this->axisK, -M_PI / 2.0f) * 0.5f,
        this->axisK * 0.375f
    },
    homeMat{
        {1.0f, 0.0f, 0.0f},
        {0.6f, 0.2f, 0.2f},
        {1.0f, 1.0f, 1.0f},
        6 
    },
    home{
        this->focus + (this->r[0] -this->r[1]) * this->axisI,
        79.0f

    },
    bigPrismMat{
        {1.0f, 0.0f, 0.0f},
        {0.7f, 0.6f, 0.1f},
        {1.0f, 1.0f, 1.0f},
        14 
    },
    bigPrism{
        {-660.0f, 930.0f, -100.0f},
        230.0f, 230.0f, 230.0f,
        // In this entire animation, we will only rotate by PI!
        M_PI / 6.0f, M_PI / 3.0f, (float)(M_PI * this->frameRatio)
    },
    galaxyMat{
        {1.0f, 0.0f, 0.0f},
        {0.1f, 0.6f, 0.1f},
        {1.0f, 1.0f, 1.0f},
        6 
    },
    galaxy{
        {-1550.0f, -990.0f, 200.0f},
        math::Vec3D::getNorm(M_PI / 10.0f, -M_PI / 8.0f),
        2100.0f
    },
    bigPlanetMat{
        {1.0f, 0.0f, 0.0f},
        {0.1f, 0.1f, 0.6f},
        {0.4f, 0.4f, 0.4f},
        25,
        0.8f
    },
    bigPlanet{
        {-270.0f, -1280.0f, 0.0f},
        620.0f
    },
    objs{
        {&(this->home), &(this->homeMat)},
        {&(this->bigPrism), &(this->bigPrismMat)},
        {&(this->galaxy), &(this->galaxyMat)},
        {&(this->bigPlanet), &(this->bigPrismMat)},
    },
    lights{
        {
            this->galaxy.getCenter() + this->galaxy.getNorm() * 100.0f,
            {1.0f, 1.0f, 1.0f}
        }
    },
    scene{
        this->per,
        {0.15f, 0.15f, 0.15f},
        this->objs, 4,
        this->lights, 1
    } {
}

void OrbitingSatellite::render() const {
    this->scene.render(0);
}
