#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace ydna
{

enum class Side
{
    Left = 0,
    Right = 1
};

enum class Voicing
{
    Marshall = 0,
    Mesa = 1
};

/** Die beiden Stereo-Bänder oberhalb des mono Low-Bands. Mid bekommt volle
    Stereo-Overdrive-Verarbeitung ohne Pitch (Gitarrenkörper-Register), High
    zusätzlich die Pitch-Stimmen (Gitarrensaiten-Register). */
enum class Band
{
    Mid = 0,
    High = 1
};

enum class CabModel
{
    Bright4x12 = 0,
    Dark2x12 = 1,
    MidFocus = 2,
    Custom = 3
};

/** Zentrale Parameter-IDs. Seitenabhängige IDs werden über sideId() gebildet,
    band- und seitenabhängige über bandSideId(), damit L/R- bzw. Mid/High-Panels
    denselben Code mit unterschiedlichem Enum nutzen können. */
namespace ParamID
{
    inline constexpr auto xoverFreqLo = "xover_freq_lo";
    inline constexpr auto xoverFreqHi = "xover_freq_hi";
    inline constexpr auto compThresh  = "comp_threshold";
    inline constexpr auto compRatio   = "comp_ratio";
    inline constexpr auto compAttack  = "comp_attack";
    inline constexpr auto compRelease = "comp_release";
    inline constexpr auto compMakeup  = "comp_makeup";
    inline constexpr auto compHighpass = "comp_highpass";
    inline constexpr auto lowLevel    = "low_level";
    inline constexpr auto mix         = "mix";
    inline constexpr auto outputGain  = "output_gain";
    inline constexpr auto bypass      = "bypass";

    // Gate (Stereo, am Eingang, vor allem anderen).
    inline constexpr auto gateEnable    = "gate_enable";
    inline constexpr auto gateThreshold = "gate_threshold";
    inline constexpr auto gateRatio     = "gate_ratio";
    inline constexpr auto gateAttack    = "gate_attack";
    inline constexpr auto gateRelease   = "gate_release";

    // Chorus/Doubler (Stereo, auf dem Gitarren-Bus).
    inline constexpr auto chorusEnable      = "chorus_enable";
    inline constexpr auto chorusRate        = "chorus_rate";
    inline constexpr auto chorusDepth       = "chorus_depth";
    inline constexpr auto chorusCentreDelay = "chorus_centre_delay";
    inline constexpr auto chorusFeedback    = "chorus_feedback";
    inline constexpr auto chorusMix         = "chorus_mix";

    // Cabinet-Sim (Stereo, auf dem Gitarren-Bus, nach dem Chorus).
    inline constexpr auto cabEnable = "cab_enable";
    inline constexpr auto cabSelect = "cab_select";
    inline constexpr auto cabLevel  = "cab_level";

    // Band-/seitenabhängige Basisnamen; bandSideId() hängt "mid_"/"high_" vor
    // und "_l"/"_r" an.
    inline constexpr auto odDriveBase    = "od_drive";
    inline constexpr auto odVoicingBase  = "od_voicing";
    inline constexpr auto odToneBase     = "od_tone";
    inline constexpr auto odLevelBase    = "od_level";
    inline constexpr auto panBase        = "pan";

    // Nur High-Band: Dry-Anteil + zwei Pitch-Stimmen + Formant-Erhalt.
    inline constexpr auto dryLevelBase         = "dry_level";
    inline constexpr auto pitchAEnableBase     = "pitch_a_enable";
    inline constexpr auto pitchASemitonesBase  = "pitch_a_semitones";
    inline constexpr auto pitchALevelBase      = "pitch_a_level";
    inline constexpr auto pitchBEnableBase     = "pitch_b_enable";
    inline constexpr auto pitchBSemitonesBase  = "pitch_b_semitones";
    inline constexpr auto pitchBLevelBase      = "pitch_b_level";
    inline constexpr auto pitchFormantBase     = "pitch_formant";
}

inline juce::String sideId(const char* base, Side side)
{
    return juce::String(base) + (side == Side::Left ? "_l" : "_r");
}

inline juce::String sideLabel(const char* base, Side side)
{
    return juce::String(base) + (side == Side::Left ? " L" : " R");
}

inline const char* bandName(Band band)
{
    return band == Band::Mid ? "mid" : "high";
}

inline const char* bandLabel(Band band)
{
    return band == Band::Mid ? "Mid" : "High";
}

inline juce::String bandSideId(const char* base, Band band, Side side)
{
    return juce::String(bandName(band)) + "_" + base + (side == Side::Left ? "_l" : "_r");
}

inline juce::String bandSideLabel(const char* base, Band band, Side side)
{
    return juce::String(bandLabel(band)) + " " + base + (side == Side::Left ? " L" : " R");
}

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

} // namespace ydna
