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
#include "./examples/rotating_cube.h"
#include "rtx/src/math/sphere.h"

using namespace cxxutil;

class SceneWrapper {
private:
    math::Material spMat;
    math::Sphere sp;

    math::Material prMat;
    math::RectPrism pr;

    math::Material plMat;
    math::Plane pl;

    math::SceneObject objs[3];
    math::Light lights[1];

    math::Perspective per;
    math::Scene sc; 
public:
    SceneWrapper() : 
    spMat{
        {1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        5
    },
    sp{
        {-2.0f, 0.3f, -0.375 + 0.25},
        0.25f
    },
    prMat{
        {1.0f, 0.0f, 0.0f},
        {0.0f, 0.6f, 0.35f},
        {1.0f, 1.0f, 1.0f},
        15
    },
    pr{
        {-1.65f, -0.25f, -0.375 + 0.075f},
        0.3f, 0.35f, 0.15f,
        M_PI / 0.6f, 0.0f, 0.0f
    },
    plMat{
        {1.0f, 0.0f, 0.0f},
        {0.2f, 0.0f, 0.1f},
        {0.6f, 0.6f, 0.6f},
        13, 0.75f
    },
    pl{
        {0.0f, 0.0f, -0.375f},
        {0.0f, 0.0f, 1.0f},
    },
    objs{
        {&sp, &spMat},
        {&pr, &prMat},
        {&pl, &plMat},
    },
    lights{
        {
            {-0.4f, 0.0f, 0.75f},
            {1.0f, 1.0f, 1.0f}
        }
    },
    per{
        {0.0f, 0.0f, 0.0f},
        0.0f, 0.0f, 0.0f,
        2.0f, 1.0f, 0.75f
    },
    sc{
        this->per,
        {0.15f, 0.15f, 0.15f},
        this->objs, 3,
        this->lights, 1
    } {}

    void render() {
        this->sc.render(1);
    }
};

int main(void) {
    SceneWrapper *sw = new SceneWrapper();

    os_ClrHome();

    sw->render();

    delete sw;

    while (!os_GetCSC());
    os_ClrHome();
}
