
#include "./vec.h"
#include "./misc.h"
#include <ti/screen.h>
#include <math.h>
#include <stdio.h>

using namespace math;

Vec3D Vec3D::getNorm(float theta, float phi) {
    float cosPhi = cos(phi); 

    return Vec3D(
        cosPhi * cos(theta),
        cosPhi * sin(theta),
        sin(phi) 
    );
}

Vec3D Vec3D::cross(const Vec3D &o) const {
    float xp = (this->y * o.z) - (this->z * o.y);
    float yp = -((this->x * o.z) - (this->z * o.x));
    float zp = (this->x * o.y) - (this->y * o.x);

    return Vec3D(xp, yp, zp);
}

Vec3D Vec3D::rotate(const Vec3D &axis, float theta) const {
    Vec3D n = axis.norm();
    
    Vec3D proj = this->proj(n);
    Vec3D vp = *this - proj; 

    return proj + (vp * cos(theta)) - (vp.cross(n) * sin(theta));
}

void Vec3D::display() const {
    char bufs[3][20];

    float2str(bufs[0], this->x);
    float2str(bufs[1], this->y);
    float2str(bufs[2], this->z);

    char fBuf[64];
    sprintf(fBuf, "%s, %s, %s", bufs[0], bufs[1], bufs[2]);

    os_PutStrLine(fBuf);
    os_NewLine();
}

