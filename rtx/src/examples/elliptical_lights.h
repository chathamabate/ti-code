
#pragma once

#include <cxxutil/core/mem.h>

#include "../math/plane.h"
#include "../math/scene.h"
#include "../math/material.h"
#include "../math/vec.h"
#include "../math/rect_prism.h"
#include "../math/cone.h"

namespace expls {
    class EllipticalLights : public cxxutil::core::SafeObject {
    private:
        const math::Perspective per;

        const math::Vec3D focus;

        const float axisTheta, axisPhi;
        const math::Vec3D axis;
        const math::Vec3D orient;

        const float lens[2];
        const float thetas[2];
        const float radii[2];
        const math::Vec3D points[2];

        const math::Material planeMat;
        const math::Plane plane;

        const math::Material coneMat;
        const math::Cone cone;

        const math::SceneObject objs[2];

        const math::Light lights[2];

        const math::Scene scene;

    public:
        EllipticalLights(uint8_t chnl, cxxutil::core::U24 frame, cxxutil::core::U24 numFrames);

        void render() const;
    };
}
