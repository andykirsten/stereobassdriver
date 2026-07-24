#include "SidePanel.h"

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

SidePanel::SidePanel(juce::AudioProcessorValueTreeState& apvts, Band bandIn, Side sideIn, bool includePitchIn)
    : band(bandIn), side(sideIn), includePitch(includePitchIn)
{
    titleLabel.setText(juce::String(bandLabel(band)).toUpperCase() + " " + (side == Side::Left ? "L" : "R"),
                        juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::Font(juce::FontOptions(18.0f, juce::Font::bold)));
    addAndMakeVisible(titleLabel);

    for (auto* s : { &driveSlider, &toneSlider, &levelSlider, &panSlider })
    {
        setupRotary(*s);
        addAndMakeVisible(*s);
    }

    setupLabel(driveLabel, "Drive");
    setupLabel(toneLabel, "Tone");
    setupLabel(levelLabel, "Level");
    setupLabel(panLabel, "Pan");
    for (auto* l : { &driveLabel, &toneLabel, &levelLabel, &panLabel })
        addAndMakeVisible(*l);

    voicingBox.addItem("Marshall", 1);
    voicingBox.addItem("Mesa", 2);
    addAndMakeVisible(voicingBox);

    driveAttachment  = std::make_unique<SliderAttachment>(apvts, bandSideId(ParamID::odDriveBase, band, side), driveSlider);
    toneAttachment   = std::make_unique<SliderAttachment>(apvts, bandSideId(ParamID::odToneBase, band, side), toneSlider);
    levelAttachment  = std::make_unique<SliderAttachment>(apvts, bandSideId(ParamID::odLevelBase, band, side), levelSlider);
    panAttachment    = std::make_unique<SliderAttachment>(apvts, bandSideId(ParamID::panBase, band, side), panSlider);
    voicingAttachment = std::make_unique<ComboAttachment>(apvts, bandSideId(ParamID::odVoicingBase, band, side), voicingBox);

    if (! includePitch)
        return;

    for (auto* s : { &dryLevelSlider, &pitchASlider, &pitchALevelSlider, &pitchBSlider, &pitchBLevelSlider })
    {
        setupRotary(*s);
        addAndMakeVisible(*s);
    }

    setupLabel(dryLevelLabel, "Dry");
    setupLabel(pitchALabel, "A Semi");
    setupLabel(pitchALevelLabel, "A Level");
    setupLabel(pitchBLabel, "B Semi");
    setupLabel(pitchBLevelLabel, "B Level");
    for (auto* l : { &dryLevelLabel, &pitchALabel, &pitchALevelLabel, &pitchBLabel, &pitchBLevelLabel })
        addAndMakeVisible(*l);

    addAndMakeVisible(pitchAEnableButton);
    addAndMakeVisible(pitchBEnableButton);
    addAndMakeVisible(formantButton);

    dryLevelAttachment    = std::make_unique<SliderAttachment>(apvts, bandSideId(ParamID::dryLevelBase, band, side), dryLevelSlider);
    pitchAAttachment      = std::make_unique<SliderAttachment>(apvts, bandSideId(ParamID::pitchASemitonesBase, band, side), pitchASlider);
    pitchALevelAttachment = std::make_unique<SliderAttachment>(apvts, bandSideId(ParamID::pitchALevelBase, band, side), pitchALevelSlider);
    pitchBAttachment      = std::make_unique<SliderAttachment>(apvts, bandSideId(ParamID::pitchBSemitonesBase, band, side), pitchBSlider);
    pitchBLevelAttachment = std::make_unique<SliderAttachment>(apvts, bandSideId(ParamID::pitchBLevelBase, band, side), pitchBLevelSlider);

    pitchAEnableAttachment = std::make_unique<ButtonAttachment>(apvts, bandSideId(ParamID::pitchAEnableBase, band, side), pitchAEnableButton);
    pitchBEnableAttachment = std::make_unique<ButtonAttachment>(apvts, bandSideId(ParamID::pitchBEnableBase, band, side), pitchBEnableButton);
    formantAttachment      = std::make_unique<ButtonAttachment>(apvts, bandSideId(ParamID::pitchFormantBase, band, side), formantButton);
}

void SidePanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4.0f);
    g.setColour(findColour(juce::GroupComponent::outlineColourId));
    g.drawRoundedRectangle(bounds, 8.0f, 1.5f);
}

void SidePanel::resized()
{
    auto area = getLocalBounds().reduced(12);

    titleLabel.setBounds(area.removeFromTop(28));
    area.removeFromTop(8);

    voicingBox.setBounds(area.removeFromTop(28));
    area.removeFromTop(12);

    const auto knobSize = juce::jmin(area.getWidth() / 2 - 8, 84);

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

    placeRow({ { &driveSlider, &driveLabel }, { &toneSlider, &toneLabel } });
    placeRow({ { &levelSlider, &levelLabel }, { &panSlider, &panLabel } });

    if (! includePitch)
        return;

    area.removeFromTop(4);
    auto formantRow = area.removeFromTop(24);
    formantButton.setBounds(formantRow);
    area.removeFromTop(6);

    auto voiceARow = area.removeFromTop(20);
    pitchAEnableButton.setBounds(voiceARow);
    placeRow({ { &pitchASlider, &pitchALabel }, { &pitchALevelSlider, &pitchALevelLabel } });

    auto voiceBRow = area.removeFromTop(20);
    pitchBEnableButton.setBounds(voiceBRow);
    placeRow({ { &pitchBSlider, &pitchBLabel }, { &pitchBLevelSlider, &pitchBLevelLabel } });

    placeRow({ { &dryLevelSlider, &dryLevelLabel } });
}

} // namespace ydna::gui
