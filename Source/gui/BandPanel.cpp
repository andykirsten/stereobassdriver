#include "BandPanel.h"

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

BandPanel::BandPanel(juce::AudioProcessorValueTreeState& apvts)
{
    titleLabel.setText("BASS", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::Font(juce::FontOptions(18.0f, juce::Font::bold)));
    addAndMakeVisible(titleLabel);

    for (auto* s : { &xoverLoSlider, &xoverHiSlider, &highpassSlider, &thresholdSlider, &ratioSlider, &attackSlider, &releaseSlider, &makeupSlider, &lowLevelSlider })
    {
        setupRotary(*s);
        addAndMakeVisible(*s);
    }

    setupLabel(xoverLoLabel, "Xover Lo");
    setupLabel(xoverHiLabel, "Xover Hi");
    setupLabel(highpassLabel, "Highpass Hz");
    setupLabel(thresholdLabel, "Threshold");
    setupLabel(ratioLabel, "Ratio");
    setupLabel(attackLabel, "Attack");
    setupLabel(releaseLabel, "Release");
    setupLabel(makeupLabel, "Makeup");
    setupLabel(lowLevelLabel, "Level");

    for (auto* l : { &xoverLoLabel, &xoverHiLabel, &highpassLabel, &thresholdLabel, &ratioLabel, &attackLabel, &releaseLabel, &makeupLabel, &lowLevelLabel })
        addAndMakeVisible(*l);

    xoverLoAttachment   = std::make_unique<SliderAttachment>(apvts, ParamID::xoverFreqLo, xoverLoSlider);
    xoverHiAttachment   = std::make_unique<SliderAttachment>(apvts, ParamID::xoverFreqHi, xoverHiSlider);
    highpassAttachment  = std::make_unique<SliderAttachment>(apvts, ParamID::compHighpass, highpassSlider);
    thresholdAttachment = std::make_unique<SliderAttachment>(apvts, ParamID::compThresh, thresholdSlider);
    ratioAttachment     = std::make_unique<SliderAttachment>(apvts, ParamID::compRatio, ratioSlider);
    attackAttachment    = std::make_unique<SliderAttachment>(apvts, ParamID::compAttack, attackSlider);
    releaseAttachment   = std::make_unique<SliderAttachment>(apvts, ParamID::compRelease, releaseSlider);
    makeupAttachment    = std::make_unique<SliderAttachment>(apvts, ParamID::compMakeup, makeupSlider);
    lowLevelAttachment  = std::make_unique<SliderAttachment>(apvts, ParamID::lowLevel, lowLevelSlider);
}

void BandPanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4.0f);
    g.setColour(findColour(juce::GroupComponent::outlineColourId));
    g.drawRoundedRectangle(bounds, 8.0f, 1.5f);
}

void BandPanel::resized()
{
    auto area = getLocalBounds().reduced(12);

    titleLabel.setBounds(area.removeFromTop(28));
    area.removeFromTop(8);

    const auto knobSize = juce::jmin(area.getWidth() / 3 - 8, 76);

    auto placeRow = [&](std::initializer_list<std::pair<juce::Slider*, juce::Label*>> items)
    {
        auto row = area.removeFromTop(knobSize + 24);
        const auto colWidth = row.getWidth() / static_cast<int>(items.size());
        for (auto& [slider, label] : items)
        {
            auto col = row.removeFromLeft(colWidth);
            slider->setBounds(col.removeFromTop(knobSize).withSizeKeepingCentre(knobSize, knobSize));
            label->setBounds(col);
        }
        area.removeFromTop(10);
    };

    placeRow({ { &xoverLoSlider, &xoverLoLabel }, { &xoverHiSlider, &xoverHiLabel } });
    placeRow({ { &highpassSlider, &highpassLabel }, { &thresholdSlider, &thresholdLabel } });
    placeRow({ { &ratioSlider, &ratioLabel } });
    placeRow({ { &attackSlider, &attackLabel }, { &releaseSlider, &releaseLabel } });
    placeRow({ { &makeupSlider, &makeupLabel }, { &lowLevelSlider, &lowLevelLabel } });
}

} // namespace ydna::gui
