#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace ydna::gui
{

/** Schlichte, eigene Farbwelt statt des JUCE-Defaults. Bewusst zurückhaltend
    gehalten, damit die erste Oberflächenversion einfach bleibt. */
class StereodriveLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    StereodriveLookAndFeel();
};

} // namespace ydna::gui
