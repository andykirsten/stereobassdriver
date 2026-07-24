#include "PluginEditor.h"

namespace ydna
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

StereodriveEditor::StereodriveEditor(StereodriveProcessor& p)
    : AudioProcessorEditor(&p),
      processorRef(p),
      gatePanel(p.apvts),
      bandPanel(p.apvts),
      midLeftPanel(p.apvts, Band::Mid, Side::Left, false),
      midRightPanel(p.apvts, Band::Mid, Side::Right, false),
      highLeftPanel(p.apvts, Band::High, Side::Left, true),
      highRightPanel(p.apvts, Band::High, Side::Right, true),
      chorusPanel(p.apvts),
      cabPanel(p.apvts, p)
{
    setLookAndFeel(&lookAndFeel);

    inBassTab.addAndMakeVisible(gatePanel);
    inBassTab.addAndMakeVisible(bandPanel);

    midTab.addAndMakeVisible(midLeftPanel);
    midTab.addAndMakeVisible(midRightPanel);

    highTab.addAndMakeVisible(highLeftPanel);
    highTab.addAndMakeVisible(highRightPanel);

    busOutTab.addAndMakeVisible(chorusPanel);
    busOutTab.addAndMakeVisible(cabPanel);

    setupRotary(mixSlider);
    setupRotary(outputSlider);
    busOutTab.addAndMakeVisible(mixSlider);
    busOutTab.addAndMakeVisible(outputSlider);

    setupLabel(mixLabel, "Mix");
    setupLabel(outputLabel, "Output");
    busOutTab.addAndMakeVisible(mixLabel);
    busOutTab.addAndMakeVisible(outputLabel);

    busOutTab.addAndMakeVisible(bypassButton);

    mixAttachment    = std::make_unique<SliderAttachment>(p.apvts, ParamID::mix, mixSlider);
    outputAttachment = std::make_unique<SliderAttachment>(p.apvts, ParamID::outputGain, outputSlider);
    bypassAttachment = std::make_unique<ButtonAttachment>(p.apvts, ParamID::bypass, bypassButton);

    tabs.addTab("In & Bass", findColour(juce::ResizableWindow::backgroundColourId), &inBassTab, false);
    tabs.addTab("Mid", findColour(juce::ResizableWindow::backgroundColourId), &midTab, false);
    tabs.addTab("High", findColour(juce::ResizableWindow::backgroundColourId), &highTab, false);
    tabs.addTab("Bus & Out", findColour(juce::ResizableWindow::backgroundColourId), &busOutTab, false);
    addAndMakeVisible(tabs);

    setResizable(true, true);
    setResizeLimits(820, 560, 1500, 1050);
    setSize(1020, 700);
}

StereodriveEditor::~StereodriveEditor()
{
    setLookAndFeel(nullptr);
}

void StereodriveEditor::paint(juce::Graphics& g)
{
    g.fillAll(findColour(juce::ResizableWindow::backgroundColourId));
}

void StereodriveEditor::resized()
{
    tabs.setBounds(getLocalBounds().reduced(8));

    {
        auto area = inBassTab.getLocalBounds().reduced(12);
        auto gateArea = area.removeFromTop(156);
        area.removeFromTop(8);

        gatePanel.setBounds(gateArea);
        bandPanel.setBounds(area);
    }

    {
        auto area = midTab.getLocalBounds().reduced(12);
        const auto colWidth = area.getWidth() / 2;
        midLeftPanel.setBounds(area.removeFromLeft(colWidth).reduced(6));
        midRightPanel.setBounds(area.reduced(6));
    }

    {
        auto area = highTab.getLocalBounds().reduced(12);
        const auto colWidth = area.getWidth() / 2;
        highLeftPanel.setBounds(area.removeFromLeft(colWidth).reduced(6));
        highRightPanel.setBounds(area.reduced(6));
    }

    {
        auto area = busOutTab.getLocalBounds().reduced(12);

        auto bottomRow = area.removeFromBottom(90);
        area.removeFromBottom(8);

        const auto colWidth = area.getWidth() / 2;
        chorusPanel.setBounds(area.removeFromLeft(colWidth).reduced(6));
        cabPanel.setBounds(area.reduced(6));

        const auto knobSize = 64;
        auto mixArea = bottomRow.removeFromLeft(bottomRow.getWidth() / 3);
        mixSlider.setBounds(mixArea.removeFromTop(knobSize).withSizeKeepingCentre(knobSize, knobSize));
        mixLabel.setBounds(mixArea);

        auto outputArea = bottomRow.removeFromLeft(bottomRow.getWidth() / 2);
        outputSlider.setBounds(outputArea.removeFromTop(knobSize).withSizeKeepingCentre(knobSize, knobSize));
        outputLabel.setBounds(outputArea);

        bypassButton.setBounds(bottomRow.withSizeKeepingCentre(bottomRow.getWidth(), 28));
    }
}

} // namespace ydna
