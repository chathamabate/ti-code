
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
        const float frameRatio;

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

        // Our home planet.
        const math::Material homeMat;
        const math::Sphere home;

        const math::Material bigPrismMat;
        const math::RectPrism bigPrism;

        // Galaxy is going to provide the light
        // for the scene.
        const math::Material galaxyMat;
        const math::Disk galaxy;

        const math::Material bigPlanetMat;
        const math::Sphere bigPlanet;

        const math::SceneObject objs[4];
        const math::Light lights[1];

        const math::Scene scene;

    public:
        OrbitingSatellite(uint8_t chnl, cxxutil::core::U24 frame, cxxutil::core::U24 numFrames);

        void render() const;
    };
}
