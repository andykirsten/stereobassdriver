#pragma once

#include <juce_dsp/juce_dsp.h>
#include "../Parameters.h"

namespace ydna::dsp
{

/** Lautsprecher-/Cabinet-Simulation via Faltung. Die eingebauten Presets sind
    synthetisch erzeugte Platzhalter-Impulsantworten (kein Sampling echter
    Boxen) - sie geben dem Overdrive die grobe Speaker-Faerbung (Hochtonabfall,
    Resonanzueberhoehung), ersetzen aber keine echte Cab-IR. Fuer echte IRs
    steht loadCustomImpulseResponse() (Datei) zur Verfuegung.

    setModel()/loadCustomImpulseResponse() sind NICHT RT-safe (Speicher-
    allokation, ggf. Datei-I/O) und duerfen nur vom Message-Thread aus
    aufgerufen werden (z.B. aus einem GUI-Callback oder prepareToPlay), nie
    aus processBlock(). juce::dsp::Convolution ist so ausgelegt, dass das
    Laden waehrend parallel laufendem process() sicher ist. */
class CabinetSim
{
public:
    void prepare(const juce::dsp::ProcessSpec& stereoSpec);
    void reset();

    void setModel(CabModel model);
    void loadCustomImpulseResponse(const juce::File& file);
    void setLevel(float linearGain);

    void process(juce::AudioBuffer<float>& buffer);

    int getLatencySamples() const { return static_cast<int>(convolution.getLatency()); }

private:
    void loadBuiltInImpulseResponse(CabModel model);

    juce::dsp::Convolution convolution;
    juce::SmoothedValue<float> level { 1.0f };
    CabModel currentModel = CabModel::Bright4x12;
    double sampleRate = 44100.0;
};

} // namespace ydna::dsp
