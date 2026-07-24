#include "PitchShifter.h"

namespace ydna::dsp
{

void PitchShifter::prepare(const juce::dsp::ProcessSpec& monoSpec)
{
    jassert(monoSpec.numChannels == 1);

    stretch.presetDefault(1, static_cast<float>(monoSpec.sampleRate));
    scratchOutput.setSize(1, static_cast<int>(monoSpec.maximumBlockSize));

    reset();
}

void PitchShifter::reset()
{
    stretch.reset();
}

void PitchShifter::setTransposeSemitones(float semitones) noexcept
{
    if (currentSemitones == semitones)
        return;

    currentSemitones = semitones;
    stretch.setTransposeSemitones(semitones);
}

void PitchShifter::setFormantCompensation(bool enabled) noexcept
{
    if (formantCompensationEnabled == enabled)
        return;

    formantCompensationEnabled = enabled;
    stretch.setFormantSemitones(0.0f, enabled);
}

void PitchShifter::process(juce::AudioBuffer<float>& buffer)
{
    const auto numSamples = buffer.getNumSamples();
    scratchOutput.setSize(1, numSamples, false, false, true);

    float* inputPtrs[1]  = { buffer.getWritePointer(0) };
    float* outputPtrs[1] = { scratchOutput.getWritePointer(0) };

    stretch.process(inputPtrs, numSamples, outputPtrs, numSamples);

    buffer.copyFrom(0, 0, scratchOutput, 0, 0, numSamples);
}

int PitchShifter::getLatencySamples() const
{
    return stretch.inputLatency() + stretch.outputLatency();
}

} // namespace ydna::dsp
