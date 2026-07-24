#include "GatePanel.h"

namespace ydna::gui
{

namespace
{
    void setupRotary(juce::Slider& s)
    {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 18);
    }

    void setupLabel(juce::Label& l, const juce::String& text)
    {
        l.setText(text, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
    }
}

GatePanel::GatePanel(juce::AudioProcessorValueTreeState& apvts)
{
    titleLabel.setText("NOISE GATE", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    titleLabel.setFont(juce::Font(juce::FontOptions(18.0f, juce::Font::bold)));
    addAndMakeVisible(titleLabel);

    addAndMakeVisible(enableButton);

    for (auto* s : { &thresholdSlider, &ratioSlider, &attackSlider, &releaseSlider })
    {
        setupRotary(*s);
        addAndMakeVisible(*s);
    }

    setupLabel(thresholdLabel, "Threshold");
    setupLabel(ratioLabel, "Ratio");
    setupLabel(attackLabel, "Attack");
    setupLabel(releaseLabel, "Release");

    for (auto* l : { &thresholdLabel, &ratioLabel, &attackLabel, &releaseLabel })
        addAndMakeVisible(*l);

    enableAttachment    = std::make_unique<ButtonAttachment>(apvts, ParamID::gateEnable, enableButton);
    thresholdAttachment = std::make_unique<SliderAttachment>(apvts, ParamID::gateThreshold, thresholdSlider);
    ratioAttachment     = std::make_unique<SliderAttachment>(apvts, ParamID::gateRatio, ratioSlider);
    attackAttachment    = std::make_unique<SliderAttachment>(apvts, ParamID::gateAttack, attackSlider);
    releaseAttachment   = std::make_unique<SliderAttachment>(apvts, ParamID::gateRelease, releaseSlider);
}

void GatePanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4.0f);
    g.setColour(findColour(juce::GroupComponent::outlineColourId));
    g.drawRoundedRectangle(bounds, 8.0f, 1.5f);
}

void GatePanel::resized()
{
    auto area = getLocalBounds().reduced(16, 10);

    auto titleRow = area.removeFromTop(28);
    titleLabel.setBounds(titleRow.removeFromLeft(titleRow.getWidth() / 2));
    enableButton.setBounds(titleRow);
    area.removeFromTop(8);

    const auto knobSize = juce::jmin(area.getHeight() - 24, 76);
    const auto colWidth = area.getWidth() / 4;

    for (auto& [slider, label] : { std::pair{ &thresholdSlider, &thresholdLabel },
                                   std::pair{ &ratioSlider, &ratioLabel },
                                   std::pair{ &attackSlider, &attackLabel },
                                   std::pair{ &releaseSlider, &releaseLabel } })
    {
        auto col = area.removeFromLeft(colWidth);
        slider->setBounds(col.removeFromTop(knobSize).withSizeKeepingCentre(knobSize, knobSize));
        label->setBounds(col);
    }
}

} // namespace ydna::gui
