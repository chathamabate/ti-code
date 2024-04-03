
#include "./vec.h"
#include "./misc.h"
#include <ti/screen.h>
#include <stdio.h>

using namespace math;

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

