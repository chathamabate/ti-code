
#pragma once

#include <cxxutil/core/mem.h>

#include "../math/scene.h"
#include "../math/material.h"
#include "../math/vec.h"
#include "../math/rect_prism.h"

namespace expls {
    class RotatingCube : public cxxutil::core::SafeObject {
    private:
        const math::Perspective per;

        const float frameRatio;

        const float sideLen;
        const math::Material cubeMat;
        const math::RectPrism cube;

        const math::SceneObject objs[1];
        const math::Light lights[2];

        const math::Scene scene;
    public:
        RotatingCube(uint8_t chnl, cxxutil::core::U24 frame, cxxutil::core::U24 numFrames);

        void render() const;
    };
}
