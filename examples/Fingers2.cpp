#include "pal/pal/pal.h"
#include "../pal-fds/pal-fds.h"
#include <vector>
#include <algorithm>
#include "zita-rev1/reverb.h"

enum FingerState
{
    /// Finger is moving down, but has not yet collided with the string.
    FingerMoving,
    /// The finger has touched the string.
    FingerDown,
    /// The finger has beed released.
    FingerReleased
};

class Finger : public Model0d
{
    public:
    float K = 1e5;
    float pos = 1.0;
    float alpha = 3;
    FingerState state = FingerState::FingerReleased;
    float moveForce = 5.0;
    float downForce = 100.0;
    float ep = 0;
    int id = 0;

    Finger(float fs = 44100) : Model0d(fs)
    {
        u = 0.00;
        up = 0.00;
        mass = 2.0;
    };

    void addBarrierCollision(float b, float K, float alpha) override
    {
        forces += (1.0 / mass) * pow2(k) * K * powf(pos(b - u), alpha + 1);
        forces += (1.0 / mass) * pow2(k) * this->K * powf(pos(b - u), this->alpha + 1);
    }

    void collideWithString(Model1d *string)
    {
        float d = u - string->u.interpolate(pos * (string->N - 1));
        float f = K * d;
        // addExternalForce(-f);
        string->addInterpolatedForce(pos * (string->N - 1), f);
    }

    void collideWithStringSI(float gamma, float sigma_0, float sigma1, Model1d *string)
    {
        int p = floor(pos * (string->N - 1));
        float ep = string->up.at(p);
        float e = string->u.at(p);
        float k = string->k;
        float h = string->h;
        float sigma_p = 20;
        float omega_0 = 1000;
        float zeta = string->u.dxx(p);
        float en = (2.0*e*pow2(h) + 1.0*ep*k*sigma_0*pow2(h) + 1.0*ep*k*sigma_p - 1.0*ep*pow2(h) - 0.5*ep*pow2(k)*pow2(omega_0) + 1.0*zeta*pow2(gamma)*pow2(h)*pow2(k))*powf(1.0*k*sigma_0*pow2(h) + 1.0*k*sigma_p + 1.0*pow2(h) + 0.5*pow2(k)*pow2(omega_0), -1);

        if (e != 0)
        {
            printf("%f, %f, %f\n", en, e, ep);
        }

        float f = -sigma_p*(en - ep)*powf(k, -1) - (0.5*en + 0.5*ep)*pow2(omega_0);
        string->forces.ref(p) += f;
    }

    void trigger()
    {
        u = 0.00;
        up = 0.00;

        state = FingerState::FingerMoving;
    }

    void release()
    {
        state = FingerState::FingerReleased;
    }

    void compute() override
    {
        addDamping(100);
        /*

        if (state == FingerState::FingerMoving)
        {
            addExternalForce(-moveForce);
        }

        if (state == FingerState::FingerDown)
        {
            addExternalForce(-downForce);
        }

        if (state == FingerState::FingerReleased && u < 0.01)
        {
            addExternalForce(moveForce);
        }
        */

        Model0d::compute();
    }
};

float phi(float v_rel, float alpha = 100)
{
    return 1.64872127070013*sqrt(2)*v_rel*powf(alpha, 1.0/2.0)*exp(-alpha*pow2(v_rel));
}

float phid(float v_rel, float alpha = 100)
{
    return -3.29744254140026*sqrt(2)*pow2(v_rel)*powf(alpha, 3.0/2.0)*exp(-alpha*pow2(v_rel)) + 1.64872127070013*sqrt(2)*powf(alpha, 1.0/2.0)*exp(-alpha*pow2(v_rel));
}

