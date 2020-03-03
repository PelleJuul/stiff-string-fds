#include "Material.h"

Material Material::createSteelMaterial()
{
    Material m;
    m.E = 200e6;
    m.p = 8000;

    return m;
}

Material Material::createAirMaterial()
{
    Material m;
    m.E = 200e6;
    m.p = 1.225;

    return m;
}