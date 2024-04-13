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

using namespace cxxutil;


int main(void) {
    const uint8_t FRAME_START = 60;
    const uint8_t FRAME_END = 119;    // Inclusive.
    const uint8_t FRAME_SKIP = 6;

    char lblBuf[30];

    for (uint8_t f = FRAME_START; f <= FRAME_END; f += FRAME_SKIP) {
        os_ClrHome();
        expls::dimmingLight(f, 120);

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
