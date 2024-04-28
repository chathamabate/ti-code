
#include "./rotating_cube.h"
#include "cxxutil/core/mem.h"

using namespace expls;

RotatingCube::RotatingCube(uint8_t chnl, cxxutil::core::U24 frame, cxxutil::core::U24 numFrames) 
    : cxxutil::core::SafeObject{chnl},
    per{
        {0.0f, 0.0f, 0.0f},
        0.0f, 0.0f, 0.0f,
        2.0f, 1.0f, 0.75
    },
    frameRatio{(float)frame / (float)numFrames},

    sideLen{0.4f},
    cubeMat{
        {0.5f, 0.0f, 0.0f},
        {0.5f, 0.5f, 0.5f},
        {1.0f, 1.0f, 1.0f},
        14 
    },

    cube{
        {-2.0f, 0.0f, 0.0f},
        this->sideLen, this->sideLen, this->sideLen,
        (float)(frameRatio * 2.0f * M_PI), 
        (float)(frameRatio * 2.0f * M_PI), 
        0.0f
    },

    objs{
        {&(this->cube), &(this->cubeMat)}
    },
    lights{
        {
            {-1.6, -1.0f, 0.0f},
            {0.8f, 0.4f, 0.0f}
        },
        {
            {-1.6, 1.0f, 0.0f},
            {0.4f, 0.8f, 0.0f}
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

void RotatingCube::render() const {
    this->scene.render(0);
}
