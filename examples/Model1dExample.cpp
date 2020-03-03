#include "pal/pal.h"
#include <cmath>
#include "../pal-fds/Mallet.h"
#include "../pal-fds/Model1d.h"
#include "../pal-fds/Material.h"

// pal build Model1dExample.cpp ../Model1d.cpp ../Domain1d.cpp ../Mallet.cpp -O3 -o Model1dExample

int main(int argc, char **argv)
{
    bool pluck = false;
    bool step = false;
    bool mute = false;
    int bowSelect = 3;

    Model1d string(100);
    string.setMaterial(Material::createSteelMaterial());
    Mallet mallet;

    Adsr forceAdsr(0.3, 0.2, 1.0, 0.2);
    Adsr velocityAdsr(0.15, 0.2, 1.0, 0.4);

    float dampening = 1;
    float freqDepDampening = 5;

    float wavespeed = 220;
    float stiffness = 1;
    float speed = 1.0;
    float bowSpeed = 0.2;
    float bowForce = 0.0;
    float bowPosition = 0.15;
    float bowAlpha = 50.0;
    float bowEpsilon = 0.1;
    float hammerForce = 0;
    float malletStrikeSpeed = 1.0;

    int speedCount = 0;

    Realtime rt = quickAudio([&](int n, int nc, float *in, float *out)
    {
        for (int i = 0; i < n; i++)
        {
            speedCount++;
            float y = 0;

            if ((speed > 0 && speedCount >= 1.0 / speed) || step)
            {
                speedCount = 0;
                step = false;

                if (pluck)
                {
                    for (int i = 5; i < 10; i++)
                    {
                        string.addExternalForce(
                            i,
                            pow(sin(M_PI * i / 10.0), 2));
                    }

                    pluck = false;
                }

                string.calculateDerivedParameters();
                string.u.prepareClampedBoundaryLeft();
                string.u.prepareClampedBoundaryRight();

                string.addTensionFreq(wavespeed);
                string.addStiffness();
                string.addDamping(dampening);
                string.addFrequencyDependentDamping(
                    powf(10, -freqDepDampening));

                hammerForce = mallet.computeAndApplyImpactForce(
                    string.u.at(10));

                string.addExternalForce(10, hammerForce);

                bowSpeed = 0.2 * velocityAdsr.next();
                bowForce = 0.4 * forceAdsr.next();

                switch (bowSelect)
                {
                case 0:
                    string.addTanhBowForce(
                        bowPosition * string.u.size(),
                        bowSpeed,
                        bowForce,
                        bowAlpha);
                    break;
                case 1:
                    string.addExponentialBowForce(
                        bowPosition * string.u.size(),
                        bowSpeed,
                        bowForce,
                        bowAlpha,
                        bowEpsilon);
                    break;
                case 2:
                    string.addContinuousBowForce(
                        bowPosition * string.u.size(),
                        bowSpeed,
                        bowForce,
                        bowAlpha);
                    break;
                case 3:
                    string.addContinuousNowtonRaphsonBowForce(
                        bowPosition * string.u.size(),
                        bowSpeed,
                        bowForce,
                        bowAlpha);
                    break;
                
                default:
                    break;
                }

                string.compute();
                mallet.compute();

                // Compute your sample here.
                y = 100 * string.u.at(30);
            }

            if (abs(y) > 1.5)
            {
                mute = true;
            }

            if (mute)
            {
                y = 0;
            }

            for (int c = 0; c < nc; c++)
            {
                // Write it to the output array here.
                out[i * nc + c] = y;
            }
        }
    });

    quickGui([&]()
    {
        using namespace ImGui;

        Begin("New PAL project");
        Text("Hello, world!");

        SliderFloat("Wave speed", &wavespeed, 0, 440);
        SliderFloat("Stiffness", &stiffness, 0, 8);
        InputFloat("Dampening", &dampening, 0.5, 1);
        InputFloat("Freq. dampening", &freqDepDampening, 0.5, 1);

        InputFloat("Speed", &speed, 0.001, 0.1, 3);

        if (Button("Step"))
        {
            step = true;
        }

        if (Button("Pluck"))
        {
            pluck = true;
        }

        Checkbox("Mute", &mute);

        if (Button("Reset"))
        {
            string.u.clear();
            string.up.clear();
            string.un.clear();
        }

        PlotLines(
            "String",
            [](void *data, int idx) { return (float)((double*)data)[idx]; }, string.u.data(),
            string.u.size() + 4,
            0,
            "",
            -0.01,
            0.01,
            ImVec2(0, 120));

        End();

        Begin("Exciters");
        if (CollapsingHeader("Bows"))
        {
            RadioButton("Linear Bow", &bowSelect, 0);
            RadioButton("Exponential Bow", &bowSelect, 1);
            RadioButton("Contiunous Bow", &bowSelect, 2);
            RadioButton("Contiunous NR Bow", &bowSelect, 3);

            SliderFloat("Bow speed", &bowSpeed, 0, 1);
            SliderFloat("Bow force", &bowForce, 0, 10);
            SliderFloat("Bow position", &bowPosition, 0, 1);
            InputFloat("Bow alpha", &bowAlpha, 0.01, 0.1);
            InputFloat("Bow epsilon", &bowEpsilon, 0.01, 0.1);

            if (Button("Trigger"))
            {
                if (forceAdsr.isTriggered)
                {
                    forceAdsr.release();
                    velocityAdsr.release();
                }
                else
                {
                    forceAdsr.trigger();
                    velocityAdsr.trigger();
                }
                
            }
        }

        if (CollapsingHeader("Mallet"))
        {
            if (Button("Strike"))
            {
                mallet.trigger(0.1, malletStrikeSpeed);
            }

            ImGui::InputFloat("Stiffness", &mallet.stiffness, 10, 100);
            ImGui::InputFloat("Mass", &mallet.mass, 0.01, 0.1);
            ImGui::InputFloat("Compression", &mallet.alpha, 0.5, 1);
            ImGui::InputFloat("Strike speed", &malletStrikeSpeed, 0.5, 1.0);

            LabelText("Mallet position", "%.2f", mallet.u);
            LabelText("Hammer force", "%.2f", hammerForce);
        }

        End();

        Begin("Force Envelope");
        forceAdsr.draw();
        End();

        Begin("Velocity Envelope");
        velocityAdsr.draw();
        End();
    });

    rt.stop();

    return 0;
}