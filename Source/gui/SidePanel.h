#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "../Parameters.h"

namespace ydna::gui
{

/** Eine Band-Seite: Drive, Voicing (Marshall/Mesa), Tone, Level, Pan. Wird
    fuer Mid (includePitch=false) und High (includePitch=true) verwendet;
    High bekommt zusaetzlich Dry-Level und die beiden Pitch-Stimmen samt
    Formant-Erhalt-Toggle. */
class SidePanel final : public juce::Component
{
public:
    SidePanel(juce::AudioProcessorValueTreeState& apvts, Band band, Side side, bool includePitch);

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    Band band;
    Side side;
    bool includePitch;

    juce::Label titleLabel;

    juce::Slider driveSlider, toneSlider, levelSlider, panSlider;
    juce::Label driveLabel, toneLabel, levelLabel, panLabel;
    juce::ComboBox voicingBox;

    juce::Slider dryLevelSlider, pitchASlider, pitchALevelSlider, pitchBSlider, pitchBLevelSlider;
    juce::Label dryLevelLabel, pitchALabel, pitchALevelLabel, pitchBLabel, pitchBLevelLabel;
    juce::ToggleButton pitchAEnableButton { "Voice A" }, pitchBEnableButton { "Voice B" }, formantButton { "Formant" };

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment  = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<SliderAttachment> driveAttachment, toneAttachment, levelAttachment, panAttachment;
    std::unique_ptr<ComboAttachment> voicingAttachment;

    std::unique_ptr<SliderAttachment> dryLevelAttachment, pitchAAttachment, pitchALevelAttachment,
        pitchBAttachment, pitchBLevelAttachment;
    std::unique_ptr<ButtonAttachment> pitchAEnableAttachment, pitchBEnableAttachment, formantAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SidePanel)
};

} // namespace ydna::gui
