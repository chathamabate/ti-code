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
#include <rtx/src/math/cone.h>

#include "./examples/orbiting_cyl.h"
#include "./examples/dimming_light.h"
#include "./examples/rotating_prisms.h"

using namespace cxxutil;

int main(void) {
    const math::Perspective PER(
            math::Vec3D(2.0f, 0.0f, 0.0f),
            math::Vec3D(0.0f, -0.5f, 0.375f),
            math::Vec3D(0.0f, 0.5f, 0.375f),
            math::Vec3D(0.0f, -0.5f, -0.375f)
    );

    const math::Material MAT(
            math::Vec3D(1.0f, 0.0f, 0.0f),
            math::Vec3D(1.0f, 0.0f, 0.0f),
            math::Vec3D(1.0f, 1.0f, 1.0f),
            8
    );

    const math::Cone CONE(
            &MAT, math::Vec3D(-2.0f, 0.0f, 0.0f),
            math::Vec3D(-1.0f, 0.0f, 0.0f),
            0.5f, 0.2f
    );

    const math::Geom * const SHAPES[1] = {&CONE};

    const math::Light L(
            math::Vec3D(0.0f, 0.0f, 0.375f),
            math::Vec3D(1.0f, 1.0f, 1.0f)
    );
    const math::Light *const LIGHTS[1] = {&L};

    const math::Scene SCENE(
            PER, math::Vec3D(0.15f, 0.15f, 0.15f),
            SHAPES, 1, LIGHTS, 1
    );


    os_ClrHome();
    SCENE.render(0);

    while (!os_GetCSC());
    exit(1);


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
