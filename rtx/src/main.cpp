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
            math::Vec3D(0.0f, 0.0f, 0.4f),
            math::Vec3D(0.5f, 0.5f, 0.5f),
            8,
            0.7f
    );

    math::Material mat2(
            math::Vec3D(0.0f, 1.0f, 0.0f),
            math::Vec3D(0.9f, 0.15f, 0.25f),
            math::Vec3D(1.0f, 1.0f, 1.0f),
            5
    );

    math::Material mat3(
            math::Vec3D(0.0f, 0.0f, 1.0f),
            math::Vec3D(0.45f, 0.65f, 0.0f),
            math::Vec3D(1.0f, 1.0f, 1.0f),
           7 
    );

    math::Plane pl(&mat1, 
            math::Vec3D(0.0f, 0.0f, -0.375f),
            math::Vec3D(0.0f, 0.0f, 1.0f)
    );

    math::RectPrism rp(&mat2, 
            math::Vec3D(-3.5f, -0.15f, 0.1),
            0.35f, 0.25f, 0.25f, 
            M_PI / 3.0f, M_PI / 3, M_PI / 4.0f
    );

    math::Sphere sp(&mat3, 
            math::Vec3D(-2.0f, 0.4f, 0.2f - 0.375),
            0.2f
    );

    math::Scene *sc = new math::Scene(2, per, 
            math::Vec3D(0.15f, 0.15f, 0.15f));

    sc->addLight(math::Light(
        math::Vec3D(-0.5f, 0.0f, 0.5f),
        math::Vec3D(1.0f, 1.0f, 1.0f)
    ));

    sc->addGeom(&pl);
    sc->addGeom(&rp);
    sc->addGeom(&sp);

    sc->render();    

    /*
    uint16_t *vram = (uint16_t *)lcd_Ram;

    for (int r = 0; r < 240; r++) {
        uint8_t r8 = (r * 32) / 240;
        uint8_t g8 = 0;
        uint8_t b8 = ((240 - r - 1) * 32) / 240;
           
        // uint16_t color = gfx_RGBTo1555((r * 255) / 240, 0, 0);

        uint16_t color = (r8 << 11) | (g8 << 5) | b8;

        for (int c = 0; c < 320; c++) {
            vram[r*320 + c] = color;
        }
    }
    */

    while (!os_GetCSC());

    os_ClrHome();


    delete sc;

    core::MemoryTracker::ONLY->checkMemLeaks();
}
