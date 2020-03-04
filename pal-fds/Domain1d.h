#pragma once

#include <vector>

class Domain1d
{
    public:
    Domain1d(int size);

    __attribute__((always_inline)) inline double at(int l)
    {
        return v[l+2];
    }

    double &ref(int l);

    void clear(double value = 0);

    /// Compute the first order forwards difference.
    /// @param  l   The point to evaluate at.
    double dxf(int l);

    double dxx(int l);
    
    double dxxxx(int l);

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