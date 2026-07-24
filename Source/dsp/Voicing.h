#pragma once

#include "../Parameters.h"

namespace ydna::dsp
{

/** Klangliche Kennwerte, die ein Overdrive-Voicing (Marshall/Mesa) definieren.
    Marshall: helleres, mittenbetontes Crunch mit symmetrischem Soft-Clip und
    Presence-Anhebung. Mesa: dunklerer, tighter Low-Mid-Fokus mit asymmetrischer,
    stärker sättigender Kennlinie. */
struct VoicingCoefficients
{
    float preEmphasisFreq;   // Hz, Frequenz der Vor-Anhebung/-Absenkung vor dem Clipping
    float preEmphasisGainDb; // dB
    float postShelfFreq;     // Hz, Presence/Tightness-Shelf nach dem Clipping
    float postShelfGainDb;   // dB
    float driveMultiplier;   // zusätzliche Gain-Skalierung vor dem Waveshaper
    float asymmetryAmount;   // 0 = symmetrisch, >0 = zunehmend asymmetrisch (Röhren-/Dioden-Feel)
};

VoicingCoefficients getVoicingCoefficients(Voicing voicing);

/** Weiche, sättigende Kennlinie. drive skaliert den Eingang vor der
    Nichtlinearität, asymmetry verschiebt Bias und Kurvenform zwischen
    positiver/negativer Halbwelle (0 = symmetrischer tanh-Clip). */
float waveshape(float x, float drive, float asymmetry) noexcept;

} // namespace ydna::dsp
