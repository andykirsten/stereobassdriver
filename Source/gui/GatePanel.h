#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "../Parameters.h"

namespace ydna::gui
{

/** Noise Gate, am Signal-Eingang (vor dem Crossover) - gehoert daher auf
    denselben Tab wie das Bass-/Crossover-Panel, nicht zu den Bus-Effekten. */
class GatePanel final : public juce::Component
{
public:
    explicit GatePanel(juce::AudioProcessorValueTreeState& apvts);

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    juce::Label titleLabel;
    juce::ToggleButton enableButton { "Gate Enable" };

    juce::Slider thresholdSlider, ratioSlider, attackSlider, releaseSlider;
    juce::Label thresholdLabel, ratioLabel, attackLabel, releaseLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<ButtonAttachment> enableAttachment;
    std::unique_ptr<SliderAttachment> thresholdAttachment, ratioAttachment, attackAttachment, releaseAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GatePanel)
};

} // namespace ydna::gui
