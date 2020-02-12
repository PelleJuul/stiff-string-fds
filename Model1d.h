#pragma once

/// ## Model1d

/// `Model1d` allows you to model forces acting on an 1D domain --- a line of
/// values indexed by an $x$ coordinate.
/// It is based on a distributed version of Newtons second law of physics
/// $$
///     a(x) = F(x) / m,
///     F(x) = m a(x)
/// $$
/// where `a(x)` is the acceleration a point $x$ of the domain, `F(x)` is to
/// forces acting on that point and `m` is the mass of the entire domain.
/// Various functions are provided to implement common forces and to handle
/// common types of boundary conditions.

/// The Model1d class utilizes the following headers
#include "Domain1d.h"   // Handle low-level datails of the 1d domain.

class Model1d
{
    public:

    /// ### Public Variables

    float fs;   // The sample rate (usually 44100).
    float k;    // The sample period (1/fs).
    float h;    // The point spacing.
    int N;      // The number of points in the domain.
    /// are the basic variable we need to implement our physics
    /// simulation.

    float E = 200e6;    // Young's modulus, Pa (N / m^2) (steel)
    float p = 8000;     // Density, kg / m^3 (steel)
    float r = 0.0003;   // Radius, m
    float length = 0.3; // Length, m
    /// describe the material and shape of the domain.

    float area;            // Cross-sectional area, m^2
    float momentOfInertia; // Moment of inertia, kg m^2
    float mass;            // Mass, kg
    float linearDensity;   // Linear density, kg / m
    /// are variables derived from the above parameters and should
    /// not be set by hand. One should call `calculateDerivedParameters` before
    /// accessing these.

    Domain1d &u;    // The current displacement of the domain (m).
    Domain1d &up;   // The previous displacement of the domain (m).
    Domain1d &un;   // The "next" displacement of the domain (m).
    /// describe the state of the system.
    /// `un` is used in the `compute` function to update `u` and `up` and should
    /// not be touched elsewhere.
    /// These should be accessed prior to calling compute to to call a
    /// `prepare*Boundary*` for both left and right.

    Domain1d forces;
    Domain1d multiplier;
    /// describe the forces acting on the mass that cause it to move.
    /// Everytime `compute` is called the forces and multipliers are used to
    /// update `u`.
    /// The update uses the equation:
    /// ```
    ///     mul * (forces + 2 * u - up).
    /// ```
    /// which can be used the generalize many different kinds of discretized
    /// forces.
    /// To better understand how they are used look at Stefan Bilbao's Numerical
    /// sound synthesis chapter 3.5 and compare it to the implementations of
    /// `addSpringForceFreq` and `addDamping`.

    /// ### Functions

    Model1d(int N, float fs=44100);
    /// Is he constructor for the `Model1d` class, where `fs` is the sample
    /// rate and `N` is the number of points in the domain.

    void addContinuousBowForce(int i, float vb, float fb, float alpha);
    /// Adds a continuous bow force where `vb` is the bow speed, `fb` is the
    /// bow force, and `alpha` determinse the shape of the friction curve.

    void addDamping(float sigma0);
    /// Adds a frequency independent dampening to the system where `sigma0` is
    /// the amount of dampening.

    void addExponentialBowForce(int i, float vb, float fb, float a, float epsilon);
    /// Adds en exponential bow force where `vb` is the bow speed, `fb` is the
    /// bow force, and `a` and `epsilon` determine the shape of the friction
    /// curve.

    void addExternalForce(int i, float force);
    /// Adds an external force `force` e.g. a mallet or other exciter at
    /// point `i`.

    void addFrequencyDependentDamping(float sigma1);
    /// Adds a dampening force that cause higher frequencies to die out faster
    /// than lower ones. `sigma1` controls the amount of dampening.

    void addStiffness();
    /// Adds a stiffness force arising from the material variables that will.

    void addStiffness(float kappa);
    /// Adds a stiffness force corresponding to the given stiffness.

    void addTanhBowForce(int i, float vb, float fb, float a);
    /// Adds a linear bowing force with bow speed and point `i`, `vb` and bow 
    /// force `fb` to the domain.

    void addTensionFreq(float freq);
    /// Adds a tension force that will result in oscillations of the given
    /// frequency.

    void calculateDerivedParameters();
    /// Calculate all derived paramters (mass, area, moment of inertia, etc).
    /// Should be called before adding any forces.

    void compute();
    /// Applies all the added forces to the mass and updates the state `u`.
    /// This should be called every time we want to produce a new sample.
    /// Audio output can be achieved by reading `u`.

    private:
    /// ### Private Variables

    Domain1d ua;
    Domain1d ub;
    Domain1d uc;
    /// The variables are used to do *reference cycling* to improve the
    /// efficiency of the `compute` function.
};