#include "Parameters.h"

namespace ydna
{

// Gemeinsame Drive/Voicing/Tone/Level/Pan-Parameter fuer Mid- und High-Band.
// includePitch fuegt zusaetzlich Dry-Level, zwei Pitch-Stimmen und
// Formant-Erhalt hinzu (nur fuer das High-Band, das Gitarrensaiten-Register).
static void addBandSideParams(std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params,
                               Band band, Side side, bool includePitch)
{
    using namespace juce;

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { bandSideId(ParamID::odDriveBase, band, side), 1 },
        bandSideLabel("Drive", band, side),
        NormalisableRange<float> { 0.0f, 1.0f, 0.0001f },
        0.4f));

    params.push_back(std::make_unique<AudioParameterChoice>(
        ParameterID { bandSideId(ParamID::odVoicingBase, band, side), 1 },
        bandSideLabel("Voicing", band, side),
        StringArray { "Marshall", "Mesa" },
        0));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { bandSideId(ParamID::odToneBase, band, side), 1 },
        bandSideLabel("Tone", band, side),
        NormalisableRange<float> { 0.0f, 1.0f, 0.0001f },
        0.5f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { bandSideId(ParamID::odLevelBase, band, side), 1 },
        bandSideLabel("Level", band, side),
        NormalisableRange<float> { 0.0f, 2.0f, 0.0001f },
        1.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { bandSideId(ParamID::panBase, band, side), 1 },
        bandSideLabel("Pan", band, side),
        NormalisableRange<float> { 0.0f, 100.0f, 0.1f },
        100.0f));

    if (! includePitch)
        return;

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { bandSideId(ParamID::dryLevelBase, band, side), 1 },
        bandSideLabel("Dry Level", band, side),
        NormalisableRange<float> { 0.0f, 1.0f, 0.0001f },
        1.0f));

    params.push_back(std::make_unique<AudioParameterBool>(
        ParameterID { bandSideId(ParamID::pitchAEnableBase, band, side), 1 },
        bandSideLabel("Pitch A Enable", band, side),
        false));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { bandSideId(ParamID::pitchASemitonesBase, band, side), 1 },
        bandSideLabel("Pitch A", band, side),
        NormalisableRange<float> { 0.0f, 12.0f, 0.01f },
        12.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { bandSideId(ParamID::pitchALevelBase, band, side), 1 },
        bandSideLabel("Pitch A Level", band, side),
        NormalisableRange<float> { 0.0f, 1.0f, 0.0001f },
        1.0f));

    params.push_back(std::make_unique<AudioParameterBool>(
        ParameterID { bandSideId(ParamID::pitchBEnableBase, band, side), 1 },
        bandSideLabel("Pitch B Enable", band, side),
        false));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { bandSideId(ParamID::pitchBSemitonesBase, band, side), 1 },
        bandSideLabel("Pitch B", band, side),
        NormalisableRange<float> { 0.0f, 24.0f, 0.01f },
        7.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { bandSideId(ParamID::pitchBLevelBase, band, side), 1 },
        bandSideLabel("Pitch B Level", band, side),
        NormalisableRange<float> { 0.0f, 1.0f, 0.0001f },
        1.0f));

    params.push_back(std::make_unique<AudioParameterBool>(
        ParameterID { bandSideId(ParamID::pitchFormantBase, band, side), 1 },
        bandSideLabel("Formant Erhalt", band, side),
        true));
}

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    using namespace juce;

    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::xoverFreqLo, 1 },
        "Crossover Lo",
        NormalisableRange<float> { 40.0f, 500.0f, 0.01f, 0.4f },
        120.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::xoverFreqHi, 1 },
        "Crossover Hi",
        NormalisableRange<float> { 300.0f, 3000.0f, 0.01f, 0.4f },
        800.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::compThresh, 1 },
        "Bass Threshold",
        NormalisableRange<float> { -48.0f, 0.0f, 0.01f },
        -24.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::compRatio, 1 },
        "Bass Ratio",
        NormalisableRange<float> { 1.0f, 20.0f, 0.01f, 0.5f },
        8.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::compAttack, 1 },
        "Bass Attack",
        NormalisableRange<float> { 0.1f, 100.0f, 0.01f, 0.4f },
        5.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::compRelease, 1 },
        "Bass Release",
        NormalisableRange<float> { 10.0f, 500.0f, 0.1f, 0.4f },
        80.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::compMakeup, 1 },
        "Bass Makeup",
        NormalisableRange<float> { -12.0f, 24.0f, 0.01f },
        6.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::compHighpass, 1 },
        "Bass Highpass",
        NormalisableRange<float> { 20.0f, 200.0f, 0.01f, 0.4f },
        50.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::lowLevel, 1 },
        "Bass Level",
        NormalisableRange<float> { 0.0f, 2.0f, 0.0001f },
        1.0f));

    // Gate.
    params.push_back(std::make_unique<AudioParameterBool>(
        ParameterID { ParamID::gateEnable, 1 }, "Gate Enable", false));
    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::gateThreshold, 1 }, "Gate Threshold",
        NormalisableRange<float> { -80.0f, 0.0f, 0.01f }, -50.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::gateRatio, 1 }, "Gate Ratio",
        NormalisableRange<float> { 1.0f, 20.0f, 0.01f, 0.5f }, 4.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::gateAttack, 1 }, "Gate Attack",
        NormalisableRange<float> { 0.1f, 100.0f, 0.01f, 0.4f }, 2.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::gateRelease, 1 }, "Gate Release",
        NormalisableRange<float> { 10.0f, 1000.0f, 0.1f, 0.4f }, 150.0f));

    // Mid-Band (volle Stereo-Overdrive-Kette, ohne Pitch).
    addBandSideParams(params, Band::Mid, Side::Left, false);
    addBandSideParams(params, Band::Mid, Side::Right, false);

    // High-Band (Overdrive + Dry/Pitch-A/Pitch-B-Blend + Formant-Erhalt).
    addBandSideParams(params, Band::High, Side::Left, true);
    addBandSideParams(params, Band::High, Side::Right, true);

    // Chorus/Doubler (Gitarren-Bus = Mid + High).
    params.push_back(std::make_unique<AudioParameterBool>(
        ParameterID { ParamID::chorusEnable, 1 }, "Chorus Enable", false));
    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::chorusRate, 1 }, "Chorus Rate",
        NormalisableRange<float> { 0.05f, 5.0f, 0.001f, 0.5f }, 0.8f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::chorusDepth, 1 }, "Chorus Depth",
        NormalisableRange<float> { 0.0f, 1.0f, 0.0001f }, 0.25f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::chorusCentreDelay, 1 }, "Chorus Delay",
        NormalisableRange<float> { 1.0f, 30.0f, 0.01f }, 7.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::chorusFeedback, 1 }, "Chorus Feedback",
        NormalisableRange<float> { 0.0f, 0.9f, 0.0001f }, 0.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::chorusMix, 1 }, "Chorus Mix",
        NormalisableRange<float> { 0.0f, 1.0f, 0.0001f }, 0.35f));

    // Cabinet-Sim (Gitarren-Bus, nach dem Chorus).
    params.push_back(std::make_unique<AudioParameterBool>(
        ParameterID { ParamID::cabEnable, 1 }, "Cab Enable", false));
    params.push_back(std::make_unique<AudioParameterChoice>(
        ParameterID { ParamID::cabSelect, 1 }, "Cab Model",
        StringArray { "Bright 4x12", "Dark 2x12", "Mid Focus", "Custom" }, 0));
    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::cabLevel, 1 }, "Cab Level",
        NormalisableRange<float> { 0.0f, 2.0f, 0.0001f }, 1.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::mix, 1 },
        "Mix",
        NormalisableRange<float> { 0.0f, 1.0f, 0.0001f },
        1.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID { ParamID::outputGain, 1 },
        "Output",
        NormalisableRange<float> { -24.0f, 24.0f, 0.01f },
        0.0f));

    params.push_back(std::make_unique<AudioParameterBool>(
        ParameterID { ParamID::bypass, 1 },
        "Bypass",
        false));

    return { params.begin(), params.end() };
}

} // namespace ydna
