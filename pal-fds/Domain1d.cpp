#include "Domain1d.h"
#include <cmath>

Domain1d::Domain1d(int size) :
    v(size+4, 0)
{
    L = size;
    L2 = L * L;
}

double &Domain1d::ref(int l)
{
    return v.at(l+2);
}

void Domain1d::clear(double value)
{
    for (int i = 0; i < v.size(); i++)
    {
        v[i] = value;
    }
}

double Domain1d::dxf(int l) const
{
    return L * (at(l+1) - at(l));
}

double Domain1d::dxx(int l) const
{
    return L2 * (at(l + 1) - 2 * at(l) + at(l - 1));
}

double Domain1d::dxxi(float l) const
{
    return L2 * (interpolate(l + 1) - 2 * interpolate(l) + interpolate(l - 1));
}

double Domain1d::dxxxx(int l) const
{
    return L2 * L2 * (at(l+2) - 4 * at(l+1) + 6 * at(l) - 4 * at(l-1) + at(l-2));
}

float Domain1d::interpolate(float p) const
{
    int i1 = floor(p);
    int i2 = ceil(p);
    float a = p - i1;
    return (1 - a) * at(i1) + a * at(i2);
}

void Domain1d::prepareClampedBoundaryLeft()
{
    ref(-2) = at(0);
    ref(-1) = 0;
}

void Domain1d::prepareClampedBoundaryRight()
{
    ref(L) = 0;
    ref(L+1) = at(L-1);
}

void Domain1d::prepareFreeBoundaryLeft()
{
    ref(-2) = 3 * at(0) - 2 * at(1);
    ref(-1) = 2 * at(0) - at(1);
}

void Domain1d::prepareFreeBoundaryRight()
{
    ref(L) = 2 * at(L-1) - at(L-2);
    ref(L+1) = 3 * at(L-1) - 2 * at(L-2);
}

