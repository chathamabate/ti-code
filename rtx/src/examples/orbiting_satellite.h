
#pragma once

#include <cxxutil/core/mem.h>

#include "../math/scene.h"
#include "../math/cone.h"
#include "../math/material.h"
#include "../math/plane.h"
#include "../math/vec.h"
#include "../math/sphere.h"
#include "../math/rect_prism.h"
#include "rtx/src/math/ray.h"

namespace expls {
    class OrbitingSatellite : public cxxutil::core::SafeObject {
    private:
        // Specs for our orbit.
        const math::Vec3D focus;
        const float r[2];
        const float axisTheta, axisPhi, axisRo;

        const math::Vec3D axisK;
        const math::Vec3D axisI;
        const math::Vec3D axisJ;    // axisI X axisJ = axisK

        // We are going to follow an elliptical orbit.
        const float theta;
        const float rTheta;
        const float dxdTheta;
        const float dydTheta;

        // our satellite and its direction!
        const math::Ray sat;
        const math::Perspective per;

        /*
        const math::Material homeMat;
        const math::Sphere home;

        const math::Material satRPMat;
        const math::RectPrism satRP;

        const math::Material satConeMat;
        const math::Cone satCone;

        const math::Material satSphereMat;
        const math::Sphere satSphere;
        */

        
        const math::Material floorMat;
        const math::Plane floor;

        const math::SceneObject objs[1];
        const math::Light lights[1];

        const math::Scene scene;

    public:
        OrbitingSatellite(uint8_t chnl, cxxutil::core::U24 frame, cxxutil::core::U24 numFrames);

        void render() const;
    };
}
