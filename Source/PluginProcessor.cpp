#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace ydna
{

StereodriveProcessor::BandSideParams StereodriveProcessor::fetchBandSideParams(juce::AudioProcessorValueTreeState& state, Band band, Side side)
{
    BandSideParams p;
    p.drive   = state.getRawParameterValue(bandSideId(ParamID::odDriveBase, band, side));
    p.voicing = state.getRawParameterValue(bandSideId(ParamID::odVoicingBase, band, side));
    p.tone    = state.getRawParameterValue(bandSideId(ParamID::odToneBase, band, side));
    p.level   = state.getRawParameterValue(bandSideId(ParamID::odLevelBase, band, side));
    p.pan     = state.getRawParameterValue(bandSideId(ParamID::panBase, band, side));
    return p;
}

StereodriveProcessor::HighSideParams StereodriveProcessor::fetchHighSideParams(juce::AudioProcessorValueTreeState& state, Side side)
{
    HighSideParams p;
    static_cast<BandSideParams&>(p) = fetchBandSideParams(state, Band::High, side);
    p.dryLevel        = state.getRawParameterValue(bandSideId(ParamID::dryLevelBase, Band::High, side));
    p.pitchAEnable    = state.getRawParameterValue(bandSideId(ParamID::pitchAEnableBase, Band::High, side));
    p.pitchASemitones = state.getRawParameterValue(bandSideId(ParamID::pitchASemitonesBase, Band::High, side));
    p.pitchALevel     = state.getRawParameterValue(bandSideId(ParamID::pitchALevelBase, Band::High, side));
    p.pitchBEnable    = state.getRawParameterValue(bandSideId(ParamID::pitchBEnableBase, Band::High, side));
    p.pitchBSemitones = state.getRawParameterValue(bandSideId(ParamID::pitchBSemitonesBase, Band::High, side));
    p.pitchBLevel     = state.getRawParameterValue(bandSideId(ParamID::pitchBLevelBase, Band::High, side));
    p.formant         = state.getRawParameterValue(bandSideId(ParamID::pitchFormantBase, Band::High, side));
    return p;
}

StereodriveProcessor::StereodriveProcessor()
    : AudioProcessor(BusesProperties()
                          .withInput("Input", juce::AudioChannelSet::stereo(), true)
                          .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    pXoverFreqLo  = apvts.getRawParameterValue(ParamID::xoverFreqLo);
    pXoverFreqHi  = apvts.getRawParameterValue(ParamID::xoverFreqHi);
    pCompThresh   = apvts.getRawParameterValue(ParamID::compThresh);
    pCompRatio    = apvts.getRawParameterValue(ParamID::compRatio);
    pCompAttack   = apvts.getRawParameterValue(ParamID::compAttack);
    pCompRelease  = apvts.getRawParameterValue(ParamID::compRelease);
    pCompMakeup   = apvts.getRawParameterValue(ParamID::compMakeup);
    pCompHighpass = apvts.getRawParameterValue(ParamID::compHighpass);
    pLowLevel     = apvts.getRawParameterValue(ParamID::lowLevel);
    pMix          = apvts.getRawParameterValue(ParamID::mix);
    pOutputGain   = apvts.getRawParameterValue(ParamID::outputGain);
    pBypass       = apvts.getRawParameterValue(ParamID::bypass);

    pGateEnable    = apvts.getRawParameterValue(ParamID::gateEnable);
    pGateThreshold = apvts.getRawParameterValue(ParamID::gateThreshold);
    pGateRatio     = apvts.getRawParameterValue(ParamID::gateRatio);
    pGateAttack    = apvts.getRawParameterValue(ParamID::gateAttack);
    pGateRelease   = apvts.getRawParameterValue(ParamID::gateRelease);

    pChorusEnable      = apvts.getRawParameterValue(ParamID::chorusEnable);
    pChorusRate        = apvts.getRawParameterValue(ParamID::chorusRate);
    pChorusDepth       = apvts.getRawParameterValue(ParamID::chorusDepth);
    pChorusCentreDelay = apvts.getRawParameterValue(ParamID::chorusCentreDelay);
    pChorusFeedback    = apvts.getRawParameterValue(ParamID::chorusFeedback);
    pChorusMix         = apvts.getRawParameterValue(ParamID::chorusMix);

    pCabEnable = apvts.getRawParameterValue(ParamID::cabEnable);
    pCabLevel  = apvts.getRawParameterValue(ParamID::cabLevel);

    midLeftParams   = fetchBandSideParams(apvts, Band::Mid, Side::Left);
    midRightParams  = fetchBandSideParams(apvts, Band::Mid, Side::Right);
    highLeftParams  = fetchHighSideParams(apvts, Side::Left);
    highRightParams = fetchHighSideParams(apvts, Side::Right);

    apvts.addParameterListener(ParamID::cabSelect, this);
}

StereodriveProcessor::~StereodriveProcessor()
{
    apvts.removeParameterListener(ParamID::cabSelect, this);
    cancelPendingUpdate();
}

void StereodriveProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID != ParamID::cabSelect)
        return;

    pendingCabModel.store(juce::roundToInt(newValue));
    triggerAsyncUpdate();
}

