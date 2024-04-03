#include "./misc.h"
#include "ti/screen.h"
#include <ti/real.h>

using namespace math;

void math::float2str(char *buf, float val) {
    real_t tmp_real = os_FloatToReal(val);
    os_RealToStr(buf, &tmp_real, 8, 1, 3);
}

void math::displayFloat(float val) {
    char buf[20];
    float2str(buf, val);

    os_PutStrLine(buf);
    os_NewLine();
}
