
#include "./orbiting_cyl.h"
#include "../math/scene.h"
#include "../math/cylinder.h"
#include "../math/sphere.h"

using namespace expls;

void expls::orbitingCylinder(cxxutil::core::U24 frame, 
            cxxutil::core::U24 numFrames) {

    const math::Perspective PER(
            math::Vec3D(2.0f, 0.0f, 0.0f),
            math::Vec3D(0.0f, -0.5f, 0.375f),
            math::Vec3D(0.0f, 0.5f, 0.375f),
            math::Vec3D(0.0f, -0.5f, -0.375f)
    );

    const float FRAME_RATIO = ((float)frame / (float)numFrames);

    const math::Vec3D SPHERE_CENTER(-3.0f, 0.25f, 0.0f);
    const float SPHERE_RADIUS = 0.5f;
    const float ORBIT_SPACE = 0.23f;
    const float CYL_RADIUS = 0.028f;
    const float CYL_LEN = 0.14f;
    const uint8_t CYL_ROTATIONS = 1;

    const math::Vec3D CYL_CENTER = SPHERE_CENTER + 
        (SPHERE_RADIUS + ORBIT_SPACE) * math::Vec3D::getNorm((-M_PI / 2.0f) + (FRAME_RATIO * 2 * M_PI), 0.0f);
    const float CYL_THETA = FRAME_RATIO * 2 * M_PI;
    const float CYL_PHI = (M_PI / 2.0f) + (-FRAME_RATIO * (float)CYL_ROTATIONS * (2 * M_PI));

    const math::Material SPHERE_MAT(
            math::Vec3D(0.0f, 0.3f, 0.0f),
            math::Vec3D(0.0f, 0.0f, 0.4f),
            math::Vec3D(0.5f, 0.5f, 0.5f),
            8,
            0.8f
    );

    const math::Sphere SPHERE(
            SPHERE_CENTER,
            SPHERE_RADIUS
    );

    const math::Material CYL_MAT(
            math::Vec3D(0.0f, 0.0f, 0.4f),
            math::Vec3D(0.8f, 0.0f, 0.3f),
            math::Vec3D(0.8f, 0.8f, 0.8f),
            8
    );

    const math::Cylinder CYL(
            CYL_CENTER, 
            CYL_LEN, CYL_RADIUS, 
            CYL_THETA, CYL_PHI
    );

    const size_t OBJS_LEN = 2;
    const math::SceneObject OBJS[OBJS_LEN] = {
        math::SceneObject(&SPHERE, &SPHERE_MAT),
        math::SceneObject(&CYL, &CYL_MAT) 
    };

    const size_t LIGHTS_LEN = 1;
    const math::Light LIGHTS[LIGHTS_LEN] = {
        math::Light( 
                math::Vec3D(-1.0f, -0.3f, 0.375f),
                math::Vec3D(1.0f, 1.0f, 1.0f)
        )
    };

    const math::Scene SCENE(
            PER,
            math::Vec3D(0.15f, 0.15f, 0.15f),
            OBJS, OBJS_LEN,
            LIGHTS,
            LIGHTS_LEN
    );

    SCENE.render(1);
}
