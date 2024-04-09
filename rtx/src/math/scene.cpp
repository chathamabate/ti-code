
#include <ti/screen.h>
#include "./scene.h"
#include "cxxutil/core/data.h"
#include "cxxutil/core/mem.h"
#include "rtx/src/math/geom.h"
#include "rtx/src/math/material.h"
#include "rtx/src/math/vec.h"
#include "./misc.h"

using namespace math;

Scene::Scene(uint8_t chnl, const Perspective &p, const Vec3D &iap) : cxxutil::core::SafeObject(chnl), per(p), ia(iap) {
    this->geoms = new cxxutil::core::CoreList<Geom *>(chnl);
    this->lights = new cxxutil::core::CoreList<Light>(chnl);
}

Scene::Scene(const Perspective &p, const Vec3D &iap) 
    : Scene(cxxutil::core::CXX_DEF_CHNL, p, iap) {}

Scene::~Scene() {
    delete this->geoms;
    delete this->lights;
}

void Scene::render() const {
    Vec3D eye = this->per.getEye();

    Vec3D rowStart = this->per.getTopLeft();

    Vec3D dh = this->per.dh();
    Vec3D dv = this->per.dv();

    // Screen vram.
    uint16_t *vram = (uint16_t *)lcd_Ram;

    for (uint8_t row = 0; row < 240; row++) {
        Vec3D pixel = rowStart;

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

        rowStart += dv;
    }
}

Vec3D Scene::trace(const Ray &r, uint8_t lim) const {
    Geom *g = NULL;
    Ray n;      // Normal of the intersection point.
    float s;    // parameter value of the interestion point with respect
                // to r.

    for (size_t i = 0; i < this->geoms->getLen(); i++) {
        Geom *gp = this->geoms->get(i);

        Ray np;
        float sp;

        if (gp->intersect(r, &np, &sp)  
                && (!g || sp < s)) {
            g = gp;
            s = sp;
            n = np;
        }
    }

    if (!g) {
        return Vec3D(0.0f, 0.0f, 0.0f);
    }

    // Otherwise we have an intersection!
    // Wooo!

    n.normalize();
    
    const Material *mat = g->getMat();

    // Start with ambient lighting... all surfaces get this.
    Vec3D color = this->ia.flatMult(mat->getKa());
     
    // Now we need our pointer to the light...
    for (size_t j = 0; j < this->lights->getLen(); j++) {
        Light light = this->lights->get(j);

        // l is the ray from our intersection point to this light source.
        Ray l(n.getPoint(), light.getPosition() - n.getPoint());

        bool obstructed = false;
        for (size_t i = 0; i < this->geoms->getLen() && !obstructed; i++) {
            Geom *g = this->geoms->get(i);

            // Neither of these will be used, however, they're needed for intersect.
            Ray rp;
            float sp;

            if (g->intersect(l, &rp, &sp) && sp < 1.0f + ERR)  {
                obstructed = true;
            }
        }

        // In this case, there is a geom between our intersection point and *this* light souce.
        // So, we just move on to the next light source.
        if (obstructed) {
            continue;
        }

        // Otherwise, let's do our color calculations! 
        l.normalize();

        // First diffuse. 
        float diffuseCosine = n.getDir() * l.getDir();
        color += diffuseCosine * mat->getKd().flatMult(light.getColor());

        Vec3D lRef = (2 * (l.getDir() * n.getDir()) * n.getDir()) - l.getDir();
        float specularCosine  = intPow(lRef * n.getDir(), mat->getAlpha());
        color += specularCosine * mat->getKs().flatMult(light.getColor());
    } 

    return color.ceil(1.0f);
}