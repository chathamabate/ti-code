
#include "./scene.h"
#include "cxxutil/core/data.h"
#include "cxxutil/core/mem.h"

using namespace math;

Scene::Scene(uint8_t chnl, const Perspective &p) : cxxutil::core::SafeObject(chnl), per(p) {
    this->geoms = new cxxutil::core::CoreList<Geom *>();
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

    for (uint8_t r = 0; r < 240; r++) {
        for (cxxutil::core::U24 c = 0; c < 320; c++) {
            Ray r(pixel, pixel - eye);

            
            pixel += dh;
        }

        pixel += dv;
    }
}
