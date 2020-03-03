#pragma once

/// ## Mallet

/// `Mallet` allows you to implement a mallet or hammer model that can be used
/// to excite models.

class Mallet
{
    public:
    /// ### Public variables

    float fs;   // The sample rate (usually 44100).
    float k;    // The sample period (1/fs).
    /// are the basic variable we need to implement our physics
    /// simulation.

    float mass = 0.01;      // The mass of the mallet, kg.
    float stiffness = 5000; // The sitffness of the mallet, unitless.
    float alpha = 1.2;      // Non-linear factor that determines how the mallet
                            // Compresses.
    /// are the variables determining the physical parameters of the mallet.

    float u = -0.1;     // The current position of the mallet, m.
    float up = -0.1;    // The previous position of the mallet, m.
    /// are the state variables of our system.

    float force;
    /// determines forces applied to the hammer.

    /// ### Functions

    Mallet(float sampleRate = 44100);
    /// Constructs a new mallet instance.

    void compute();
    /// Applies all added forces to the mallet.

    float computeAndApplyImpactForce(float uOther);
    /// Computes the force occuring due to hammer impact with another object
    /// a position `uOther`. The computed force is returned and is applied to
    /// the hammer at the same time.

    void trigger(float position = 0.1, float velocity = 1.0);
    /// Trigger the mallet to start moving from the given `position` with the
    /// given `velocity`.
};