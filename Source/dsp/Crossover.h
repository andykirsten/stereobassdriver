#pragma once

#include <juce_dsp/juce_dsp.h>

namespace ydna::dsp
{

/** 3-Band Linkwitz-Riley Crossover (24 dB/Okt) mit zwei verschiebbaren
    Trennfrequenzen. Umsetzung als zwei kaskadierte 2-Wege-Splits: zuerst wird
    bei freqLo in Low/Rest getrennt, danach Rest bei freqHi in Mid/High. Da
    ein LR4-Filterpaar (Lowpass+Highpass derselben Cutoff-Frequenz) JEDES
    Eingangssignal exakt wieder zu diesem Signal aufsummiert (flacher Betrags-
    und Phasengang, unabhaengig vom Signalinhalt), gilt Low+Rest=Input exakt
    und Mid+High=Rest exakt - also Low+Mid+High=Input exakt, ganz ohne
    zusaetzlichen Allpass zur Phasenkompensation. */
class Crossover
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    void setCrossoverFrequencies(float loHz, float hiHz);

    /** Zerlegt den Input-Block in Low/Mid/High. Alle drei Ausgabepuffer
        muessen bereits auf dieselbe Kanal-/Sample-Anzahl wie input
        dimensioniert sein. */
    void process(const juce::AudioBuffer<float>& input,
                 juce::AudioBuffer<float>& lowOut,
                 juce::AudioBuffer<float>& midOut,
                 juce::AudioBuffer<float>& highOut);

private:
    juce::dsp::LinkwitzRileyFilter<float> lowPass;
    juce::dsp::LinkwitzRileyFilter<float> highPassLo;
    juce::dsp::LinkwitzRileyFilter<float> midLowPass;
    juce::dsp::LinkwitzRileyFilter<float> highPassHi;

    juce::AudioBuffer<float> restScratch;
};

} // namespace ydna::dsp
