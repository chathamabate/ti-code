
#include <ti/screen.h>
#include "./scene.h"
#include "cxxutil/core/data.h"
#include "cxxutil/core/mem.h"
#include "rtx/src/math/geom.h"

using namespace math;

Scene::Scene(uint8_t chnl, const Perspective &p) : cxxutil::core::SafeObject(chnl), per(p) {
    this->geoms = new cxxutil::core::CoreList<Geom *>(chnl);
}

Scene::Scene(const Perspective &p) : Scene(cxxutil::core::CXX_DEF_CHNL, p) {}

Scene::~Scene() {
    delete this->geoms;
}

void Scene::render() const {
    Vec3D eye = this->per.getEye();

    Vec3D pixel = this->per.getTopLeft();
    Vec3D dh = this->per.dh();
    Vec3D dv = this->per.dv();

    // Screen vram.
    uint16_t *vram = (uint16_t *)lcd_Ram;

    for (uint8_t row = 0; row < 240; row++) {
        for (cxxutil::core::U24 col = 0; col < 320; col++) {
            Ray r(pixel, pixel - eye);

            Vec3D color = this->trace(r, 0);

            uint8_t red = color.getX()   * 31;  
            uint8_t green = color.getY() * 63;
            uint8_t blue = color.getZ()  * 31;

            uint16_t rgb = (red << 11) | (green << 5) | blue;

            vram[(row * 320) + col] = rgb;
            
            pixel += dh;
        }

        pixel += dv;
    }
}

Vec3D Scene::trace(const Ray &r, uint8_t lim) const {
    for (size_t i = 0; i < this->geoms->getLen(); i++) {
        Geom *g = this->geoms->get(i);

        Ray rp;
        float sp;

        if (g->intersect(r, &rp, &sp)) {
            return Vec3D(0, 1.0f, 0);
        }
    }

    return Vec3D(0, 0.0f, 0);
}
