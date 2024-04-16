#include <cmath>
#include <cstdio>
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

#include "./examples/orbiting_cyl.h"
#include "./examples/dimming_light.h"
#include "./examples/rotating_prisms.h"

using namespace cxxutil;

int main(void) {
    math::Perspective per(
        math::Vec3D(0.0f, 0.0f, 1.5f),
       0.0f, M_PI / 2.0f,  0.0f,
        2.0f, 1.0f, 0.75f
    );

    math::Cylinder defCyl;

    const math::Geom *gs[1] = {&defCyl};

    math::Light l0(
            math::Vec3D(2.0f, 0.0f, 2.0f),
            math::Vec3D(1.0f, 1.0f, 1.0f)
    );
    const math::Light *ls[1] = {&l0};

    math::Scene s(
        per, math::Vec3D(0.15f, 0.15f, 0.15f),
        gs, 1, ls, 1
    );

    s.render(0);
    while (!os_GetCSC());
    exit(1);



    /*
    const math::Perspective PER(
            math::Vec3D(2.0f, 0.0f, 0.0f),
            math::Vec3D(0.0f, -0.5f, 0.375f),
            math::Vec3D(0.0f, 0.5f, 0.375f),
            math::Vec3D(0.0f, -0.5f, -0.375f)
    );

    os_ClrHome();
    PER.getEye().display();
    PER.getTopLeft().display();
    while (!os_GetCSC());
    exit(1);
    */


    const cxxutil::core::U24 NUM_FRAMES = 240;
    const cxxutil::core::U24 FRAME_START = 5;
    const cxxutil::core::U24 FRAME_END = 59;
    const cxxutil::core::U24 FRAME_SKIP = 6;

    char lblBuf[30];

    for (cxxutil::core::U24 f = FRAME_START; f <= FRAME_END; f += FRAME_SKIP) {
        os_ClrHome();

        expls::orbitingCylinder(f, NUM_FRAMES);

        while (!os_GetCSC());

        sprintf(lblBuf, "Last Frame: %d", f);
        os_PutStrFull(lblBuf);

        while (!os_GetCSC());
    }

    os_ClrHome();
    os_PutStrLine("Done Rendering");
    while (!os_GetCSC());

    os_ClrHome();

    core::MemoryTracker::ONLY->checkMemLeaks();
}
