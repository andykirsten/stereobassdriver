#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "../Parameters.h"

namespace ydna::gui
{

/** Zentrale Bass-Sektion: Trennfrequenz und Kompressor für das Low-Band. */
class BandPanel final : public juce::Component
{
public:
    explicit BandPanel(juce::AudioProcessorValueTreeState& apvts);

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    juce::Label titleLabel;

    juce::Slider xoverLoSlider, xoverHiSlider, highpassSlider, thresholdSlider, ratioSlider, attackSlider, releaseSlider, makeupSlider, lowLevelSlider;
    juce::Label xoverLoLabel, xoverHiLabel, highpassLabel, thresholdLabel, ratioLabel, attackLabel, releaseLabel, makeupLabel, lowLevelLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> xoverLoAttachment, xoverHiAttachment, highpassAttachment, thresholdAttachment, ratioAttachment,
        attackAttachment, releaseAttachment, makeupAttachment, lowLevelAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BandPanel)
};

} // namespace ydna::gui
