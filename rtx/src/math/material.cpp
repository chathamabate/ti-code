
#include "./material.h"
#include "./vec.h"

using namespace math;

static Material DEF_MATERIAL_VAL(
        Vec3D(0, 0, 0), Vec3D(1, 1, 1), Vec3D(1, 1, 1), 5);

Material *Material::DEF_MATERIAL = &DEF_MATERIAL_VAL;
