#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "../Parameters.h"

namespace ydna
{
class StereodriveProcessor;
}

namespace ydna::gui
{

/** Cabinet-/IR-Simulation: eingebaute Presets, Level, Enable, und ein
    Datei-Loader fuer echte Cab-IRs (ruft direkt auf dem Message-Thread in
    den Processor hinein, ausserhalb des APVTS-Attachments). */
class CabPanel final : public juce::Component
{
public:
    CabPanel(juce::AudioProcessorValueTreeState& apvts, StereodriveProcessor& processor);

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    StereodriveProcessor& processor;

    juce::Label titleLabel;
    juce::ToggleButton enableButton { "Cab Enable" };
    juce::ComboBox modelBox;
    juce::Label modelLabel;
    juce::Slider levelSlider;
    juce::Label levelLabel;
    juce::TextButton loadIrButton { "Load IR..." };
    std::unique_ptr<juce::FileChooser> fileChooser;

    using ComboAttachment  = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<ButtonAttachment> enableAttachment;
    std::unique_ptr<ComboAttachment> modelAttachment;
    std::unique_ptr<SliderAttachment> levelAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CabPanel)
};

} // namespace ydna::gui
