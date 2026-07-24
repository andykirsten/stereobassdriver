#include "CabinetSim.h"

namespace ydna::dsp
{

namespace
{
    // Erzeugt eine synthetische Platzhalter-Speaker-IR: gefiltertes Rauschen
    // mit exponentiellem Zerfall, grob geformt wie eine Cab-Resonanz. Ersetzt
    // keine echte Cab-Aufnahme, gibt der Overdrive-Stufe aber die grundlegende
    // Hoehenbedaempfung/Resonanzcharakteristik eines Gitarrenlautsprechers.
    juce::AudioBuffer<float> makeSyntheticIR(double sr, float cutoffHz, float resonanceHz, float decayMs)
    {
        const auto numSamples = juce::jmax(64, static_cast<int>(sr * decayMs * 0.001));
        juce::AudioBuffer<float> ir(1, numSamples);

        juce::Random random(1234);
        juce::dsp::IIR::Filter<float> lowpass;
        lowpass.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sr, cutoffHz, 0.7071f);
        juce::dsp::IIR::Filter<float> peak;
        peak.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sr, resonanceHz, 2.0f, 2.5f);

        auto* data = ir.getWritePointer(0);
        const auto decaySamples = static_cast<float>(numSamples);
        for (int i = 0; i < numSamples; ++i)
        {
            const auto envelope = std::exp(-3.0f * static_cast<float>(i) / decaySamples);
            data[i] = (random.nextFloat() * 2.0f - 1.0f) * envelope;
        }

        for (int i = 0; i < numSamples; ++i)
            data[i] = peak.processSample(lowpass.processSample(data[i]));

        return ir;
    }
}

void CabinetSim::prepare(const juce::dsp::ProcessSpec& stereoSpec)
{
    sampleRate = stereoSpec.sampleRate;
    convolution.prepare(stereoSpec);
    level.reset(stereoSpec.sampleRate, 0.02);
    loadBuiltInImpulseResponse(currentModel);
}

void CabinetSim::reset()
{
    convolution.reset();
}

void CabinetSim::setModel(CabModel model)
{
    if (model == CabModel::Custom)
        return;

    currentModel = model;
    loadBuiltInImpulseResponse(model);
}

void CabinetSim::loadBuiltInImpulseResponse(CabModel model)
{
    float cutoff = 4000.0f, resonance = 1200.0f, decayMs = 25.0f;
    switch (model)
    {
        case CabModel::Bright4x12: cutoff = 5500.0f; resonance = 1800.0f; decayMs = 18.0f; break;
        case CabModel::Dark2x12:   cutoff = 2800.0f; resonance = 900.0f;  decayMs = 30.0f; break;
        case CabModel::MidFocus:   cutoff = 3500.0f; resonance = 1400.0f; decayMs = 22.0f; break;
        case CabModel::Custom: return;
    }

    auto ir = makeSyntheticIR(sampleRate, cutoff, resonance, decayMs);
    convolution.loadImpulseResponse(std::move(ir), sampleRate,
                                     juce::dsp::Convolution::Stereo::no,
                                     juce::dsp::Convolution::Trim::no,
                                     juce::dsp::Convolution::Normalise::yes);
}

void CabinetSim::loadCustomImpulseResponse(const juce::File& file)
{
    if (! file.existsAsFile())
        return;

    currentModel = CabModel::Custom;
    convolution.loadImpulseResponse(file,
                                     juce::dsp::Convolution::Stereo::yes,
                                     juce::dsp::Convolution::Trim::yes,
                                     0,
                                     juce::dsp::Convolution::Normalise::yes);
}

void CabinetSim::setLevel(float linearGain)
{
    level.setTargetValue(linearGain);
}

void CabinetSim::process(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);
    convolution.process(juce::dsp::ProcessContextReplacing<float>(block));

    const auto numSamples = buffer.getNumSamples();
    const auto numChannels = buffer.getNumChannels();
    for (int i = 0; i < numSamples; ++i)
    {
        const auto g = level.getNextValue();
        for (int ch = 0; ch < numChannels; ++ch)
            buffer.getWritePointer(ch)[i] *= g;
    }
}

} // namespace ydna::dsp
