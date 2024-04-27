
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
        {0.5f, 0.5f, 0.0f},
        {0.6f, 0.2f, 0.6f},
        {0.5f, 0.5f, 0.5f},
        8
    },
    home{
        this->focus + (this->r[0] -this->r[1]) * this->axisI,
        79.0f

    },
    bigPrismMat{
        {0.5f, 0.0f, 0.0f},
        {0.9f, 0.3f, 0.3f},
        {1.0f, 1.0f, 1.0f},
        14 
    },
    bigPrism{
        {-1200.0f, 1400.0f, -100.0f},
        500.0f, 500.0f, 500.0f,
        // In this entire animation, we will only rotate by PI!
        M_PI / 6.0f, M_PI / 3.0f, (float)(M_PI * this->frameRatio)
    },
    galaxyMat{
        {0.3f, 0.0f, 0.0f},
        {0.1f, 0.3f, 0.2f},
        {0.5f, 0.5f, 0.5f},
        4 
    },
    galaxy{
        {-2780.0f, -940.0f, 600.0f},
        math::Vec3D::getNorm(M_PI / 10.0f, -M_PI / 8.0f),
        4000.0f
    },
    bigPlanetMat{
        {0.4f, 0.0f, 0.0f},
        {0.1f, 0.1f, 0.6f},
        {0.4f, 0.4f, 0.4f},
        25,
        0.8f
    },
    bigPlanet{
        {-930.0f, -2670.0f, 0.0f},
        1000.0f
    },
    objs{
        {&(this->home), &(this->homeMat)},
        {&(this->bigPrism), &(this->bigPrismMat)},
        {&(this->galaxy), &(this->galaxyMat)},
        {&(this->bigPlanet), &(this->bigPlanetMat)},
    },
    lights{
        {
            this->galaxy.getCenter() + this->galaxy.getNorm() * 400.0f,
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
    this->scene.render(1);
}
