#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "PluginProcessor.h"
#include "gui/LookAndFeel.h"
#include "gui/BandPanel.h"
#include "gui/SidePanel.h"
#include "gui/GatePanel.h"
#include "gui/ChorusPanel.h"
#include "gui/CabPanel.h"

namespace ydna
{

/** Tabs sind in Signalfluss-Reihenfolge angeordnet:
    In & Bass (Gate -> Crossover -> Kompressor) -> Mid -> High -> Bus & Out
    (Chorus -> Cab -> Mix/Output/Bypass). */
class StereodriveEditor final : public juce::AudioProcessorEditor
{
public:
    explicit StereodriveEditor(StereodriveProcessor&);
    ~StereodriveEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    StereodriveProcessor& processorRef;

    gui::StereodriveLookAndFeel lookAndFeel;

    juce::TabbedComponent tabs { juce::TabbedButtonBar::TabsAtTop };

    // "In & Bass" tab: Eingangs-Gate + Crossover/Kompressor (Low-Band, mono)
    juce::Component inBassTab;
    gui::GatePanel gatePanel;
    gui::BandPanel bandPanel;

    // "Mid" tab: Mid-Band-Seitenregler (Gitarrenkoerper, kein Pitch)
    juce::Component midTab;
    gui::SidePanel midLeftPanel;
    gui::SidePanel midRightPanel;

    // "High" tab: High-Band-Seitenregler (mit Pitch A/B + Formant)
    juce::Component highTab;
    gui::SidePanel highLeftPanel;
    gui::SidePanel highRightPanel;

    // "Bus & Out" tab: Chorus + Cab (auf dem Gitarren-Bus) + Mix/Output/Bypass
    juce::Component busOutTab;
    gui::ChorusPanel chorusPanel;
    gui::CabPanel cabPanel;
    juce::Slider mixSlider, outputSlider;
    juce::Label mixLabel, outputLabel;
    juce::ToggleButton bypassButton { "Bypass" };

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment, outputAttachment;
    std::unique_ptr<ButtonAttachment> bypassAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereodriveEditor)
};

} // namespace ydna