void StereodriveProcessor::handleAsyncUpdate()
{
    cabinetSim.setModel(static_cast<CabModel>(pendingCabModel.load()));
}

void StereodriveProcessor::loadCustomCabImpulseResponse(const juce::File& file)
{
    cabinetSim.loadCustomImpulseResponse(file);
    customIRPath = file.getFullPathName();

    if (auto* choice = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(ParamID::cabSelect)))
        *choice = static_cast<int>(CabModel::Custom);
}

bool StereodriveProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto out = layouts.getMainOutputChannelSet();
    if (out != juce::AudioChannelSet::stereo())
        return false;

    const auto in = layouts.getMainInputChannelSet();
    return in == juce::AudioChannelSet::stereo() || in == juce::AudioChannelSet::mono();
}

void StereodriveProcessor::prepareToPlay(double newSampleRate, int samplesPerBlock)
{
    sampleRate = newSampleRate;

    juce::dsp::ProcessSpec stereoSpec { newSampleRate, static_cast<juce::uint32>(samplesPerBlock), 2 };
    juce::dsp::ProcessSpec monoSpec   { newSampleRate, static_cast<juce::uint32>(samplesPerBlock), 1 };

    crossover.prepare(stereoSpec);
    lowCompressor.prepare(stereoSpec);
    gate.prepare(stereoSpec);

    midLeft.overdrive.prepare(monoSpec);
    midLeft.mono.setSize(1, samplesPerBlock);
    midRight.overdrive.prepare(monoSpec);
    midRight.mono.setSize(1, samplesPerBlock);

    for (auto* chain : { &highLeft, &highRight })
    {
        chain->overdrive.prepare(monoSpec);
        chain->pitchA.prepare(monoSpec);
        chain->pitchB.prepare(monoSpec);
        chain->mono.setSize(1, samplesPerBlock);
        chain->dryScratch.setSize(1, samplesPerBlock);
        chain->pitchAScratch.setSize(1, samplesPerBlock);
        chain->pitchBScratch.setSize(1, samplesPerBlock);
    }

    chorus.prepare(stereoSpec);
    cabinetSim.prepare(stereoSpec);

    dryBuffer.setSize(2, samplesPerBlock);
    lowBand.setSize(2, samplesPerBlock);
    midBand.setSize(2, samplesPerBlock);
    highBand.setSize(2, samplesPerBlock);
    midBandMixed.setSize(2, samplesPerBlock);
    highBandMixed.setSize(2, samplesPerBlock);
    midAligned.setSize(2, samplesPerBlock);
    guitarBus.setSize(2, samplesPerBlock);
    workBuffer.setSize(2, samplesPerBlock);
    lowDelayedBuffer.setSize(2, samplesPerBlock);
    dryDelayedBuffer.setSize(2, samplesPerBlock);

    lowBandDelay.prepare(stereoSpec);
    dryDelay.prepare(stereoSpec);
    midAlignDelay.prepare(stereoSpec);

    updateLatencyCompensation();
}

