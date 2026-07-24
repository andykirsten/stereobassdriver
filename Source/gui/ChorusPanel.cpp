#include "ChorusPanel.h"

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

ChorusPanel::ChorusPanel(juce::AudioProcessorValueTreeState& apvts)
{
    titleLabel.setText("CHORUS / DOUBLER", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::Font(juce::FontOptions(18.0f, juce::Font::bold)));
    addAndMakeVisible(titleLabel);

    addAndMakeVisible(enableButton);

    for (auto* s : { &rateSlider, &depthSlider, &centreDelaySlider, &feedbackSlider, &mixSlider })
    {
        setupRotary(*s);
        addAndMakeVisible(*s);
    }

    setupLabel(rateLabel, "Rate");
    setupLabel(depthLabel, "Depth");
    setupLabel(centreDelayLabel, "Delay");
    setupLabel(feedbackLabel, "Feedback");
    setupLabel(mixLabel, "Mix");

    for (auto* l : { &rateLabel, &depthLabel, &centreDelayLabel, &feedbackLabel, &mixLabel })
        addAndMakeVisible(*l);

    enableAttachment      = std::make_unique<ButtonAttachment>(apvts, ParamID::chorusEnable, enableButton);
    rateAttachment        = std::make_unique<SliderAttachment>(apvts, ParamID::chorusRate, rateSlider);
    depthAttachment       = std::make_unique<SliderAttachment>(apvts, ParamID::chorusDepth, depthSlider);
    centreDelayAttachment = std::make_unique<SliderAttachment>(apvts, ParamID::chorusCentreDelay, centreDelaySlider);
    feedbackAttachment    = std::make_unique<SliderAttachment>(apvts, ParamID::chorusFeedback, feedbackSlider);
    mixAttachment         = std::make_unique<SliderAttachment>(apvts, ParamID::chorusMix, mixSlider);
}

void ChorusPanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4.0f);
    g.setColour(findColour(juce::GroupComponent::outlineColourId));
    g.drawRoundedRectangle(bounds, 8.0f, 1.5f);
}

void ChorusPanel::resized()
{
    auto area = getLocalBounds().reduced(16);

    titleLabel.setBounds(area.removeFromTop(28));
    area.removeFromTop(8);

    enableButton.setBounds(area.removeFromTop(24));
    area.removeFromTop(12);

    auto placeRow = [&](std::initializer_list<std::pair<juce::Slider*, juce::Label*>> items)
    {
        const auto knobSize = juce::jmin(area.getWidth() / static_cast<int>(items.size()) - 8, 76);
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

    placeRow({ { &rateSlider, &rateLabel }, { &depthSlider, &depthLabel } });
    placeRow({ { &centreDelaySlider, &centreDelayLabel }, { &feedbackSlider, &feedbackLabel } });
    placeRow({ { &mixSlider, &mixLabel } });
}

} // namespace ydna::gui
