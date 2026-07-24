#include "Voicing.h"
#include <cmath>

namespace ydna::dsp
{

VoicingCoefficients getVoicingCoefficients(Voicing voicing)
{
    if (voicing == Voicing::Marshall)
    {
        return VoicingCoefficients {
            /* preEmphasisFreq   */ 800.0f,
            /* preEmphasisGainDb */ 4.0f,
            /* postShelfFreq     */ 3000.0f,
            /* postShelfGainDb   */ 3.0f,
            /* driveMultiplier   */ 1.0f,
            /* asymmetryAmount   */ 0.0f
        };
    }

    // Mesa: dunkler, tighter Low-Mid-Fokus, mehr Sättigung, asymmetrische Kennlinie.
    return VoicingCoefficients {
        /* preEmphasisFreq   */ 250.0f,
        /* preEmphasisGainDb */ 3.0f,
        /* postShelfFreq     */ 3500.0f,
        /* postShelfGainDb   */ -2.0f,
        /* driveMultiplier   */ 1.6f,
        /* asymmetryAmount   */ 0.35f
    };
}

float waveshape(float x, float drive, float asymmetry) noexcept
{
    const auto driven = x * drive;

    if (asymmetry <= 0.0001f)
        return std::tanh(driven);

    // Asymmetrische Kennlinie: positive und negative Halbwelle sättigen
    // unterschiedlich stark, plus kleiner DC-Ausgleich, um die Bias-freie
    // Rückgabe (kein wahrnehmbarer Offset) zu erhalten.
    const auto posGain = 1.0f + asymmetry;
    const auto negGain = 1.0f - asymmetry * 0.5f;
    const auto shaped = driven >= 0.0f ? std::tanh(driven * posGain)
                                        : std::tanh(driven * negGain);
    const auto dcOffset = 0.5f * asymmetry * 0.15f;
    return shaped - dcOffset;
}

} // namespace ydna::dsp
