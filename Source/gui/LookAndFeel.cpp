#include "LookAndFeel.h"

namespace ydna::gui
{

StereodriveLookAndFeel::StereodriveLookAndFeel()
{
    const auto background = juce::Colour::fromRGB(0x1c, 0x1e, 0x22);
    const auto panel      = juce::Colour::fromRGB(0x26, 0x29, 0x2f);
    const auto accent     = juce::Colour::fromRGB(0xe8, 0x7a, 0x2e); // warmes Orange, Amp-artig
    const auto text       = juce::Colour::fromRGB(0xe8, 0xe6, 0xe1);

    setColour(juce::ResizableWindow::backgroundColourId, background);
    setColour(juce::Slider::rotarySliderFillColourId, accent);
    setColour(juce::Slider::rotarySliderOutlineColourId, panel.brighter(0.2f));
    setColour(juce::Slider::thumbColourId, accent);
    setColour(juce::Slider::textBoxTextColourId, text);
    setColour(juce::Slider::textBoxOutlineColourId, panel);
    setColour(juce::Label::textColourId, text);
    setColour(juce::ComboBox::backgroundColourId, panel);
    setColour(juce::ComboBox::textColourId, text);
    setColour(juce::ToggleButton::textColourId, text);
    setColour(juce::ToggleButton::tickColourId, accent);
    setColour(juce::GroupComponent::textColourId, accent);
    setColour(juce::GroupComponent::outlineColourId, panel.brighter(0.3f));
}

} // namespace ydna::gui
