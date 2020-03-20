#pragma once

#include <vector>

class Domain1d
{
    public:
    Domain1d(int size);

    __attribute__((always_inline)) inline double at(int l) const
    {
        return v[l+2];
    }

    double &ref(int l);

    void clear(double value = 0);

    /// Compute the first order forwards difference.
    /// @param  l   The point to evaluate at.
    double dxf(int l) const;

    double dxx(int l) const;

    double dxxi(float p) const; 
    
    double dxxxx(int l) const;

    float interpolate(float p) const;
    /// Get the linearly interpolated point at fractional index `p` where
    /// `0 <= p < N - 1`.

    void prepareClampedBoundaryLeft();
    void prepareClampedBoundaryRight();

    void prepareFreeBoundaryLeft();
    void prepareFreeBoundaryRight();

    size_t size()
    {
        return L;
    };

    double *data()
    {
        return v.data();
    };

    private:
    float L = 0;
    float L2 = 0;
    std::vector<double> v;
};