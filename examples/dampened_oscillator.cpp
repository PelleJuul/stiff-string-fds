#include <pal.h>
#include "../Model1d.h"

// pal build dampened_oscillator.cpp ../Model1d.cpp -O3 -o dampened_oscillator

int main(int argc, char **argv)
{
    Scope scope(1000);
    Model0d m(44100);
    m.u = 1.0;
    m.up = 1.0;

    Realtime rt = quickAudio([&](int n, int nc, float *in, float *out)
    {
        for (int i = 0; i < n; i++)
        {
            m.addSpringForceFreq(0, 440);
            m.addDamping(3);
            m.compute();

            // Compute your sample here.
            float y = m.u;
            // printf("%f\n", y);
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
        ImGui::Text("Hello, world!");
        scope.draw();
        ImGui::End();
    });

    rt.stop();

    return 0;
}