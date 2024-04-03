#include "./misc.h"
#include <ti/real.h>

using namespace math;

void math::float2str(char *buf, float val) {
    real_t tmp_real = os_FloatToReal(val);
    os_RealToStr(buf, &tmp_real, 8, 1, 3);
}
