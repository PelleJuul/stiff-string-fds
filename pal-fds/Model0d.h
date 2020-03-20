#pragma once

/// ## Model0d

/// `Model0d` allows you to model forces acting on a point like mass that can
/// move in one dimension, e.g. up and down.
/// It is basically an implementation of Newtons second law of physics
/// $$
///     a = F / m,
///     F = ma
/// $$
/// where `a` is the the acceleration of the object, `F` are the applied forces,
/// and `m` is the mass.
/// Various functions are provided to quickly implement common forces
class Model0d
{
    public:

    /// ### Variables

    /// These are the basic variable we need to implement our physics
    /// simulation.
    float fs;   // The sample rate (usually 44100).
    float k;    // The sample period (1/fs).

    /// These are the variables describing the state of the mass.
    float u = 0;    // The current position of the mass (in meters).
    float up = 0;   // The previous position of the mass (m).
    float un = 0;   // The "next" position of the mass.
    /// `un` is used in the `compute` function to update `u` and `up` and should
    /// not be touched elsewhere.

    /// This is where we specify the mass of the system.
    float mass = 0.001; // The mass (kg).

    /// These variables describe the forces acting on the mass that cause it to
    /// move.
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
    float forces = 0;
    float mul = 1.0;

    /// ### Functions

    Model0d(float fs = 44100);
    /// Is the constructor for the `Model0d` cass. `fs` is the sample rate.

    virtual void addBarrierCollision(float b, float K, float alpha);
    /// Adds a collision with a rigid barrier *below* the mass where `b` is
    /// the position of the barrier, `K` is the stiffness, and `alpha` is a
    /// non-linear exponent of the collision. 

    void addExternalForce(float f);
    /// Adds an external force to the mass.
    
    void addSpringForce(float u0, float c);
    /// Adds a linear spring force based on Hooke's law to the mass:
    /// ```
    ///     F = -(1/m) * c * (u - u0)
    /// ```
    /// `u0` is the other point the spring should be tied to (e.g. 0 if you
    /// wish to implement a simple harmonic oscillator).
    /// `c` is the spring constant.

    void addSpringForceFreq(float u0, float f);
    /// Works like `addSpringForce`, but takes the desired frequency `f` as a
    /// parameter instead of the spring constant.

    void addNonLinearSpringForce(float u0, float c);
    /// Adds a non-linear, siffening or hard, spring force to the system using
    /// the equation.
    /// ```
    ///     F = -(1/m) * c * (u - u0)^3.
    /// ```

    void addNonLinearSpringForceFreq(float u0, float f);
    /// Adds a non-linear, siffening or hard, spring force to the system using
    /// the equation.
    /// ```
    ///     F = -(1/m) * c * (u - u0)^3.
    /// ```

    void addDamping(float sigma0);
    /// Adds dampening to the system causing an exponential decay.

    void addBowForce(float vb, float fb, float a = 10, float epsilon = 0.1);
    /// Adds a force to the system similar to a violin bow.
    /// `vb` is the speed of the bow (m/s).
    /// `fb` is force in Newtons at which the bow is pressed down onto the mass.
    /// `a` and `epsilon` controls the character of the bow, see Bilbao's
    /// Numerical Sound Synthesis chapter 4.3.1.

    float computeForPoint();
    /// Compute the next value of `u` at the specified position `i` without
    /// comitting the change to the state. This is used for computing solutions
    /// using the Newton-Rhapson method.

    virtual void compute();
    /// Applies all the added forces to the mass and updates the state `u`.
    /// This should be called every time we want to produce a new sample.
    /// Audio output can be achieved by reading `u`.
};