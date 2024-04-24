
#pragma once

#include <cxxutil/core/mem.h>

#include "../math/cone.h"
#include "../math/rect_prism.h"
#include "../math/sphere.h"
#include "../math/plane.h"
#include "../math/scene.h"
#include "../math/material.h"

namespace expls {
    class RotatingPerspective : public cxxutil::core::SafeObject {
    private:
        const float theta;
        const float radius;

        const math::Perspective per;

        const math::Material sphereMat;
        const math::Sphere sphere; 

        const math::Material rectPrismMat;
        const math::RectPrism rectPrism;

        const math::Material coneMat;
        const math::Cone cone;

        const math::Material planeMat;
        const math::Plane plane;

        const math::SceneObject objects[4];
        const math::Light lights[1];

        const math::Scene scene;

    public:
        RotatingPerspective(uint8_t chnl, cxxutil::core::U24 frame, cxxutil::core::U24 numFrames);

        void render() const;
    };
}
