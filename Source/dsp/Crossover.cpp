#include "Crossover.h"

namespace ydna::dsp
{

void Crossover::prepare(const juce::dsp::ProcessSpec& spec)
{
    lowPass.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    highPassLo.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    midLowPass.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    highPassHi.setType(juce::dsp::LinkwitzRileyFilterType::highpass);

    lowPass.prepare(spec);
    highPassLo.prepare(spec);
    midLowPass.prepare(spec);
    highPassHi.prepare(spec);

    restScratch.setSize(static_cast<int>(spec.numChannels),
                         static_cast<int>(spec.maximumBlockSize));
}

void Crossover::reset()
{
    lowPass.reset();
    highPassLo.reset();
    midLowPass.reset();
    highPassHi.reset();
}

void Crossover::setCrossoverFrequencies(float loHz, float hiHz)
{
    lowPass.setCutoffFrequency(loHz);
    highPassLo.setCutoffFrequency(loHz);
    midLowPass.setCutoffFrequency(hiHz);
    highPassHi.setCutoffFrequency(hiHz);
}

void Crossover::process(const juce::AudioBuffer<float>& input,
                         juce::AudioBuffer<float>& lowOut,
                         juce::AudioBuffer<float>& midOut,
                         juce::AudioBuffer<float>& highOut)
{
    const auto numChannels = input.getNumChannels();
    const auto numSamples = input.getNumSamples();

    lowOut.makeCopyOf(input, true);
    restScratch.setSize(numChannels, numSamples, false, false, true);
    restScratch.makeCopyOf(input, true);

    juce::dsp::AudioBlock<float> lowBlock(lowOut);
    juce::dsp::AudioBlock<float> restBlock(restScratch);

    lowPass.process(juce::dsp::ProcessContextReplacing<float>(lowBlock));
    highPassLo.process(juce::dsp::ProcessContextReplacing<float>(restBlock));

    midOut.makeCopyOf(restScratch, true);
    highOut.makeCopyOf(restScratch, true);

    juce::dsp::AudioBlock<float> midBlock(midOut);
    juce::dsp::AudioBlock<float> highBlock(highOut);

    midLowPass.process(juce::dsp::ProcessContextReplacing<float>(midBlock));
    highPassHi.process(juce::dsp::ProcessContextReplacing<float>(highBlock));
}

} // namespace ydna::dsp
