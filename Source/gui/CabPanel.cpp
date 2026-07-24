#include "CabPanel.h"
#include "../PluginProcessor.h"

namespace ydna::gui
{

CabPanel::CabPanel(juce::AudioProcessorValueTreeState& apvts, StereodriveProcessor& processorIn)
    : processor(processorIn)
{
    titleLabel.setText("CABINET", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::Font(juce::FontOptions(18.0f, juce::Font::bold)));
    addAndMakeVisible(titleLabel);

    addAndMakeVisible(enableButton);

    modelBox.addItem("Bright 4x12", 1);
    modelBox.addItem("Dark 2x12", 2);
    modelBox.addItem("Mid Focus", 3);
    modelBox.addItem("Custom", 4);
    addAndMakeVisible(modelBox);

    modelLabel.setText("Model", juce::dontSendNotification);
    modelLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(modelLabel);

    levelSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    levelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 18);
    addAndMakeVisible(levelSlider);

    levelLabel.setText("Level", juce::dontSendNotification);
    levelLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(levelLabel);

    addAndMakeVisible(loadIrButton);
    loadIrButton.onClick = [this]
    {
        fileChooser = std::make_unique<juce::FileChooser>("IR-Datei laden...", juce::File(), "*.wav;*.aiff;*.aif");
        const auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
        fileChooser->launchAsync(flags, [this](const juce::FileChooser& chooser)
        {
            const auto file = chooser.getResult();
            if (file.existsAsFile())
                processor.loadCustomCabImpulseResponse(file);
        });
    };

    enableAttachment = std::make_unique<ButtonAttachment>(apvts, ParamID::cabEnable, enableButton);
    modelAttachment  = std::make_unique<ComboAttachment>(apvts, ParamID::cabSelect, modelBox);
    levelAttachment  = std::make_unique<SliderAttachment>(apvts, ParamID::cabLevel, levelSlider);
}

void CabPanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4.0f);
    g.setColour(findColour(juce::GroupComponent::outlineColourId));
    g.drawRoundedRectangle(bounds, 8.0f, 1.5f);
}

void CabPanel::resized()
{
    auto area = getLocalBounds().reduced(16);

    titleLabel.setBounds(area.removeFromTop(28));
    area.removeFromTop(12);

    enableButton.setBounds(area.removeFromTop(24));
    area.removeFromTop(12);

    modelBox.setBounds(area.removeFromTop(28));
    modelLabel.setBounds(area.removeFromTop(18));
    area.removeFromTop(12);

    const auto knobSize = juce::jmin(area.getWidth(), 90);
    levelSlider.setBounds(area.removeFromTop(knobSize).withSizeKeepingCentre(knobSize, knobSize));
    levelLabel.setBounds(area.removeFromTop(18));
    area.removeFromTop(12);

    loadIrButton.setBounds(area.removeFromTop(28));
}

} // namespace ydna::gui
