# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

`ydna.stereodrive` is a JUCE (C++20) audio plugin: an overdrive/saturation effect for
electric bass. It splits the signal into a mono low band (heavily compressed) and a stereo
high band whose left/right sides get independent overdrive voicing, tone, level, optional
pitch-up, and pan. Built with CMake, targeting VST3 + Standalone on Windows (+AU on macOS).

## Build

CMake and the MSVC toolchain are not on `PATH` by default in this environment. Locate them
under the Visual Studio install before invoking `cmake`, e.g. (adjust for the actual VS
edition/version installed):

```bash
find "/c/Program Files/Microsoft Visual Studio" -iname "cmake.exe"
export PATH="<dir containing cmake.exe>:$PATH"
```

Configure (first time / after CMakeLists.txt changes) and build:

```bash
cmake -B build
cmake --build build --config Debug
```

JUCE 8.0.12 and Signalsmith Stretch are pulled via `FetchContent` on first configure (no
submodules/vendoring). `COPY_PLUGIN_AFTER_BUILD` is `FALSE` in `CMakeLists.txt` because the
default VST3 system folder requires admin rights to write to in this environment — copy the
built `.vst3` manually if you need it in a DAW's plugin path.

Build outputs:
- `build/ydna_stereodrive_artefacts/Debug/VST3/stereodrive.vst3`
- `build/ydna_stereodrive_artefacts/Debug/Standalone/stereodrive.exe`

There is no test suite in this repo. Verification is: does it build, does the Standalone
host launch and run without crashing, and manual listening/parameter checks (no automated
DSP tests exist despite the `Tests/` idea mentioned in planning notes — that directory was
never created).

## Architecture

### Signal flow (`Source/PluginProcessor.cpp::processBlock`)

```
Input (mono DI duplicated to stereo if needed)
        │
   Crossover (movable crossover_freq, Linkwitz-Riley 24dB/oct)
        ├── LOW  → highpass (comp_highpass, default 50Hz) → Compressor → makeup/level → mono, center
        └── HIGH → per side (L, R):
                     Overdrive (voicing Marshall/Mesa, drive, tone, level)
                     → PitchShifter (Signalsmith Stretch, 0..+12 semitones, always running)
                     → pan (0-100%, equal-power law) into stereo highBandMixed
        │
   low (delay-compensated) + highBandMixed → dry/wet mix → output gain → output
```

Key invariant: the **pitch shifter always processes, even at 0 semitones**, so its latency
never changes with automation. `updateLatencyCompensation()` reads the fixed
overdrive-oversampling + pitch-shifter latency once and delays the low band and the dry
signal (via `juce::dsp::DelayLine`) by that same amount, then reports it via
`setLatencySamples()`. This avoids DAW PDC glitches when a user toggles pitch on/off — do
not make the pitch shifter's latency conditional on `pitch_enable`.

Another invariant to preserve when touching `processSide` in `processBlock`: the per-side
lambda reads its input from `highBand` (the crossover's untouched stereo output) but writes
its panned result into a separate accumulation buffer `highBandMixed`, not back into
`highBand`. Since panning can route a side's signal into *both* output channels, writing
back into `highBand` would let one side's output contaminate the other side's still-to-be-read
input. `highBandMixed` is cleared once before both `processSide` calls run.

All working buffers (`workBuffer`, `lowBand`, `highBand`, `highBandMixed`, delay-line
scratch buffers, per-side mono buffers) are preallocated in `prepareToPlay` and resized with
`setSize(..., avoidReallocating=true)` inside `processBlock` — never allocate in the audio
callback.

### Module layout

- **`Parameters.h`/`.cpp`** — single source of truth for parameter IDs and the
  `AudioProcessorValueTreeState::ParameterLayout`. Per-side parameters (drive, voicing, tone,
  level, pitch enable/semitones, pan) are declared once in `addSideParams()` and instantiated
  for `Side::Left`/`Side::Right` via `sideId()`/`sideLabel()`, which append `_l`/`_r` or
  `" L"`/`" R"`. When adding a new per-side control, extend `addSideParams()` plus the
  `SideParams` struct and `fetchSideParams()` in `PluginProcessor.cpp` — don't duplicate the
  L/R declarations by hand.
- **`dsp/Crossover`** — thin wrapper around two `juce::dsp::LinkwitzRileyFilter` instances
  (one lowpass, one highpass at the same cutoff); LR4 sums flat by construction.
- **`dsp/BandCompressor`** — sums the low band to mono, applies a highpass (cleanup filter,
  removes subsonic content before the compressor's detector sees it), then
  `juce::dsp::Compressor` + makeup gain + output level, and duplicates the mono result back
  across all channels.
- **`dsp/OverdriveStage`** — per-side, mono. Chain: voicing-dependent pre-emphasis (peak
  filter) → drive gain → 2x oversampling → `waveshape()` (tanh-based, symmetric or
  asymmetric depending on voicing) → auto-level compensation (`1/sqrt(driveGain)`, so
  higher drive doesn't just get louder) → post shelf → tone shelf (±6dB tilt at 2.5kHz) →
  output level. Reports oversampling latency via `getLatencySamples()`.
- **`dsp/Voicing`** — pure data + the `waveshape()` saturation curve. "Marshall" and "Mesa"
  are just different `VoicingCoefficients` (pre/post filter freq+gain, drive multiplier,
  asymmetry amount), not separate code paths. Add a new amp character by adding a case here,
  not by branching in `OverdriveStage`.
- **`dsp/PitchShifter`** — wrapper around `signalsmith::stretch::SignalsmithStretch`
  configured for pure pitch-shift (equal input/output block size per call). See the "always
  running" invariant above.
- **`gui/BandPanel`** — center column: crossover, bass highpass, compressor controls, low
  level. **`gui/SidePanel`** — one instance per side: drive, voicing, tone, level, pan,
  pitch enable/semitones. Both use a shared local `setupRotary`/`setupLabel` pattern and bind
  via `AudioProcessorValueTreeState::SliderAttachment`/`ComboBoxAttachment`/`ButtonAttachment`
  — no manual parameter listeners. `PluginEditor` lays out `SidePanel` (L) | `BandPanel` |
  `SidePanel` (R) plus a bottom row (mix, output gain, bypass).

### Adding a new parameter

1. Add the ID to `ParamID` in `Parameters.h` (use `*Base` + `sideId()` if it's per-side).
2. Add the `AudioParameterFloat`/`Bool`/`Choice` in `createParameterLayout()` /
   `addSideParams()` in `Parameters.cpp`.
3. Cache a raw pointer to it in `PluginProcessor` (global: member in the processor;
   per-side: field on `SideParams`, fetched in `fetchSideParams()`).
4. Read it with `->load()` in `processBlock()` and feed it to the relevant DSP class's
   setter.
5. Add a `juce::Slider`/`ComboBox`/`ToggleButton` + attachment in the relevant GUI panel and
   place it in `resized()`.
