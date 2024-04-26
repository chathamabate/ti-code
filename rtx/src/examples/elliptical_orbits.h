
#pragma once

#include <cxxutil/core/mem.h>

#include "../math/sphere.h"
#include "../math/plane.h"
#include "../math/scene.h"
#include "../math/material.h"
#include "../math/vec.h"

namespace expls {
    class EllipticalOrbits : public cxxutil::core::SafeObject {
    private:
        const math::Perspective per;

        const float r0, r1;

        const float axisTheta;
        const float axisPhi;

        const math::Vec3D axis;
        const math::Vec3D orient;
        const math::Vec3D orbitCenter;
        const math::Ray orbitFocii[3];

        const float theta;
        const float orbitRadius;

        const float sphereRadius;

        const math::Material sphereMat;
        const math::Sphere spheres[3];

        const math::Material planeMat;
        const math::Plane plane;

        const math::SceneObject objs[4];
        const math::Light lights[1];

        const math::Scene scene;

    public:
        EllipticalOrbits(uint8_t chnl, cxxutil::core::U24 frame, cxxutil::core::U24 numFrames);
        void render() const;
    };
}
