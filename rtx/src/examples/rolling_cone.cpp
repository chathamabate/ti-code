
#include "./rolling_cone.h"
#include "cxxutil/core/mem.h"
#include "rtx/src/math/scene.h"
#include "rtx/src/math/vec.h"

using namespace expls;

RollingCone::RollingCone(uint8_t chnl, cxxutil::core::U24 frame, cxxutil::core::U24 numFrames) 
    : cxxutil::core::SafeObject(chnl),
    per(
            math::Vec3D(0.0f, 0.0f, 0.0f),
            0.0f, 0.0f, 0.0f, 
            2.0f, 1.0f, 0.75f
    ),
    center(-1.55f, -0.40f, -0.375f),
    theta(((float)frame / (float)numFrames) * M_PI * 2.0f),
    phi(M_PI / 8.3f),
    length(0.32f),
    coneMat(
            math::Vec3D(0.4f, 0.0f, 0.0f),
            math::Vec3D(0.15f, 0.15f, 0.8f),
            math::Vec3D(1.0f, 1.0f, 1.0f),
            6
    ),
    cone(
            this->center + (0.5f * this->length * 
                math::Vec3D::getNorm(theta, phi)),
            this->length, 
            this->length * tan(this->phi),
            this->theta, this->phi 
    ),
    sphereMat(
            math::Vec3D(0.4f, 0.0f, 0.0f),
            math::Vec3D(0.1f, 0.1f, 0.1f),
            math::Vec3D(0.85f, 0.85f, 0.85f),
            20,
            0.95f
    ),
    sphere(
            math::Vec3D(-3.3f, 0.35f, 0.4f - 0.375),
            0.4f
    ),
    planeMat(
            math::Vec3D(0.4f, 0.0f, 0.0f),
            math::Vec3D(0.2f, 0.45f, 0.1f),
            math::Vec3D(0.3f, 0.3f, 0.3f),
            8
    ),
    plane(
             math::Vec3D(0.0f, 0.0f, -0.375f),
             math::Vec3D(0.0f, 0.0f, 1.0f)
    ),
    objs{
            math::SceneObject(
                    &(this->cone),
                    &(this->coneMat)
            ),
            math::SceneObject(
                    &(this->sphere),
                    &(this->sphereMat)
            ),
            math::SceneObject(
                    &(this->plane),
                    &(this->planeMat)
            )
    },
    lights{
            math::Light(
                    math::Vec3D(0.0f, -2.0f, 2.0f),
                    math::Vec3D(1.0f, 1.0f, 1.0f)
            )
    },
    scene(
            this->per,
            math::Vec3D(0.15f, 0.15f, 0.15f),
            this->objs, 3,
            this->lights, 1
            
    ) { 
}

void RollingCone::render() const {
    this->scene.render(1);
}
