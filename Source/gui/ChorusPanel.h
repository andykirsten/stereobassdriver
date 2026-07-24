#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "../Parameters.h"

namespace ydna::gui
{

/** Doubler/Chorus, auf dem Gitarren-Bus (Mid+High) - laeuft direkt vor der
    Cabinet-Simulation und gehoert daher auf denselben Tab wie diese. */
class ChorusPanel final : public juce::Component
{
public:
    explicit ChorusPanel(juce::AudioProcessorValueTreeState& apvts);

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    juce::Label titleLabel;
    juce::ToggleButton enableButton { "Chorus Enable" };

    juce::Slider rateSlider, depthSlider, centreDelaySlider, feedbackSlider, mixSlider;
    juce::Label rateLabel, depthLabel, centreDelayLabel, feedbackLabel, mixLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<ButtonAttachment> enableAttachment;
    std::unique_ptr<SliderAttachment> rateAttachment, depthAttachment, centreDelayAttachment,
        feedbackAttachment, mixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChorusPanel)
};

} // namespace ydna::gui
