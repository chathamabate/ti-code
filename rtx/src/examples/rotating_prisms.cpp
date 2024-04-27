
#include "./rotating_prisms.h"

#include "../math/geom.h"
#include "../math/scene.h"
#include "../math/rect_prism.h"
#include "rtx/src/math/material.h"
#include "rtx/src/math/plane.h"
#include "rtx/src/math/rect_plane.h"
#include <cxxutil/core/mem.h>

using namespace expls;

RotatingPrisms::RotatingPrisms(uint8_t chnl, cxxutil::core::U24 frame, cxxutil::core::U24 numFrames) 
    : 
       cxxutil::core::SafeObject(chnl),
       per(
            math::Vec3D(0.0f, 0.0f, 0.0f),
            0.0f, 0.0f, 0.0f, 
            2.0f, 1.0f, 0.75f
       ),
       rpMat(
            math::Vec3D(1.0f, 0.0f, 0.0f),
            math::Vec3D(0.0f, 0.2f, 0.7f),
            math::Vec3D(1.0f, 1.0f, 1.0f),
            8
       ),
       rpLen(0.3f), 
       rpWid(0.1f),
       rpHei(0.1f),
       center(-5.25f, 0.0f, 0.12f),
       radius(0.3f),

       frameRatio((float)frame / (float)numFrames),
       azTheta(M_PI / 6.0f),
       azPhi(M_PI / 4.0f),
       theta(this->frameRatio * 2 * M_PI),

       az(math::Vec3D::getNorm(this->azTheta, this->azPhi)),
       ax(math::Vec3D::getNorm(this->azTheta, this->azPhi - (M_PI / 2.0f))
               .rotate(this->az, this->theta)),
       ay(math::Vec3D::getNorm(this->azTheta + (M_PI / 2.0f), 0.0f)
               .rotate(this->az, this->theta)),

       rps{
           math::RectPrism(
                   this->center + (this->radius * this->ax),
                   this->ax * (this->rpLen / 2.0f),
                   this->ay * (this->rpWid / 2.0f),
                   this->az * (this->rpHei / 2.0f)
           ),
           math::RectPrism(
                   this->center - (this->radius * this->ax),
                   -this->ax * (this->rpLen / 2.0f),
                   -this->ay * (this->rpWid / 2.0f),
                   this->az * (this->rpHei / 2.0f)
           ),
           math::RectPrism(
                   this->center + (this->radius * this->ay),
                   this->ay * (this->rpLen / 2.0f),
                   -this->ax * (this->rpWid / 2.0f),
                   this->az * (this->rpHei / 2.0f)
           ),
           math::RectPrism(
                   this->center - (this->radius * this->ay),
                   -this->ay * (this->rpLen / 2.0f),
                   this->ax * (this->rpWid / 2.0f),
                   this->az * (this->rpHei / 2.0f)
           ),
           math::RectPrism(
                   this->center + (this->radius * this->az),
                   this->az * (this->rpLen / 2.0f),
                   this->ay * (this->rpWid / 2.0f),
                   -this->ax * (this->rpHei / 2.0f)
           ),
           math::RectPrism(
                   this->center - (this->radius * this->az),
                   -this->az * (this->rpLen / 2.0f),
                   this->ay * (this->rpWid / 2.0f),
                   this->ax * (this->rpHei / 2.0f)
           )
       },

       planeMat(
            math::Vec3D(1.0f, 0.0f, 0.0f),
            math::Vec3D(0.65f, 0.0f, 0.45f),
            math::Vec3D(0.4f, 0.4f, 0.4f),
            8,
            0.8f
       ),
       plane(
            math::Vec3D(0.0f, 0.0f, -0.375f),
            math::Vec3D(0.0f, 0.0f, 1.0f) 
       ),
       objs{
           math::SceneObject(&(this->plane), &(this->planeMat)),
           math::SceneObject(&(this->rps[0]), &(this->rpMat)),
           math::SceneObject(&(this->rps[1]), &(this->rpMat)),
           math::SceneObject(&(this->rps[2]), &(this->rpMat)),
           math::SceneObject(&(this->rps[3]), &(this->rpMat)),
           math::SceneObject(&(this->rps[4]), &(this->rpMat)),
           math::SceneObject(&(this->rps[5]), &(this->rpMat)),
       },
       ambFactors(0.15f, 0.15f, 0.15f),
       lights{
           math::Light(
                math::Vec3D(-1.0f, 0.0f, 0.5f),
                math::Vec3D(1.0f, 1.0f, 1.0f)
           )
       } {
}

void RotatingPrisms::render() const {
    const math::Scene SCENE(
            this->per,
            this->ambFactors,
            this->objs, 7,
            this->lights, 1
    );

    SCENE.render(0);
}
