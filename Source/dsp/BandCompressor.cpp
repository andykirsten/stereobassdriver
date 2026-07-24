#include "BandCompressor.h"

namespace ydna::dsp
{

void BandCompressor::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    // Kompressor arbeitet intern mono.
    juce::dsp::ProcessSpec monoSpec { spec.sampleRate, spec.maximumBlockSize, 1 };
    compressor.prepare(monoSpec);

    highpassFilter.prepare(monoSpec);
    highpassFilter.reset();
    setHighpassFrequency(50.0f);

    monoScratch.setSize(1, static_cast<int>(spec.maximumBlockSize));

    makeupGain.reset(spec.sampleRate, 0.02);
    outputLevel.reset(spec.sampleRate, 0.02);
}

void BandCompressor::reset()
{
    compressor.reset();
    highpassFilter.reset();
}

void BandCompressor::setThreshold(float dB)   { compressor.setThreshold(dB); }
void BandCompressor::setRatio(float ratio)    { compressor.setRatio(ratio); }
void BandCompressor::setAttack(float ms)      { compressor.setAttack(ms); }
void BandCompressor::setRelease(float ms)     { compressor.setRelease(ms); }

void BandCompressor::setMakeupGainDb(float dB)
{
    makeupGain.setTargetValue(juce::Decibels::decibelsToGain(dB));
}

void BandCompressor::setOutputLevel(float linearGain)
{
    outputLevel.setTargetValue(linearGain);
}

void BandCompressor::setHighpassFrequency(float hz)
{
    *highpassFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, hz, 0.7071f);
}

void BandCompressor::process(juce::AudioBuffer<float>& buffer)
{
    const auto numSamples = buffer.getNumSamples();
    const auto numChannels = buffer.getNumChannels();

    monoScratch.setSize(1, numSamples, false, false, true);
    monoScratch.clear();

    const auto scale = 1.0f / static_cast<float>(juce::jmax(1, numChannels));
    for (int ch = 0; ch < numChannels; ++ch)
        monoScratch.addFrom(0, 0, buffer, ch, 0, numSamples, scale);

    juce::dsp::AudioBlock<float> monoBlock(monoScratch);
    highpassFilter.process(juce::dsp::ProcessContextReplacing<float>(monoBlock));
    compressor.process(juce::dsp::ProcessContextReplacing<float>(monoBlock));

    auto* mono = monoScratch.getWritePointer(0);
    for (int i = 0; i < numSamples; ++i)
    {
        const auto g = makeupGain.getNextValue() * outputLevel.getNextValue();
        mono[i] *= g;
    }

    for (int ch = 0; ch < numChannels; ++ch)
        buffer.copyFrom(ch, 0, monoScratch, 0, 0, numSamples);
}

} // namespace ydna::dsp
