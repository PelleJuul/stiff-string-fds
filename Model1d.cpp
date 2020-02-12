#include "Model1d.h"
#include "utils.h"
#include <cmath>

Model1d::Model1d(int N, float fs) :
    ua(N),
    ub(N),
    uc(N),
    u(ua),
    up(ub),
    un(uc),
    forces(N),
    multiplier(N)
{
    this->fs = fs;
    k = 1.0 / fs;

    this->N = N;
    h = 1.0 / N;
}

void Model1d::addContinuousBowForce(int i, float vb, float fb, float alpha)
{
    float eta = fs * (u.at(i) - up.at(i)) - vb;
    float f = N * (1 / linearDensity) * fb * sqrt(2 * alpha) * eta * exp(-alpha * pow2(eta) + 0.5);
    forces.at(i) -= pow2(k) * f;
}

void Model1d::addDampening(float sigma0)
{
    for (int i = 0; i < N; i++)
    {
        forces.at(i) += sigma0 * k * up.at(i);
        multiplier.at(i) *= 1.0 / (1 + k * sigma0);
    }
}

void Model1d::addExponentialBowForce(int i, float vb, float fb, float a, float epsilon)
{
    float eta = fs * (u.at(i) - up.at(i)) - vb;
    float f = N * (1 / linearDensity) * fb * sgn(eta) * (epsilon + (1 - epsilon) * exp(-a * fabs(eta)));
    forces.at(i) -= pow2(k) * f;
}

void Model1d::addExternalForce(int i, float force)
{
    forces.at(i) += N * (1.0 / linearDensity) * pow2(k) * force;
}

static float mapRange(float x, float oldMin, float oldMax, float newMin, float newMax)
{
    float r = (x - oldMin) / (oldMax - oldMin);
    return newMin + r * (newMax - newMin);
}

void Model1d::addStiffness()
{
    float kappa = sqrt(E * momentOfInertia / linearDensity);
    addStiffness(kappa);
}

void Model1d::addStiffness(float kappa)
{
    for (int i = 0; i < N; i++)
    {
        forces.at(i) -= pow2(kappa) * pow2(k) * u.dxxxx(i);
    }
}

void Model1d::addTanhBowForce(int i, float vb, float fb, float a)
{
    float eta = fs * (u.at(i) - up.at(i)) - vb;
    float c = N * (1 / linearDensity) * fb * tanh(a * eta); //; fabs(eta);
    forces.at(i) += pow2(k) * c;
}

void Model1d::addTensionFreq(float freq)
{
    for (int i = 0; i < N; i++)
    {
        forces.at(i) += pow2(freq) * pow2(k) * u.dxx(i);
    }
}

void Model1d::calculateDerivedParameters()
{
    area = M_PI * r * r;
    mass = p * area * length;
    momentOfInertia = M_PI * r * r * r * r / 4;
    linearDensity = p * area;
}

void Model1d::compute()
{
    for (int i = 0; i < N; i++)
    {
        un.at(i) = multiplier.at(i) * (forces.at(i) + 2 * u.at(i) - up.at(i));
    }

    Domain1d &swap = up;
    up = u;
    u = un;
    un = swap;

    forces.clear();
    multiplier.clear(1);
}