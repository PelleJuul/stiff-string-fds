#include "Mallet.h"
#include "utils.h"
#include <cmath>

Mallet::Mallet(float sampleRate)
{
    fs = sampleRate;
    k = 1.0 / sampleRate;
}

void Mallet::compute()
{
    float un = force + 2 * u - up;

    up = u;
    u = un;
    force = 0;
}

float Mallet::computeAndApplyImpactForce(float uOther)
{
    float diff = u - uOther;
    diff = diff > 0 ? diff : 0;
    float f = stiffness * powf(diff, alpha);
    force -= (1 / mass) * pow2(k) * f;
    return f;
}

void Mallet::trigger(float position, float velocity)
{
    u = -position;
    up = u - k * velocity;
}