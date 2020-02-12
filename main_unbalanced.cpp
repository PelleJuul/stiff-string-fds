#include <pal.h>

#define pow2(x) (x * x)

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

class Material
{
    public:
    float density;
    float width;
    float height;
    float length;
    float radius;
};

class Domain
{
    public:
    virtual int getNumIndices();
    virtual float &at(int i);
    virtual Domain *previous(int i);
    virtual Domain *force(int i);
    virtual Domain *temp(int i);
    virtual float calculateMass(const Material &m);
};

class Domain1d : public Domain
{
    public:
    float u;
    Domain1d *up;
    Domain1d *un;
    Domain1d *f;

    int getNumIndices() { return 1; }

    float &at(int i) { return u; }
    Domain *previous(int i) { return up; }
    Domain *force(int i) { return f; }
    Domain *temp(int i) { return un; }

    float calculateMass(const Material &m)
    {
        return 0.001;
    }
};

/*
class Model
{
    Material *material;
    Domain *domain;

    float mass; //kg;

    Model(Material *material, Domain *domain)
    {
        this->material = material;
        this->domain = domain;

        // How the mass is computed depends on the domain.
        mass = domain->calculateMass(*material);
    }
};
*/

float noise()
{
    return 2 - 2.0 * ((rand() % 10000) / 10000.0);
}

class Model0d
{
    public:
    float u = 0;
    float up = 0;
    float un = 0;
    float forces = 0; // Applied force
    float mul = 1.0; // Multiplier

    float mass = 0.001; // kg;

    float fs;
    float k;

    Model0d(float fs)
    {
        this->fs = fs;
        k = 1.0 / fs;
    }

    float &force() { return forces; }
    float &multiplier() { return mul; }

    void addSpringForce(float u0, float c)
    {
        forces -= (1.0 / mass) * c * pow2(k) * (u - u0);
    }

    void addNonLinearSpringForce(float u0, float c)
    {
        forces -= powf(c, 4) * pow2(k) * powf(u - u0, 3);
    }

    void addUnbalancedSpringForce(float u0, float c, float b)
    {
        forces -= (1.0 / mass) * c * pow2(k) * sinh(b * (u - u0)) / sinh(b);
    }

    void addDampening(float sigma0)
    {
        forces += k * sigma0 * up;
        mul *= 1.0 / (1.0 + k * sigma0);
    }

    void addBowForce(float vb, float fb, float a)
    {
        float eta = fs * (u - up) - vb;
        float c = -(1 / mass) * fb * a * eta;
        float f = eta > 0 ? c : -c;
        forces += pow2(k) * f;
    }

    void compute()
    {
        un = mul * forces + 2 * u - up;

        up = u;
        u = un;

        forces = 0;
        mul = 1.0;
    }
};

/*
/// Spring connection between model and constante
void springConnection(Model *model, float u2, float k)
{
    for (int i = 0; i < model.domain.numIndices(); i++)
    {
        model.addForce(i, k (model->at(i) - u2));
    }
}
*/

int main(int argc, char **argv)
{
    Scope scope(1000);
    Model0d m1(44100);
    m1.u = 0;
    m1.up = 0;
    float c = 0.0;
    float vb = 0;
    float fb = 0.2;
    float p = 60;
    float b = 0.5;
    float f = pitchToFreq(p);
    float k = pow2(2 * M_PI * f) * 0.001;

    Realtime rt = quickAudio([&](int n, int nc, float *in, float *out)
    {
        for (int i = 0; i < n; i++)
        {
            m1.addNonLinearSpringForce(0, 2 * M_PI * f);
            // m1.addSpringForce(0, k);
            m1.addBowForce(vb, fb, 0.1);
            m1.addDampening(10);

            m1.compute();
            float y = 1000 * (m1.u);
            scope.write(y);

            for (int c = 0; c < nc; c++)
            {
                // Write it to the output array here.
                out[i * nc + c] = y;
            }
        }
    });

    quickGui([&]()
    {
        ImGui::Begin("New PAL project");
        ImGui::SliderFloat("c", &c, 0.0, 1.0);
        ImGui::SliderFloat("p", &p, 60, 60+24);
        ImGui::SliderFloat("Fb", &fb, 0, 0.2);
        ImGui::SliderFloat("Vb", &vb, 0, 2);
        ImGui::SliderFloat("b", &b, 0, 20);
        scope.draw();
        ImGui::End();

        f = pitchToFreq(p);
        // fb = map(c, 0, 1, 0, 0.15);
        // vb = c;
        k = pow2(2 * M_PI * f) * 0.001;
    });

    rt.stop();

    return 0;
}