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

    math::Perspective per(
            math::Vec3D(2.0f, 0.0f, 0.0f),
            math::Vec3D(0.0f, -0.5f, 0.375f),
            math::Vec3D(0.0f, 0.5f, 0.375f),
            math::Vec3D(0.0f, -0.5f, -0.375f)
    );

    math::Material mat1(
            math::Vec3D(0.2f, 0.0f, 0.0f),
            math::Vec3D(0.0f, 0.0f, 0.3f),
            math::Vec3D(0.5f, 0.5f, 0.5f),
            8,
            0.9f
    );

    math::Material mat2(
            math::Vec3D(0.0f, 1.0f, 0.0f),
            math::Vec3D(0.9f, 0.15f, 0.25f),
            math::Vec3D(1.0f, 1.0f, 1.0f),
            5
    );

    math::Material mat3(
            math::Vec3D(0.0f, 0.0f, 1.0f),
            math::Vec3D(0.25f, 0.45f, 0.0f),
            math::Vec3D(0.65f, 0.65f, 0.65f),
            7 
    );

    math::Cylinder cy(&mat2,
            math::Vec3D(-2.10f, -0.2f, 0.1f - 0.375), 
            0.2f, 0.2f, 0.0f, M_PI / 2.0f);

    math::Plane pl(&mat3, 
            math::Vec3D(0.0f, 0.0f, -0.375f),
            math::Vec3D(0.0f, 0.0f, 1.0f)
    );

    math::Sphere sp(&mat1, 
            math::Vec3D(-2.5f, 0.3f, -0.375 + 0.3f),
            0.3f
    );

    math::RectPrism rp(&mat2, 
            math::Vec3D(-2.15f, -0.45f, -0.375f + (0.1f / 2.0f)),
            0.35f, 0.45f, 0.1f, 
            M_PI / 6, 0.0f, 0.0f
    );

    math::Scene *sc = new math::Scene(2, per, 
            math::Vec3D(0.15f, 0.15f, 0.15f));

    sc->addLight(math::Light(
        math::Vec3D(-1.0f, 0.0f, 0.375f),
        math::Vec3D(1.0f, 1.0f, 1.0f)
    ));

    sc->addGeom(&cy);
    sc->addGeom(&sp);
    sc->addGeom(&pl);
    sc->render();    

    while (!os_GetCSC());

    os_ClrHome();

    delete sc;

    core::MemoryTracker::ONLY->checkMemLeaks();
}
