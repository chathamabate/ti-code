#include <cmath>
#include <cstdio>
#include <cxxutil/core/input.h>
#include <ti/screen.h>
#include <graphx.h>
#include <sys/lcd.h>
#include "cxxutil/core/mem.h"
#include "rtx/src/examples/rotating_perspective.h"
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
    const cxxutil::core::U24 NUM_FRAMES = 240;
    const cxxutil::core::U24 FRAME_START = 5;
    const cxxutil::core::U24 FRAME_END = 239;
    const cxxutil::core::U24 FRAME_SKIP = 6;

    char lblBuf[30];

    for (cxxutil::core::U24 f = FRAME_START; f <= FRAME_END; f += FRAME_SKIP) {
        os_ClrHome();
        expls::dimmingLight(0, 240);

        /*
        expls::RotatingPerspective *rp = 
            new expls::RotatingPerspective(1, f, NUM_FRAMES);
        rp->render();
        delete rp;
        */

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
