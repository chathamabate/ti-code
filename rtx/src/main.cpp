#include <cmath>
#include <cstdio>
#include <cxxutil/core/input.h>
#include <ti/screen.h>
#include <graphx.h>
#include <sys/lcd.h>
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
#include "./examples/rolling_cone.h"
#include "./examples/elliptical_orbits.h"
#include "./examples/elliptical_lights.h"
#include "./examples/orbiting_satellite.h"

using namespace cxxutil;

int main(void) {
    const cxxutil::core::U24 NUM_FRAMES = 400;
    const cxxutil::core::U24 FRAME_START = 351;
    const cxxutil::core::U24 FRAME_END = 399;
    const cxxutil::core::U24 FRAME_SKIP = 7;

    char lblBuf[30];

    for (cxxutil::core::U24 f = FRAME_START; f <= FRAME_END; f += FRAME_SKIP) {
        os_ClrHome();

        expls::OrbitingSatellite *os = 
            new expls::OrbitingSatellite(1, f, NUM_FRAMES);
        os->render();
        delete os;

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