void StereodriveProcessor::updateLatencyCompensation()
{
    // Mid und High teilen sich denselben Overdrive-Oversampling-Faktor, daher
    // ist deren Latenz identisch; High kommt zusaetzlich die (fuer Voice A und
    // B identische) Pitch-Latenz hinzu. Beide Pitch-Instanzen laufen immer
    // (auch bei deaktivierter Stimme), daher ist die Latenz automations-
    // unabhaengig konstant. juce::dsp::Convolution ist per Default-Konstruktor
    // im Zero-Latency-Modus, die Cab-Latenz wird trotzdem defensiv abgefragt.
    const auto overdriveLatency = highLeft.overdrive.getLatencySamples();
    const auto pitchLatency = highLeft.pitchA.getLatencySamples();
    const auto cabLatency = cabinetSim.getLatencySamples();

    const auto highBandLatency = overdriveLatency + static_cast<double>(pitchLatency);
    const auto guitarBusLatency = highBandLatency + static_cast<double>(cabLatency);

    latencySamples = juce::roundToInt(guitarBusLatency);

    midAlignDelay.setDelay(static_cast<float>(pitchLatency));
    lowBandDelay.setDelay(static_cast<float>(latencySamples));
    dryDelay.setDelay(static_cast<float>(latencySamples));

    setLatencySamples(latencySamples);
}

void StereodriveProcessor::releaseResources()
{
}

void StereodriveProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const auto numSamples = buffer.getNumSamples();
    const auto numOutChannels = getTotalNumOutputChannels();

    // Mono-Input auf beide Kanaele duplizieren: aus einem mono DI-Bass soll
    // bewusst ein Stereobild entstehen.
    workBuffer.setSize(2, numSamples, false, false, true);
    auto& work = workBuffer;
    if (getTotalNumInputChannels() >= 2)
    {
        work.copyFrom(0, 0, buffer, 0, 0, numSamples);
        work.copyFrom(1, 0, buffer, 1, 0, numSamples);
    }
    else
    {
        work.copyFrom(0, 0, buffer, 0, 0, numSamples);
        work.copyFrom(1, 0, buffer, 0, 0, numSamples);
    }

    // ---- Noise Gate (vor allem anderen, kein Latenzeinfluss) ----
    if (pGateEnable->load() > 0.5f)
    {
        gate.setThreshold(pGateThreshold->load());
        gate.setRatio(pGateRatio->load());
        gate.setAttack(pGateAttack->load());
        gate.setRelease(pGateRelease->load());
        juce::dsp::AudioBlock<float> gateBlock(work);
        gate.process(juce::dsp::ProcessContextReplacing<float>(gateBlock));
    }

    dryBuffer.makeCopyOf(work, true);

    // ---- 3-Band-Crossover ----
    const auto loFreq = pXoverFreqLo->load();
    const auto hiFreq = juce::jmax(pXoverFreqHi->load(), loFreq + 1.0f);
    crossover.setCrossoverFrequencies(loFreq, hiFreq);
    crossover.process(work, lowBand, midBand, highBand);

    // ---- Low-Band: Kompressor, mono, mittig ----
    lowCompressor.setThreshold(pCompThresh->load());
    lowCompressor.setRatio(pCompRatio->load());
    lowCompressor.setAttack(pCompAttack->load());
    lowCompressor.setRelease(pCompRelease->load());
    lowCompressor.setMakeupGainDb(pCompMakeup->load());
    lowCompressor.setOutputLevel(pLowLevel->load());
    lowCompressor.setHighpassFrequency(pCompHighpass->load());
    lowCompressor.process(lowBand);

    // ---- Mid-Band: pro Seite Overdrive + Pan, keine Pitch (Gitarrenkoerper) ----
    midBandMixed.setSize(2, numSamples, false, false, true);
    midBandMixed.clear();

    auto processMidSide = [&](MidSideChain& chain, BandSideParams& params, int channel)
    {
        chain.mono.setSize(1, numSamples, false, false, true);
        chain.mono.copyFrom(0, 0, midBand, channel, 0, numSamples);

        const auto voicing = static_cast<Voicing>(juce::roundToInt(params.voicing->load()));
        chain.overdrive.setVoicing(voicing);
        chain.overdrive.setDrive(params.drive->load());
        chain.overdrive.setTone(params.tone->load());
        chain.overdrive.setOutputLevel(params.level->load());
        chain.overdrive.process(chain.mono);

        const auto pan = juce::jlimit(0.0f, 1.0f, params.pan->load() * 0.01f);
        const auto angle = (pan + 1.0f) * juce::MathConstants<float>::pi * 0.25f;
        const auto ownGain = std::sin(angle);
        const auto otherGain = std::cos(angle);
        const auto otherChannel = 1 - channel;

        midBandMixed.addFrom(channel, 0, chain.mono, 0, 0, numSamples, ownGain);
        midBandMixed.addFrom(otherChannel, 0, chain.mono, 0, 0, numSamples, otherGain);
    };

    processMidSide(midLeft, midLeftParams, 0);
    processMidSide(midRight, midRightParams, 1);

    // ---- High-Band: Overdrive + Dry/PitchA/PitchB-Blend + Pan (Gitarrensaiten) ----
    highBandMixed.setSize(2, numSamples, false, false, true);
    highBandMixed.clear();

    auto processHighSide = [&](HighSideChain& chain, HighSideParams& params, int channel)
    {
        chain.mono.setSize(1, numSamples, false, false, true);
        chain.mono.copyFrom(0, 0, highBand, channel, 0, numSamples);

        const auto voicing = static_cast<Voicing>(juce::roundToInt(params.voicing->load()));
        chain.overdrive.setVoicing(voicing);
        chain.overdrive.setDrive(params.drive->load());
        chain.overdrive.setTone(params.tone->load());
        chain.overdrive.setOutputLevel(params.level->load());
        chain.overdrive.process(chain.mono);

        const bool formantOn = params.formant->load() > 0.5f;
        chain.pitchA.setFormantCompensation(formantOn);
        chain.pitchB.setFormantCompensation(formantOn);

        chain.dryScratch.setSize(1, numSamples, false, false, true);
        chain.pitchAScratch.setSize(1, numSamples, false, false, true);
        chain.pitchBScratch.setSize(1, numSamples, false, false, true);

        chain.dryScratch.copyFrom(0, 0, chain.mono, 0, 0, numSamples);

        // Beide Pitch-Stimmen laufen IMMER (konstante Latenz); Mute erfolgt
        // ueber die Blend-Gains unten, nicht ueber Ein-/Ausschalten der Verarbeitung.
        chain.pitchAScratch.copyFrom(0, 0, chain.mono, 0, 0, numSamples);
        chain.pitchA.setTransposeSemitones(params.pitchASemitones->load());
        chain.pitchA.process(chain.pitchAScratch);

        chain.pitchBScratch.copyFrom(0, 0, chain.mono, 0, 0, numSamples);
        chain.pitchB.setTransposeSemitones(params.pitchBSemitones->load());
        chain.pitchB.process(chain.pitchBScratch);

        const bool aOn = params.pitchAEnable->load() > 0.5f;
        const bool bOn = params.pitchBEnable->load() > 0.5f;
        const auto dryGain = params.dryLevel->load();
        const auto aGain = aOn ? params.pitchALevel->load() : 0.0f;
        const auto bGain = bOn ? params.pitchBLevel->load() : 0.0f;

        chain.mono.clear();
        chain.mono.addFrom(0, 0, chain.dryScratch, 0, 0, numSamples, dryGain);
        chain.mono.addFrom(0, 0, chain.pitchAScratch, 0, 0, numSamples, aGain);
        chain.mono.addFrom(0, 0, chain.pitchBScratch, 0, 0, numSamples, bGain);

        const auto pan = juce::jlimit(0.0f, 1.0f, params.pan->load() * 0.01f);
        const auto angle = (pan + 1.0f) * juce::MathConstants<float>::pi * 0.25f;
        const auto ownGain = std::sin(angle);
        const auto otherGain = std::cos(angle);
        const auto otherChannel = 1 - channel;

        highBandMixed.addFrom(channel, 0, chain.mono, 0, 0, numSamples, ownGain);
        highBandMixed.addFrom(otherChannel, 0, chain.mono, 0, 0, numSamples, otherGain);
    };

    processHighSide(highLeft, highLeftParams, 0);
    processHighSide(highRight, highRightParams, 1);

    // ---- Mid auf High-Band-Latenz (Pitch) ausgleichen, dann zum Gitarren-Bus summieren ----
    midAligned.setSize(2, numSamples, false, false, true);
    {
        juce::dsp::AudioBlock<float> midBlock(midBandMixed);
        juce::dsp::AudioBlock<float> midAlignedBlock(midAligned);
        juce::dsp::ProcessContextNonReplacing<float> ctx(midBlock, midAlignedBlock);
        midAlignDelay.process(ctx);
    }

    guitarBus.setSize(2, numSamples, false, false, true);
    guitarBus.makeCopyOf(midAligned, true);
    guitarBus.addFrom(0, 0, highBandMixed, 0, 0, numSamples);
    guitarBus.addFrom(1, 0, highBandMixed, 1, 0, numSamples);

    // ---- Doubler/Chorus auf dem Gitarren-Bus ----
    if (pChorusEnable->load() > 0.5f)
    {
        chorus.setRate(pChorusRate->load());
        chorus.setDepth(pChorusDepth->load());
        chorus.setCentreDelay(pChorusCentreDelay->load());
        chorus.setFeedback(pChorusFeedback->load());
        chorus.setMix(pChorusMix->load());
        juce::dsp::AudioBlock<float> chorusBlock(guitarBus);
        chorus.process(juce::dsp::ProcessContextReplacing<float>(chorusBlock));
    }

    // ---- Cabinet-Sim auf dem Gitarren-Bus ----
    if (pCabEnable->load() > 0.5f)
    {
        cabinetSim.setLevel(pCabLevel->load());
        cabinetSim.process(guitarBus);
    }

    // ---- Low-Band zeitlich an Gitarren-Bus-Latenz angleichen ----
    lowDelayedBuffer.setSize(2, numSamples, false, false, true);
    {
        juce::dsp::AudioBlock<float> lowBlock(lowBand);
        juce::dsp::AudioBlock<float> lowDelayedBlock(lowDelayedBuffer);
        juce::dsp::ProcessContextNonReplacing<float> ctx(lowBlock, lowDelayedBlock);
        lowBandDelay.process(ctx);
    }
    auto& lowDelayed = lowDelayedBuffer;

    // ---- Dry-Signal ebenfalls verzoegern, damit Mix nicht kammfiltert ----
    dryDelayedBuffer.setSize(2, numSamples, false, false, true);
    {
        juce::dsp::AudioBlock<float> dryBlock(dryBuffer);
        juce::dsp::AudioBlock<float> dryDelayedBlock(dryDelayedBuffer);
        juce::dsp::ProcessContextNonReplacing<float> ctx(dryBlock, dryDelayedBlock);
        dryDelay.process(ctx);
    }
    auto& dryDelayed = dryDelayedBuffer;

    // ---- Summe Low + Gitarren-Bus, Output-Gain, Bypass/Mix ----
    const auto outputGainLin = juce::Decibels::decibelsToGain(pOutputGain->load());
    const bool bypassed = pBypass->load() > 0.5f;
    const auto mixAmount = bypassed ? 0.0f : pMix->load();

    for (int ch = 0; ch < 2; ++ch)
    {
        auto* wet = work.getWritePointer(ch);
        const auto* low = lowDelayed.getReadPointer(ch);
        const auto* guitar = guitarBus.getReadPointer(ch);
        const auto* dry = dryDelayed.getReadPointer(ch);

        for (int i = 0; i < numSamples; ++i)
        {
            const auto wetSample = (low[i] + guitar[i]) * outputGainLin;
            wet[i] = dry[i] * (1.0f - mixAmount) + wetSample * mixAmount;
        }
    }

    for (int ch = 0; ch < numOutChannels; ++ch)
        buffer.copyFrom(ch, 0, work, juce::jmin(ch, 1), 0, numSamples);
}

juce::AudioProcessorEditor* StereodriveProcessor::createEditor()
{
    return new StereodriveEditor(*this);
}

void StereodriveProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    if (auto xml = state.createXml())
    {
        xml->setAttribute("customIRPath", customIRPath);
        copyXmlToBinary(*xml, destData);
    }
}

void StereodriveProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
    {
        if (xml->hasTagName(apvts.state.getType()))
        {
            customIRPath = xml->getStringAttribute("customIRPath");
            apvts.replaceState(juce::ValueTree::fromXml(*xml));

            if (customIRPath.isNotEmpty())
            {
                const auto file = juce::File(customIRPath);
                if (file.existsAsFile())
                    cabinetSim.loadCustomImpulseResponse(file);
            }
        }
    }
}

} // namespace ydna

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ydna::StereodriveProcessor();
}
