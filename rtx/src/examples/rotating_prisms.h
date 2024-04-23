
#pragma once

#include "../math/geom.h"
#include "../math/scene.h"
#include "../math/vec.h"
#include "../math/rect_prism.h"
#include "../math/material.h"
#include "../math/plane.h"

#include <stdint.h>
#include <cxxutil/core/mem.h>

namespace expls {

    class RotatingPrisms : public cxxutil::core::SafeObject {
    private:
        const math::Perspective per;

        const math::Material rpMat;

        const float rpLen;
        const float rpWid;
        const float rpHei;

        const math::Vec3D center;
        const float radius;

        const float frameRatio;

        const float azTheta;
        const float azPhi;
        const float theta;

        const math::Vec3D az;
        const math::Vec3D ax;
        const math::Vec3D ay;

        const math::RectPrism rps[6];

        const math::Material planeMat;
        const math::Plane plane;

        const math::SceneObject objs[7];

        const math::Vec3D ambFactors;
        const math::Light lights[1];

    public:
        RotatingPrisms(uint8_t chnl, cxxutil::core::U24 frame, cxxutil::core::U24 numFrames);

        void render() const;
    };
}
