
#include "./rotating_prisms.h"

#include "../math/geom.h"
#include "../math/scene.h"
#include "../math/rect_prism.h"
#include "rtx/src/math/material.h"
#include "rtx/src/math/plane.h"
#include <cxxutil/core/mem.h>

using namespace expls;

RotatingPrisms::RotatingPrisms(cxxutil::core::U24 frame, cxxutil::core::U24 numFrames) 
    : 
       per(
            math::Vec3D(2.0f, 0.0f, 0.0f),
            math::Vec3D(0.0f, -0.5f, 0.375f),
            math::Vec3D(0.0f, 0.5f, 0.375f),
            math::Vec3D(0.0f, -0.5f, -0.375f)
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
       center(-3.5f, 0.0f, 0.14f),
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

       rp0(
               &(this->rpMat),
               this->center + (this->radius * this->ax),
               this->ax * (this->rpLen / 2.0f),
               this->ay * (this->rpWid / 2.0f),
               this->az * (this->rpHei / 2.0f)
       ),
       rp1(
               &(this->rpMat),
               this->center - (this->radius * this->ax),
               -this->ax * (this->rpLen / 2.0f),
               -this->ay * (this->rpWid / 2.0f),
               this->az * (this->rpHei / 2.0f)
       ),
       rp2(
               &(this->rpMat),
               this->center - (this->radius * this->ax),
               this->ay * (this->rpWid / 2.0f),
               -this->ax * (this->rpLen / 2.0f),
               this->az * (this->rpHei / 2.0f)
       ),
       rp3(
               &(this->rpMat),
               this->center - (this->radius * this->ax),
               -this->ay * (this->rpWid / 2.0f),
               this->ax * (this->rpLen / 2.0f),
               this->az * (this->rpHei / 2.0f)
       ),
       rp4(
               &(this->rpMat),
               this->center - (this->radius * this->ax),
               this->az * (this->rpHei / 2.0f),
               this->ay * (this->rpWid / 2.0f),
               -this->ax * (this->rpLen / 2.0f)
       ),
       rp5(
               &(this->rpMat),
               this->center - (this->radius * this->ax),
               -this->az * (this->rpHei / 2.0f),
               this->ay * (this->rpWid / 2.0f),
               this->ax * (this->rpLen / 2.0f)
       ),

       planeMat(
            math::Vec3D(1.0f, 0.0f, 0.0f),
            math::Vec3D(0.65f, 0.0f, 0.45f),
            math::Vec3D(0.4f, 0.4f, 0.4f),
            8,
            0.8f
       ),
       plane(
            &(this->planeMat),
            math::Vec3D(0.0f, 0.0f, -0.375f),
            math::Vec3D(0.0f, 0.0f, 1.0f) 
       ),
       ambFactors(0.15f, 0.15f, 0.15f),
       l0(
            math::Vec3D(-1.0f, 0.0f, 0.5f),
            math::Vec3D(1.0f, 1.0f, 1.0f)
       ) {
}

void RotatingPrisms::render() const {
    const size_t GEOMS_LEN = 7;
    const math::Geom *GEOMS[GEOMS_LEN] = {
        &(this->plane),
        &(this->rp0),
        &(this->rp1),
        &(this->rp2),
        &(this->rp3),
        &(this->rp4),
        &(this->rp5)
    };

    const size_t LIGHTS_LEN = 1;
    const math::Light *LIGHTS[LIGHTS_LEN] = {
        &(this->l0)
    };

    const math::Scene SCENE(
            this->per,
            this->ambFactors,
            GEOMS, 
            GEOMS_LEN,
            LIGHTS,
            LIGHTS_LEN
    );

    SCENE.render(1);
}
