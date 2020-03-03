#include "Model1d.h"
#include <cmath>
#include <exception>
#include "Reed.h"
#include "utils.h"

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

void Model1d::addContinuousNowtonRaphsonBowForce(int lb, float vb, float fb, float alpha)
{
    // Start by approximating the relative velocity using the backwards difference.
    float vrel = fs * (u.at(lb) - up.at(lb)) - vb;
    float nh = N * (1 / linearDensity);
    float force = 0;
    float delta = 1;
    float c = 1.0 / (2 * k);

    for (int i = 0; i < 50 && fabs(delta) > 10e-4; i++)
    {
        // Save previous force
        float fOld = forces.at(lb);

        // Add the estimated bow force to the forces
        force = pow2(k) * nh * fb * sqrt(2 * alpha) * vrel * exp(-alpha * pow2(vrel) + 0.5);
        forces.at(lb) -= force; 

        // Compute the numerator of the Newton-Raphson step
        float num = c * computeForPoint(lb) - c * up.at(lb) - vb - vrel;

        // Restore force
        forces.at(lb) = fOld;

        // Compute denominator of NR stepA
        float thetad =
            sqrt(2 * alpha) * (exp(-alpha * pow2(vrel) + 0.5)
                - 2 * alpha * pow2(vrel) * exp(-alpha * pow2(vrel) + 0.5));
        float denom = -c * multiplier.at(lb) * pow2(k) * nh * fb * thetad - 1;

        float delta = (num / denom);
        vrel = vrel - delta;
    }

    forces.at(lb) -= force;
}

void Model1d::addDamping(float sigma0)
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

void Model1d::addFrequencyDependentDamping(float sigma1)
{
    for (int i = 0; i < N; i++)
    {
        forces.at(i) += k * 2 * sigma1 * (u.dxx(i) - up.dxx(i));
    }
}

void Model1d::addReedForce(Reed *reed, float wavespeed, float pm)
{
    // Compute intermediate variables
    float gamma = wavespeed / length;

    float O = (p * pow2(wavespeed) * reed->Sr) / (reed->Mr * reed->H0);
    float R = sqrtf(2.0) * ((reed->w * reed->H0) / S0);
    float S = (reed->Sr * reed->H0) / (wavespeed * S0);

    float a1 = -(2.0 / k) * reed->dtb() 
             + pow2(reed->omega0) * reed->yp
             - powf(reed->omega1, reed->alpha + 1) * (reed->yp + 1)
               * powf(fabs(neg(reed->y + 1)), reed->alpha - 1);
    float a2 = (2.0 / k)
             + 2 * reed->sigma0
             + k * pow2(reed->omega0)
             - k * powf(reed->omega1, reed->alpha + 1)
               * powf(fabs(neg(reed->y + 1)), reed->alpha - 1);

    float b1 = (a2 / (S * O)) * R * pos(reed->y + 1);
    float b2 = a1 / O;
    float b3 = a2 / (S * O);

    float c1 = -(h / (k * gamma));
    float c2 = (h / (k * gamma)) * pm
             - (h / (k * pow2(gamma))) * fs * (u.at(0) - up.at(0))
             - u.dxf(0);

    float d1 = b1 / (1 - b3 * c1);
    float d2 = (b2 - b3 * c2) / (1 - b3 * c1);

    // Do sanity checks
    if (a2 < 0)
    {
        printf("a2 was less than zero\n");
    }

    if (b1 < 0)
    {
        printf("b1 was less than zero\n");
    }

    if (b3 < 0)
    {
        printf("b3 was less than zero\n");
    }

    if (c2 < 0)
    {
        printf("c2 was less than zero\n");
    }

    if (d1 < 0)
    {
        printf("d1 was less than zero\n");
    }

    // Compute p_delta
    float sqrtAbsPDelta = (-d1 + sqrt(pow2(d1) + 4 * fabs(d2))) / 2.0;
    float absPDelta = pow2(sqrtAbsPDelta);
    float pdelta = -absPDelta * sgn(d2);

    // Compute yn
    // float e1 = powf(fabs(neg(reed->y + 1)), reed->alpha - 1);
    // float e2 = 0.5 * pow2(k) * powf(reed->omega1, reed->alpha + 1) * e1;
    // float e3 = 0.5 * pow2(k) * pow2(reed->omega0);
    // float e4 = (1 + e3 + k * reed->sigma0 - e2);
    // float e5 =(-e3 + k * reed->sigma0 + e2 - 1);
    // float yn = (1.0 / e4) * (
    //       reed->yp * e5
    //     + pow2(k) * powf(reed->omega1, reed->alpha + 1)
    //     - pow2(k) * O * pdelta
    //     + 2 * reed->y);
    float eta = powf(fabs(neg(reed->y + 1)), reed->alpha - 1);
    float yn = (-O*pdelta*pow2(k) + 0.5*eta*reed->yp*pow2(k)*powf(reed->omega1, reed->alpha + 1) + 1.0*eta*pow2(k)*powf(reed->omega1, reed->alpha + 1) + k*reed->sigma0*reed->yp + 2*reed->y - 0.5*reed->yp*pow2(k)*pow2(reed->omega0) - reed->yp)*powf(-0.5*eta*pow2(k)*powf(reed->omega1, reed->alpha + 1) + k*reed->sigma0 + 0.5*pow2(k)*pow2(reed->omega0) + 1, -1);
    float actualY = yn * reed->H0 + reed->H0;

    reed->yp = reed->y;
    reed->y = yn;

    // Compute p_in
    float pin = pm - pdelta;

    // Compute un[0]
    u.at(-1) = 2 * k * gamma * pin - up.at(-1);
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

float Model1d::computeForPoint(int i)
{
    return multiplier.at(i) * (forces.at(i) + 2 * u.at(i) - up.at(i));
}

void Model1d::setMaterial(const Material &material)
{
    E = material.E;
    p = material.p;
    calculateDerivedParameters();
}