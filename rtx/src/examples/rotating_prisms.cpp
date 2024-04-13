
#include "./rotating_prisms.h"

#include "../math/geom.h"
#include "../math/scene.h"
#include "../math/rect_prism.h"
#include "rtx/src/math/material.h"
#include "rtx/src/math/plane.h"
#include <cxxutil/core/mem.h>

using namespace expls;

void expls::rotatingPrisms(cxxutil::core::U24 frame, cxxutil::core::U24 numFrames) {
    const math::Perspective PER(
            math::Vec3D(2.0f, 0.0f, 0.0f),
            math::Vec3D(0.0f, -0.5f, 0.375f),
            math::Vec3D(0.0f, 0.5f, 0.375f),
            math::Vec3D(0.0f, -0.5f, -0.375f)
    );

    const float FRAME_RATIO = ((float)frame / (float)numFrames);

    const math::Material MAT(
            math::Vec3D(1.0f, 0.0f, 0.0f),
            math::Vec3D(0.0f, 0.2f, 0.7f),
            math::Vec3D(1.0f, 1.0f, 1.0f),
            8
    );

    const float LENGTH = 0.3f;
    const float WIDTH = 0.1f;
    const float HEIGHT = 0.1f;

    const math::Vec3D CENTER(-3.5f, 0.0f, 0.12f);
    const float RADIUS = 0.3f;

    const float AZ_THETA = M_PI / 6.0f;
    const float AZ_PHI = M_PI / 4.0f;

    const float THETA = FRAME_RATIO * 2 * M_PI;

    // The prisms will rotate about an axis.
    const math::Vec3D AZ = math::Vec3D::getNorm(AZ_THETA, AZ_PHI);
    const math::Vec3D AX = math::Vec3D::getNorm(AZ_THETA, AZ_PHI - (M_PI / 2.0f))
        .rotate(AZ, THETA);
    const math::Vec3D AY = math::Vec3D::getNorm(AZ_THETA + (M_PI / 2.0f), 0.0f)
        .rotate(AZ, THETA);

    const math::RectPrism RP0(
            &MAT, 
            CENTER + (RADIUS * AX),
            (AX * (LENGTH / 2.0f)),
            (AY * (WIDTH / 2.0f)),
            (AZ * (HEIGHT / 2.0f))
    );

    const math::RectPrism RP1(
            &MAT, 
            CENTER - (RADIUS * AX),
            -(AX * (LENGTH / 2.0f)),
            -(AY * (WIDTH / 2.0f)),
            (AZ * (HEIGHT / 2.0f))
    );

    const math::RectPrism RP2(
            &MAT, 
            CENTER + (RADIUS * AY),
            (AY * (LENGTH / 2.0f)), 
            -(AX * (WIDTH / 2.0f)),
            (AZ * (HEIGHT / 2.0f))
    );

    const math::RectPrism RP3(
            &MAT, 
            CENTER - (RADIUS * AY),
            -(AY * (LENGTH / 2.0f)), 
            (AX * (WIDTH / 2.0f)),
            (AZ * (HEIGHT / 2.0f))
    );

    const math::RectPrism RP4(
            &MAT, 
            CENTER + (RADIUS * AZ),
            (AZ * (LENGTH / 2.0f)), 
            (AY * (WIDTH / 2.0f)),
            -(AX * (HEIGHT / 2.0f))
    );

    const math::RectPrism RP5(
            &MAT, 
            CENTER - (RADIUS * AZ),
            -(AZ * (LENGTH / 2.0f)), 
            (AY * (WIDTH / 2.0f)),
            (AX * (HEIGHT / 2.0f))
    );

    const math::Material PLANE_MAT(
            math::Vec3D(1.0f, 0.0f, 0.0f),
            math::Vec3D(0.65f, 0.0f, 0.45f),
            math::Vec3D(0.4f, 0.4f, 0.4f),
            8,
            0.8f
    );

    const math::Plane PLANE(
            &PLANE_MAT, 
            math::Vec3D(0.0f, 0.0f, -0.375f),
            math::Vec3D(0.0f, 0.0f, 1.0f) 
    );

    const size_t GEOMS_LEN = 7;
    const math::Geom *GEOMS[GEOMS_LEN] = {
        &PLANE,
        &RP0,
        &RP1,
        &RP2,
        &RP3,
        &RP4,
        &RP5
    };

    math::Light L0( 
            math::Vec3D(-1.0f, 0.0f, 0.5f),
            math::Vec3D(1.0f, 1.0f, 1.0f)
    );

    const size_t LIGHTS_LEN = 1;
    const math::Light *LIGHTS[LIGHTS_LEN] = {
        &L0
    };

    const math::Scene SCENE(
            PER,
            math::Vec3D(0.15f, 0.15f, 0.15f),
            GEOMS, 
            GEOMS_LEN,
            LIGHTS,
            LIGHTS_LEN
    );

    SCENE.render(1);
}
