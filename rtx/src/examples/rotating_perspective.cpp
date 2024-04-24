#include "./rotating_perspective.h"
#include "cxxutil/core/mem.h"
#include "rtx/src/math/rect_plane.h"
#include "rtx/src/math/scene.h"
#include "rtx/src/math/vec.h"

using namespace expls;

RotatingPerspective::RotatingPerspective(uint8_t chnl, cxxutil::core::U24 frame, cxxutil::core::U24 numFrames) 
    : cxxutil::core::SafeObject(chnl), 
    theta(((float)frame / (float)numFrames) * M_PI * 2.0f),
    radius(3.0f),
    per(
            math::Vec3D(
                this->radius * cos(this->theta),
                this->radius * sin(this->theta),
                0.0f
            ),
            theta, 0.0f, 0.0f, 
            2.0f, 1.0f, 0.75f
    ),
    sphereMat(
            math::Vec3D(0.5f, 0.0f, 0.5f),
            math::Vec3D(0.2f, 0.3f, 0.6f),
            math::Vec3D(1.0f, 1.0f, 1.0f),
            6 
    ),
    sphere(
            math::Vec3D(-0.2f, -0.4f, -0.375f + 0.2f),
            0.2f
    ), 
    rectPrismMat(
            math::Vec3D(0.5f, 0.0f, 0.5f),
            math::Vec3D(0.0f, 0.7f, 0.3f),
            math::Vec3D(1.0f, 1.0f, 1.0f),
            6 
    ),
    rectPrism(
            math::Vec3D(-0.15f, 0.3f, -0.375f + 0.075f),
            0.35f, 0.25f, 0.15f,
            -M_PI / 8.0f, 0.0f, 0.0f
    ),
    coneMat(
            math::Vec3D(0.5f, 0.0f, 0.5f),
            math::Vec3D(0.7f, 0.1f, 0.2f),
            math::Vec3D(1.0f, 1.0f, 1.0f),
            10
    ),
    cone(
            math::Vec3D(0.3f, -0.1f, -0.375f + 0.2f),
            0.4f, 0.2f,
            0.0f, -M_PI / 2.0f
    ),
    planeMat(
            math::Vec3D(0.5f, 0.0f, 0.5f),
            math::Vec3D(0.7f, 0.0f, 0.4f),
            math::Vec3D(0.5f, 0.5f, 0.5f),
            8,
            0.8f
    ),
    plane(
            math::Vec3D(0.0f, 0.0f, -0.375f),
            math::Vec3D(0.0f, 0.0f, 1.0f)
    ),
    objects{
        math::SceneObject(&(this->sphere), &(this->sphereMat)),
        math::SceneObject(&(this->rectPrism), &(this->rectPrismMat)),
        math::SceneObject(&(this->cone), &(this->coneMat)),
        math::SceneObject(&(this->plane), &(this->planeMat))
    },
    lights{
        math::Light(
                math::Vec3D(this->radius, 0.0f, 0.15f),
                math::Vec3D(1.0f, 1.0f, 1.0f)
        )
    },
    scene(
            this->per,
            math::Vec3D(0.15f, 0.15f, 0.15f), 
            this->objects, 4,
            this->lights, 1
    ) {
}

void RotatingPerspective::render() const {
    this->scene.render(1);
}
