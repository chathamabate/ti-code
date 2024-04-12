#include <cmath>
#include <cxxutil/core/input.h>
#include <ti/screen.h>
#include <graphx.h>
#include <sys/lcd.h>
#include "./math/misc.h"
#include "./math/vec.h"
#include "./math/geom.h"
#include "./math/sphere.h"
#include "cxxutil/core/mem.h"
#include "rtx/src/math/cylinder.h"
#include "rtx/src/math/disk.h"
#include "rtx/src/math/material.h"
#include "rtx/src/math/ray.h"
#include "rtx/src/math/rect_prism.h"
#include "rtx/src/math/scene.h"
#include <rtx/src/math/plane.h>
#include <ti/real.h>
#include <rtx/src/math/rect_plane.h>

using namespace cxxutil;

int main(void) {
    
    // Ok, nice, this works!
    os_ClrHome(); 

    const math::Perspective per(
            math::Vec3D(2.0f, 0.0f, 0.0f),
            math::Vec3D(0.0f, -0.5f, 0.375f),
            math::Vec3D(0.0f, 0.5f, 0.375f),
            math::Vec3D(0.0f, -0.5f, -0.375f)
    );

    const uint8_t FRAME = 34;
    const uint8_t TOTAL_FRAMES = 60;

    const float FRAME_RATIO = ((float)FRAME / (float)TOTAL_FRAMES);

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
            &SPHERE_MAT,
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
            &CYL_MAT, CYL_CENTER, 
            CYL_LEN, CYL_RADIUS, 
            CYL_THETA, CYL_PHI
    );

    math::Scene *sc = new math::Scene(2, per, 
            math::Vec3D(0.15f, 0.15f, 0.15f));

    sc->addLight(math::Light(
        math::Vec3D(-1.0f, -0.3f, 0.375f),
        math::Vec3D(1.0f, 1.0f, 1.0f)
    ));

    sc->addGeom(&CYL);
    sc->addGeom(&SPHERE);
    sc->render();    

    while (!os_GetCSC());

    os_ClrHome();

    delete sc;

    core::MemoryTracker::ONLY->checkMemLeaks();
}
