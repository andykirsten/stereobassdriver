#pragma once

#include <juce_dsp/juce_dsp.h>
#include "Voicing.h"

namespace ydna::dsp
{

/** Overdrive für einen Kanal (eine Stereoseite): Pre-EQ (voicing-abhängig) ->
    Oversampling -> sättigende Kennlinie -> Post-Shelf -> Tone -> Level.
    Arbeitet grundsätzlich mono (eine Stereoseite = ein Kanal). */
class OverdriveStage
{
public:
    void prepare(const juce::dsp::ProcessSpec& monoSpec);
    void reset();

    void setVoicing(Voicing voicing);
    void setDrive(float drive01) noexcept;      // 0..1, geglättet
    void setTone(float tone01) noexcept;        // 0..1, geglättet (0=dunkel, 1=hell)
    void setOutputLevel(float linearGain) noexcept;

    void process(juce::AudioBuffer<float>& buffer);

    /** Latenz, die durch das interne Oversampling entsteht (Samples, Basisrate). */
    double getLatencySamples() const;

private:
    void updateFilterCoefficients();

    Voicing currentVoicing = Voicing::Marshall;
    VoicingCoefficients coeffs = getVoicingCoefficients(Voicing::Marshall);

    juce::dsp::IIR::Filter<float> preFilter;
    juce::dsp::IIR::Filter<float> postShelfFilter;
    juce::dsp::IIR::Filter<float> toneFilter;

    juce::dsp::Oversampling<float> oversampling { 1, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR };

    juce::SmoothedValue<float> drive      { 0.4f };
    juce::SmoothedValue<float> outputGain { 1.0f };
    float toneValue = 0.5f;
    float lastDriveGain = 1.0f;

    double sampleRate = 44100.0;
    bool filtersDirty = true;
};

} // namespace ydna::dsp
