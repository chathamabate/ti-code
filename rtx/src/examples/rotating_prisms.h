
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

        const math::RectPrism rp0;
        const math::RectPrism rp1;
        const math::RectPrism rp2;
        const math::RectPrism rp3;
        const math::RectPrism rp4;
        const math::RectPrism rp5;

        const math::Material planeMat;
        const math::Plane plane;

        const math::Vec3D ambFactors;
        const math::Light l0;

    public:
        RotatingPrisms(cxxutil::core::U24 frame, cxxutil::core::U24 numFrames);

        void render() const;
    };
}
