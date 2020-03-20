#include "utils.h"
#include "Model0d.h"
#include <cmath>

Model0d::Model0d(float fs)
{
    this->fs = fs;
    k = 1.0 / fs;
}

void Model0d::addBarrierCollision(float b, float K, float alpha)
{
    forces += (1.0 / mass) * pow2(k) * K * powf(pos(b - u), alpha + 1);
}

void Model0d::addExternalForce(float f)
{
    forces += (1.0 / mass) * pow2(k) * f;
}

void Model0d::addSpringForce(float u0, float c)
{
    forces -= (1.0 / mass) * c * pow2(k) * (u - u0);
}

void Model0d::addSpringForceFreq(float u0, float f)
{
    float omega = 2 * M_PI * f;
    forces -= pow2(omega) * pow2(k) * (u - u0);
}

void Model0d::addNonLinearSpringForce(float u0, float c)
{
    forces -= (1.0 / mass) * c * pow2(k) * powf(u - u0, 3);
}

void Model0d::addNonLinearSpringForceFreq(float u0, float f)
{
    float omega = 2 * M_PI * f; 
    forces -= pow2(pow2(omega)) * pow2(k) * powf(u - u0, 3);
}

void Model0d::addDamping(float sigma0)
{
    forces += k * sigma0 * up;
    mul *= 1.0 / (1.0 + k * sigma0);
}

void Model0d::addBowForce(float vb, float fb, float a, float epsilon)
{
    float eta = fs * (u - up) - vb;
    float f = (1 / mass) * fb * sgn(eta) * (epsilon + (1 - epsilon) * exp(-a * fabs(eta)));
    forces += pow2(k) * f;
}

void Model0d::compute()
{
    un = mul * (forces + 2 * u - up);

    up = u;
    u = un;

    forces = 0;
    mul = 1.0;
}

float Model0d::computeForPoint()
{
    return mul * (forces + 2 * u - up);
}