int main(int argc, char **argv)
{
    Scope scope(44100);
    Model1d string(100);
    string.setMaterial(Material::createSteelMaterial());

    std::vector<Finger*> fingers;
    Finger finger;
    SinOsc vibrato(0.01, 4, 44100);
    float pos = 0.667; 
    float targetPos = pos;

    float f0 = pitchToFreq(60 - 12);
    float b = -0.001;

    printf("f0 = %f\n", f0);

    startMidi();
    std::vector<uint8_t> message;

    Adsr forceAdsr(0.3, 0.2, 1.0, 0.2);
    Adsr velocityAdsr(0.15, 0.2, 1.0, 0.4);
    int midiCount = 0;
    std::vector<int> notes;
    bool pause = false;
    bool step = false;
    int stepCount = 1;
    int stepSize = 20;

    Reverb reverb;
    reverb.init(44100, false);

    string.u.ref(3) = 0.001;
    string.up.ref(3) = 0.001;

    Realtime rt = quickAudio([&](int n, int nc, float *in, float *out)
    {
        while (getMidiMessage(&message))
        {
            /*
            for (auto u : message)
            {
                printf("%i ", u);
            }
            */

            if (message[0] == 144)
            {
                targetPos = f0 / pitchToFreq(message[1]);
                notes.push_back(message[1]);
                forceAdsr.trigger();
                velocityAdsr.trigger();
            }
            else if (message[0] == 128)
            {
                if (!notes.empty())
                {
                    pos = f0 / pitchToFreq(notes.back());
                    notes.pop_back();
                }

                if (notes.empty())
                {
                    forceAdsr.release();
                    velocityAdsr.release();
                }
            }

                printf("notes: %i\n", notes.size());
        }

        for (int i = 0; i < n; i++)
        {
            if (pause && !step)
            {
                continue;
            }

            stepCount--;

            if (stepCount <= 0)
            {
                stepCount = 0;
                step = false;
            }

            // Compute your sample here.

            // pos = targetPos; // 0.9 * pos + 0.1 * targetPos;
            // finger.pos = targetPos;

            string.calculateDerivedParameters();
            string.u.prepareClampedBoundaryLeft();
            string.u.prepareClampedBoundaryRight();

            /*
            string.addTensionFreq(2 * f0);
            string.addDamping(2.0);

            finger.collideWithStringSI(2 * f0, 2.0, 1e-2, &string);
            */

            Domain1d &u = string.u;
            Domain1d &up = string.up;

            float p = targetPos * string.N;
            float ep = string.up.interpolate(p);
            float e = string.u.interpolate(p);
            float sigma_0 = 1.0;
            float sigma_1 = 0.01;
            float rrho = (1 / string.linearDensity);

            float k = string.k;
            float h = string.h;
            float omega_0 = 10000;
            float zeta = u.dxxi(p);
            float epsilon = ((2 * sigma_1) / k) * (zeta - up.dxxi(p));

            float sigma_p = 400.0;
            float gamma = 2 * f0;
            float J_f = (1 / h);

            float en = (1.0*J_f*ep*k*sigma_p - 0.5*J_f*ep*pow2(k)*pow2(omega_0) + 2.0*e + 1.0*ep*k*sigma_0 - 1.0*ep + 1.0*epsilon*pow2(k) + 1.0*zeta*pow2(gamma)*pow2(k))*powf(1.0*J_f*k*sigma_p + 0.5*J_f*pow2(k)*pow2(omega_0) + 1.0*k*sigma_0 + 1.0, -1);
            float F_f = -sigma_p*(en - ep)*powf(k, -1) - (0.5*en + 0.5*ep)*pow2(omega_0);

            const int pb = floor(string.N  * 0.0918372);
            const float vb = 0.2 * velocityAdsr.next();
            const float F_d = 0.4 * forceAdsr.next();
            float v_rel = (1 / k) * (u.at(pb) - up.at(pb)) - vb;
            float delta = 1;
            float J_b = (1 / h) * (1 / string.linearDensity);
            zeta = string.u.dxx(pb);
            epsilon = ((2 * sigma_1) / k) * (zeta - up.dxx(pb));

            for (int l = 0; l < 50 && fabs(delta) > 1e-4; l++)
            {
                float delta = (-v_rel - vb + (-string.up.at(pb) + (F_d*J_b*phi(v_rel)*pow2(k) + epsilon*pow2(k) + k*sigma_0*string.up.at(pb) + 2*string.u.at(pb) - string.up.at(pb) + zeta*pow2(gamma)*pow2(k))*powf(k*sigma_0 + 1, -1))*powf(k, -1)/2)*powf(F_d*J_b*k*phid(v_rel)*powf(k*sigma_0 + 1, -1)/2 - 1, -1);
                v_rel = v_rel - delta;
            }
            
            float F_b = F_d * phi(v_rel);
            float a_f = p - floor(p);


            for (int l = 0; l < string.N; l++)
            {
                J_f = l == floor(p) ? ((1 - a_f) / h) : 0;
                J_f = l == ceil(p) ? (a_f / h) : 0;
                J_b = l == pb ? (1 / h) * rrho : 0;
                zeta = string.u.dxx(l);
                epsilon = ((2 * sigma_1) / k) * (zeta - up.dxx(l));

                string.un.ref(l) = (F_b*J_b*pow2(k) + F_f*J_f*pow2(k) + epsilon*pow2(k) + k*sigma_0*up.at(l) + 2*u.at(l) - up.at(l) + zeta*pow2(gamma)*pow2(k))*powf(k*sigma_0 + 1, -1);
            }

            Domain1d &utmp = string.up;
            string.up = string.u;
            string.u = string.un;
            string.un = utmp;
            
            
            
            /*
            string.addTensionFreq(gamma);
            string.addDamping(sigma_0);
            float bowSpeed = 0.2 * velocityAdsr.next();
            float bowForce = 0.4 * forceAdsr.next();
            string.addContinuousNowtonRaphsonBowForce(string.N * 0.09, bowSpeed, bowForce, 100);

            // finger.compute();
            string.compute();
            */

            float y = 500 * string.u.at(string.N * 0.15342);
            float yl, yr;

            float *reverbIn[2] = {&y, &y};
            float *reverbOut[2] = {&yl, &yr};

            reverb.prepare(1);
            reverb.process(1, reverbIn, reverbOut);

            for (int c = 0; c < nc; c++)
            {
                // Write it to the output array here.
                out[i * nc + c] = *reverbOut[c % 2];
            }
        }
    });

    bool isTriggered = false;

    quickGui([&]()
    {
        ImGui::Begin("New PAL project");

        ImGui::LabelText("Finger disp.", "%f\n", finger.u);

        ImGui::SliderFloat("Finger pos", &pos, 0, 1.0);

        if (ImGui::Button("Trigger / release"))
        {
            if (finger.state == FingerState::FingerReleased)
            {
                printf("Triggered\n");
                finger.trigger();
            }
            else
            {
                printf("Released\n");
                finger.release();
            }
        }

        ImGui::Checkbox("Pause", &pause);

        if (ImGui::Button("Step"))
        {
            step = true;
            stepCount = stepSize;
        }

        ImGui::InputInt("Step size", &stepSize);

        ImGui::PlotLines(
            "String",
            [](void *data, int idx) { return (float)((double*)data)[idx]; }, string.u.data(),
            string.u.size() + 4,
            0,
            "",
            -0.001,
            0.001,
            ImVec2(0, 120));

        ImGui::End();

        ImGui::ShowDemoWindow();
    });

    rt.stop();

    return 0;
}