#include <pal.h>
#include "../Model1d.h"

int main(int argc, char **argv)
{
    Realtime rt = quickAudio([&](int n, int nc, float *in, float *out)
    {
        Scope scope(1000);
        Model0d m(44100);
        m.up = 0.0;

        for (int i = 0; i < n; i++)
        {
            m.addSpringForceFreq(0, 440);
            m.addDampening(1);
            m.addBowForce(0.2, 1, 10, 0.1);
            m.compute();

            // Compute your sample here.
            float y = m.u;
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
        scope.
        ImGui::End();
    });

    rt.stop();

    return 0;
}