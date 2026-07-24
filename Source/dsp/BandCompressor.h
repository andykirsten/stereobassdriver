#pragma once

#include <juce_dsp/juce_dsp.h>

namespace ydna::dsp
{

/** Kompressor für das Low-Band. Summiert den Input-Buffer intern auf mono
    (der Bassbereich soll unabhängig vom Stereobild in der Mitte bleiben),
    komprimiert stark und schreibt das identische Ergebnis auf alle Kanäle
    zurück, damit das Center-Signal später gleich auf L/R addiert wird. */
class BandCompressor
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    void setThreshold(float dB);
    void setRatio(float ratio);
    void setAttack(float ms);
    void setRelease(float ms);
    void setMakeupGainDb(float dB);
    void setOutputLevel(float linearGain);
    void setHighpassFrequency(float hz);

    /** Verarbeitet buffer in-place (mono-summiert, komprimiert, auf alle
        Kanäle dupliziert). */
    void process(juce::AudioBuffer<float>& buffer);

private:
    juce::dsp::Compressor<float> compressor;
    juce::dsp::IIR::Filter<float> highpassFilter;
    juce::AudioBuffer<float> monoScratch;
    juce::SmoothedValue<float> makeupGain { 1.0f };
    juce::SmoothedValue<float> outputLevel { 1.0f };
    double sampleRate = 44100.0;
};

} // namespace ydna::dsp
