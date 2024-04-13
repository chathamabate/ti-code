
#include <ti/screen.h>
#include "./scene.h"
#include "cxxutil/core/data.h"
#include "cxxutil/core/mem.h"
#include "rtx/src/math/geom.h"
#include "rtx/src/math/material.h"
#include "rtx/src/math/vec.h"
#include "./misc.h"

using namespace math;

Scene::Scene(const Perspective &p, const Vec3D &iap, 
    const Geom * const *gs, size_t gsl, const Light * const *ls, size_t lsl) 
    : per(p), ia(iap), geoms(gs), geomsLen(gsl), lights(ls), lightsLen(lsl) {
}

void Scene::render(uint8_t lim) const {
    Vec3D eye = this->per.getEye();

    Vec3D rowStart = this->per.getTopLeft();

    Vec3D dh = this->per.dh();
    Vec3D dv = this->per.dv();

    // Screen vram.
    uint16_t *vram = (uint16_t *)lcd_Ram;

    for (uint8_t row = 0; row < 240; row++) {
        Vec3D pixel = rowStart;
        size_t rowSI = row * 320;

        for (cxxutil::core::U24 col = 0; col < 320; col++) {
            Ray r(pixel, pixel - eye);

            Vec3D color = this->trace(r, lim);

            uint8_t red = color.getX()   * 31;  
            uint8_t green = color.getY() * 63;
            uint8_t blue = color.getZ()  * 31;

            uint16_t rgb = (red << 11) | (green << 5) | blue;

            vram[rowSI + col] = rgb;
            
            pixel += dh;
        }

        rowStart += dv;
    }
}

Vec3D Scene::trace(const Ray &r, uint8_t lim) const {
    const Geom *g = NULL;
    Ray n;      // Normal of the intersection point.
    float s;    // parameter value of the interestion point with respect
                // to r.

    for (size_t i = 0; i < this->geomsLen; i++) {
        const Geom *gp = this->geoms[i];

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
    //
    // All surfaces get ambient lighting.
    const Material *mat = g->getMat();
    Vec3D color = this->ia.flatMult(mat->getKa());

    // All surfaces have a front side and a back side.
    // The back side will only ever get ambient lighting.
    if ((-r.getDir()) * n.getDir() < ERR) {
        return color;
    }

    n.normalize();
     
    // Now we need our pointer to the light...
    for (size_t j = 0; j < this->lightsLen; j++) {
        Light light = *(this->lights[j]);

        // l is the ray from our intersection point to this light source.
        Ray l(n.getPoint(), light.getPosition() - n.getPoint());

        // Here the normal and direction to light are in opposite directions.
        // No need to do specular / diffuse calculations.
        if (l.getDir() * n.getDir() < ERR) {
            continue;
        }

        bool obstructed = false;
        for (size_t i = 0; i < this->geomsLen && !obstructed; i++) {
            const Geom *gp = this->geoms[i];

            // Skip self if not self shadowable.
            if (gp == g && !(g->selfShadowable())) {
                continue;
            }

            // Neither of these will be used, however, they're needed for intersect.
            Ray rp;
            float sp;

            if (gp->intersect(l, &rp, &sp) && sp < 1.0f + ERR)  {
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
        float cosine = n.getDir() * l.getDir();

        // First diffuse. 
        color += cosine * mat->getKd().flatMult(light.getColor());

        // Vec3D lRef = (2 * cosine * n.getDir()) - l.getDir();
        float specularCosine = intPow(cosine, mat->getAlpha());
        color += specularCosine * mat->getKs().flatMult(light.getColor());
    } 

    // Finally reflections!
    if (lim > 0 && mat->isReflective()) {
        Vec3D toEye = -r.getDir();
        Vec3D refDir = (2 * (n.getDir() * toEye) * n.getDir()) - toEye;
        Ray ref(n.getPoint(), refDir);

        Vec3D refColor = this->trace(ref, lim - 1);

        color += mat->getReflectivity() * refColor;
    }

    return color.ceil(1.0f);
}
