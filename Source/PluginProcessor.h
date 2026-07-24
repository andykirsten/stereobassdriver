#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_events/juce_events.h>

#include "Parameters.h"
#include "dsp/Crossover.h"
#include "dsp/BandCompressor.h"
#include "dsp/OverdriveStage.h"
#include "dsp/PitchShifter.h"
#include "dsp/CabinetSim.h"

namespace ydna
{

class StereodriveProcessor final : public juce::AudioProcessor,
                                    private juce::AudioProcessorValueTreeState::Listener,
                                    private juce::AsyncUpdater
{
public:
    StereodriveProcessor();
    ~StereodriveProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    /** Vom Message-Thread aus aufzurufen (z.B. GUI-Button nach FileChooser),
        niemals aus processBlock(): Convolution-IR-Laden ist nicht RT-safe. */
    void loadCustomCabImpulseResponse(const juce::File& file);

private:
    void updateLatencyCompensation();

    // AudioProcessorValueTreeState::Listener: reagiert auf Aenderungen von
    // cab_select (auch per Automation) und stoesst das eigentliche,
    // nicht-RT-safe IR-Laden ueber AsyncUpdater auf dem Message-Thread an.
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void handleAsyncUpdate() override;

    // Mid: volle Stereo-Overdrive-Kette ohne Pitch (Gitarrenkoerper-Register).
    struct MidSideChain
    {
        dsp::OverdriveStage overdrive;
        juce::AudioBuffer<float> mono;
    };

    // High: Overdrive + Dry/PitchA/PitchB-Blend (Gitarrensaiten-Register).
    struct HighSideChain
    {
        dsp::OverdriveStage overdrive;
        dsp::PitchShifter pitchA;
        dsp::PitchShifter pitchB;
        juce::AudioBuffer<float> mono, dryScratch, pitchAScratch, pitchBScratch;
    };

    struct BandSideParams
    {
        std::atomic<float>* drive = nullptr;
        std::atomic<float>* voicing = nullptr;
        std::atomic<float>* tone = nullptr;
        std::atomic<float>* level = nullptr;
        std::atomic<float>* pan = nullptr;
    };

    struct HighSideParams : BandSideParams
    {
        std::atomic<float>* dryLevel = nullptr;
        std::atomic<float>* pitchAEnable = nullptr;
        std::atomic<float>* pitchASemitones = nullptr;
        std::atomic<float>* pitchALevel = nullptr;
        std::atomic<float>* pitchBEnable = nullptr;
        std::atomic<float>* pitchBSemitones = nullptr;
        std::atomic<float>* pitchBLevel = nullptr;
        std::atomic<float>* formant = nullptr;
    };

    static BandSideParams fetchBandSideParams(juce::AudioProcessorValueTreeState& state, Band band, Side side);
    static HighSideParams fetchHighSideParams(juce::AudioProcessorValueTreeState& state, Side side);

    dsp::Crossover crossover;
    dsp::BandCompressor lowCompressor;
    juce::dsp::NoiseGate<float> gate;

    MidSideChain midLeft, midRight;
    HighSideChain highLeft, highRight;

    juce::dsp::Chorus<float> chorus;
    dsp::CabinetSim cabinetSim;

    juce::dsp::DelayLine<float> lowBandDelay   { 1 << 16 };
    juce::dsp::DelayLine<float> dryDelay       { 1 << 16 };
    juce::dsp::DelayLine<float> midAlignDelay  { 1 << 16 };

    juce::AudioBuffer<float> dryBuffer, lowBand, midBand, highBand;
    juce::AudioBuffer<float> midBandMixed, highBandMixed, midAligned, guitarBus;
    juce::AudioBuffer<float> workBuffer, lowDelayedBuffer, dryDelayedBuffer;

    // Gecachte Parameterzeiger (APVTS besitzt den Speicher).
    std::atomic<float>* pXoverFreqLo = nullptr;
    std::atomic<float>* pXoverFreqHi = nullptr;
    std::atomic<float>* pCompThresh = nullptr;
    std::atomic<float>* pCompRatio = nullptr;
    std::atomic<float>* pCompAttack = nullptr;
    std::atomic<float>* pCompRelease = nullptr;
    std::atomic<float>* pCompMakeup = nullptr;
    std::atomic<float>* pCompHighpass = nullptr;
    std::atomic<float>* pLowLevel = nullptr;
    std::atomic<float>* pMix = nullptr;
    std::atomic<float>* pOutputGain = nullptr;
    std::atomic<float>* pBypass = nullptr;

    std::atomic<float>* pGateEnable = nullptr;
    std::atomic<float>* pGateThreshold = nullptr;
    std::atomic<float>* pGateRatio = nullptr;
    std::atomic<float>* pGateAttack = nullptr;
    std::atomic<float>* pGateRelease = nullptr;

    std::atomic<float>* pChorusEnable = nullptr;
    std::atomic<float>* pChorusRate = nullptr;
    std::atomic<float>* pChorusDepth = nullptr;
    std::atomic<float>* pChorusCentreDelay = nullptr;
    std::atomic<float>* pChorusFeedback = nullptr;
    std::atomic<float>* pChorusMix = nullptr;

    std::atomic<float>* pCabEnable = nullptr;
    std::atomic<float>* pCabLevel = nullptr;

    std::atomic<int> pendingCabModel { 0 };
    juce::String customIRPath;

    BandSideParams midLeftParams, midRightParams;
    HighSideParams highLeftParams, highRightParams;

    double sampleRate = 44100.0;
    int latencySamples = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereodriveProcessor)
};

} // namespace ydna
