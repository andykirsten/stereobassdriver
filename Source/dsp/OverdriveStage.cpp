#include "OverdriveStage.h"
#include <cmath>

namespace ydna::dsp
{

void OverdriveStage::prepare(const juce::dsp::ProcessSpec& monoSpec)
{
    jassert(monoSpec.numChannels == 1);
    sampleRate = monoSpec.sampleRate;

    preFilter.prepare(monoSpec);
    postShelfFilter.prepare(monoSpec);
    toneFilter.prepare(monoSpec);

    oversampling.initProcessing(static_cast<size_t>(monoSpec.maximumBlockSize));

    drive.reset(monoSpec.sampleRate, 0.02);
    outputGain.reset(monoSpec.sampleRate, 0.02);

    filtersDirty = true;
    reset();
}

void OverdriveStage::reset()
{
    preFilter.reset();
    postShelfFilter.reset();
    toneFilter.reset();
    oversampling.reset();
}

void OverdriveStage::setVoicing(Voicing voicing)
{
    if (voicing == currentVoicing)
        return;

    currentVoicing = voicing;
    coeffs = getVoicingCoefficients(voicing);
    filtersDirty = true;
}

void OverdriveStage::setDrive(float drive01) noexcept
{
    drive.setTargetValue(juce::jlimit(0.0f, 1.0f, drive01));
}

void OverdriveStage::setTone(float tone01) noexcept
{
    toneValue = juce::jlimit(0.0f, 1.0f, tone01);
    filtersDirty = true;
}

void OverdriveStage::setOutputLevel(float linearGain) noexcept
{
    outputGain.setTargetValue(linearGain);
}

void OverdriveStage::updateFilterCoefficients()
{
    preFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate, coeffs.preEmphasisFreq, 0.7f,
        juce::Decibels::decibelsToGain(coeffs.preEmphasisGainDb));

    postShelfFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRate, coeffs.postShelfFreq, 0.7f,
        juce::Decibels::decibelsToGain(coeffs.postShelfGainDb));

    // Tone-Knob: einfacher High-Shelf-Tilt, ±6 dB um die Mitte des Reglers.
    const auto toneDb = (toneValue - 0.5f) * 2.0f * 6.0f;
    toneFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRate, 2500.0f, 0.7f, juce::Decibels::decibelsToGain(toneDb));

    filtersDirty = false;
}

void OverdriveStage::process(juce::AudioBuffer<float>& buffer)
{
    if (filtersDirty)
        updateFilterCoefficients();

    const auto numSamples = buffer.getNumSamples();
    auto* data = buffer.getWritePointer(0);

    juce::dsp::AudioBlock<float> block(buffer);
    preFilter.process(juce::dsp::ProcessContextReplacing<float>(block));

    // Drive-Gain (inkl. voicing-abhängiger Skalierung) VOR dem Oversampling
    // anwenden: reine Verstärkung erzeugt keine neuen Aliasing-Anteile, daher
    // genügt hier Basis-Samplerate-Smoothing.
    constexpr float maxDriveGain = 16.0f;
    for (int i = 0; i < numSamples; ++i)
    {
        const auto driveGain = (1.0f + drive.getNextValue() * (maxDriveGain - 1.0f)) * coeffs.driveMultiplier;
        lastDriveGain = driveGain;
        data[i] *= driveGain;
    }

    auto oversampledBlock = oversampling.processSamplesUp(block);
    auto* osData = oversampledBlock.getChannelPointer(0);
    const auto numOversampled = oversampledBlock.getNumSamples();

    for (size_t i = 0; i < numOversampled; ++i)
        osData[i] = waveshape(osData[i], 1.0f, coeffs.asymmetryAmount);

    oversampling.processSamplesDown(block);

    // Auto-Level-Kompensation, damit höherer Drive nicht nur lauter wird.
    const auto compensation = 1.0f / std::sqrt(juce::jmax(1.0f, lastDriveGain));
    for (int i = 0; i < numSamples; ++i)
        data[i] *= compensation;

    postShelfFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
    toneFilter.process(juce::dsp::ProcessContextReplacing<float>(block));

    for (int i = 0; i < numSamples; ++i)
        data[i] *= outputGain.getNextValue();
}

double OverdriveStage::getLatencySamples() const
{
    return oversampling.getLatencyInSamples();
}

} // namespace ydna::dsp
