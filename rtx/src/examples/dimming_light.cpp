
#include "./dimming_light.h"
#include "../math/scene.h"
#include "../math/plane.h"
#include "../math/material.h"
#include "../math/sphere.h"
#include "../math/rect_prism.h"
#include "rtx/src/math/vec.h"
#include <cmath>


using namespace expls;

void expls::dimmingLight(cxxutil::core::U24 frame, 
            cxxutil::core::U24 numFrames) {
    const math::Perspective PER(
            math::Vec3D(0.0f, 0.0f, 0.0f),
            0.0f, 0.0f, 0.0f, 
            2.0f, 1.0f, 0.75f
    );

    const float FRAME_RATIO = ((float)frame / (float)numFrames);

    // two shapes sitting on a plane.

    const math::Material PLANE_MAT(
            math::Vec3D(0.65f, 0.0f, 0.0f),
            math::Vec3D(0.5f, 0.5f, 0.4f),
            math::Vec3D(0.2f, 0.2f, 0.2f),
            8,
            0.6f
    );

    const math::Plane PLANE(
            math::Vec3D(0.0f, 0.0f, -0.375),
            math::Vec3D(0.0f, 0.0f, 1.0f)
    );

    const math::Material SPHERE_MAT(
            math::Vec3D(0.0f, 0.0f, 0.6f),
            math::Vec3D(0.6f, 0.2f, 0.2f),
            math::Vec3D(0.4f, 0.4f, 0.4f),
            8,
            0.8f
    );

    const float SPHERE_RADIUS = 0.35f;
    const math::Sphere SPHERE(
            math::Vec3D(-3.0f, 0.35f, -0.375 + SPHERE_RADIUS),
            SPHERE_RADIUS
    );

    const math::Material RP_MAT(
            math::Vec3D(0.0f, 1.0f, 0.6f),
            math::Vec3D(0.7f, 0.3f, 0.7f),
            math::Vec3D(0.9f, 0.9f, 0.9f),
            8
    );

    const float RP_HEIGHT = 0.14f;
    const math::RectPrism RP(
            math::Vec3D(-2.0f, -0.25f, -0.375 + (RP_HEIGHT / 2.0f)),        
            0.35f, 0.45f, RP_HEIGHT, M_PI / 6.0f, 0.0f, 0.0f
    );

    const size_t OBJS_LEN = 3;
    const math::SceneObject OBJS[OBJS_LEN] = {
        math::SceneObject(&PLANE, &PLANE_MAT),
        math::SceneObject(&SPHERE, &SPHERE_MAT),
        math::SceneObject(&RP, &RP_MAT)
    };

    const float LIGHT_COEF1 = (cosf(2 * M_PI * FRAME_RATIO) + 1.0f) / 2.0f;
    const float LIGHT_COEF2 = (LIGHT_COEF1 / 2.0f) + 0.5f;

    const math::Light LIGHT(
            math::Vec3D(-0.5f, 0.0f, 0.5f),
            math::Vec3D(LIGHT_COEF1 * 1.0f, LIGHT_COEF1 * 1.0f, LIGHT_COEF2 * 1.0f)
    );

    const size_t LIGHTS_LEN = 1;
    const math::Light LIGHTS[LIGHTS_LEN] = {
        math::Light(
            math::Vec3D(-0.5f, 0.0f, 0.5f),
            math::Vec3D(LIGHT_COEF1 * 1.0f, LIGHT_COEF1 * 1.0f, LIGHT_COEF2 * 1.0f)
        )
    };

    const math::Scene SCENE(
        PER, 
        math::Vec3D(0.15f, 0.15f, 0.15f),
        OBJS, OBJS_LEN,
        LIGHTS, LIGHTS_LEN
    );

    SCENE.render(2);
}
