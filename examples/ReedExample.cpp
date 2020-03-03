#include "pal/pal.h"
#include "../pal-fds/Reed.h"
#include "../pal-fds/Model1d.h"

int main(int argc, char **argv)
{
    Model1d string(100);
    Reed reed;

    Realtime rt = quickAudio([&](int n, int nc, float *in, float *out)
    {
        for (int i = 0; i < n; i++)
        {
            // Compute your sample here.
            float y = 0;

            string.u.prepareClampedBoundaryRight();
            // string.u.prepareClampedBoundaryLeft();

            string.addReedForce(&reed, 200, 0.0134);
            string.addTensionFreq(200);
            string.addDamping(0.1);

            string.compute();

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
        ImGui::TextWrapped(
            "Congratulations, you have successfully compiled a new pal project."
            "To start developing your application edit the 'main.cpp' file "
            "which already contains the code necessary for this UI and for "
            "real time audio. To see all UI widgets available through ImGui "
            "look at the source code for ImGui::ShowDemoWindow in pal/imgui/"
            "imgui_demo.cpp.\n\nHave a great time using pal! If you find a bug "
            "or have questions please post an issue on github.com/PelleJuul/pal"
        );

        ImGui::PlotLines(
            "String",
            [](void *data, int idx) { return (float)((double*)data)[idx]; }, string.u.data(),
            string.u.size() + 4,
            0,
            "",
            -0.01,
            0.01,
            ImVec2(0, 120));

        ImGui::End();
        ImGui::ShowDemoWindow();
    });

    rt.stop();

    return 0;
}