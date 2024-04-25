#pragma once

#include <cxxutil/core/mem.h>

#include "../math/scene.h"
#include "../math/cone.h"
#include "../math/material.h"
#include "../math/plane.h"
#include "../math/vec.h"
#include "../math/sphere.h"

namespace expls {
    class RollingCone : public cxxutil::core::SafeObject {
    private:
        const math::Perspective per;

        // Where the cone is in its circular path.
        const math::Vec3D center;
        const float theta;
        const float phi;
        const float length;

        const math::Material coneMat;
        const math::Cone cone;

        const math::Material sphereMat;
        const math::Sphere sphere;

        const math::Material planeMat;
        const math::Plane plane;

        const math::SceneObject objs[3];
        const math::Light lights[1];

        const math::Scene scene;

    public:
        RollingCone(uint8_t chnl, cxxutil::core::U24 frame, cxxutil::core::U24 numFrames);

        void render() const;
    };
}
