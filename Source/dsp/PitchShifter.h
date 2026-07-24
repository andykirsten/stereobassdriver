#pragma once

#include <juce_dsp/juce_dsp.h>
#include <signalsmith-stretch/signalsmith-stretch.h>

namespace ydna::dsp
{

/** Wrapper um Signalsmith Stretch für reinen Pitch-Shift (kein Time-Stretch):
    Input- und Output-Blockgröße sind pro process()-Aufruf identisch, die
    Bibliothek übernimmt intern Pufferung/Latenz. Läuft immer mit (auch bei
    0 Halbtönen), damit die von der Engine erzeugte Latenz konstant bleibt
    und der Host keine wechselnde PDC-Latenz sieht. */
class PitchShifter
{
public:
    void prepare(const juce::dsp::ProcessSpec& monoSpec);
    void reset();

    void setTransposeSemitones(float semitones) noexcept;

    /** Haelt die Formanten (Klangkoerper) unabhaengig von der Transposition
        an ihrer urspruenglichen Frequenzlage fest, statt sie mit hochzu-
        pitchen ("Chipmunk"-Effekt). Ruft intern setFormantSemitones(0,
        compensatePitch=enabled) auf der Stretch-Instanz auf. */
    void setFormantCompensation(bool enabled) noexcept;

    /** Verarbeitet buffer in-place (mono). */
    void process(juce::AudioBuffer<float>& buffer);

    int getLatencySamples() const;

private:
    signalsmith::stretch::SignalsmithStretch<float> stretch;
    juce::AudioBuffer<float> scratchOutput;
    float currentSemitones = 0.0f;
    bool formantCompensationEnabled = false;
};

} // namespace ydna::dsp
