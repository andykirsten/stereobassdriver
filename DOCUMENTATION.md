# ydna.stereodrive — Dokumentation (Stand 2026-07-24)

Bass-Overdrive-Plugin (VST3 / Standalone, JUCE + CMake), konzipiert als
„All-in-one Bass→Gitarre"-Sound: ein einzelnes DI-Bass-Signal deckt gleichzeitig das
Bassfundament und ein verzerrtes, gepitchtes „Gitarren"-Register ab.

## Signalfluss

```
Input (mono DI → stereo)
  │
  Noise Gate (stereo, vor allem)
  │
  3-Band-Crossover (Xover Lo / Xover Hi, Linkwitz-Riley 24 dB/oct)
   ├── LOW  → Bass-Highpass → Kompressor → Makeup/Level → mono, mittig
   ├── MID  → pro Seite (L/R): Overdrive (Drive/Voicing/Tone/Level) → Pan → Mid-Summe
   └── HIGH → pro Seite (L/R): Overdrive → Dry + Pitch-Stimme A + Pitch-Stimme B
              (beide Stimmen laufen immer, Formant-Erhalt optional) → Pan → High-Summe
  │
  Gitarren-Bus = Mid-Summe + High-Summe
   → Chorus/Doubler
   → Cabinet-Simulation (eingebaute IRs oder eigene Datei)
  │
  Bass (latenzkompensiert) + Gitarren-Bus → Dry/Wet-Mix → Output-Gain → Bypass → Out
```

**Wichtige Eigenschaft:** Die Pitch-Shifter (Voice A & B im High-Band) laufen technisch
immer mit, auch wenn deaktiviert — dadurch bleibt die an die DAW gemeldete Plugin-Latenz
konstant, egal welche Regler (Gate, Chorus, Cab, Pitch A/B, Formant) ein-/ausgeschaltet
werden. Nur Overdrive-Oversampling und die beiden Pitch-Stimmen tragen zur Latenz bei.

## GUI: Tabs

Die Tabs sind in Signalfluss-Reihenfolge angeordnet (Input → ... → Output), nicht nach
Regler-Art gruppiert:

- **In & Bass** — Noise Gate (Signal-Eingang) + Crossover/Kompressor (Low-Band, mono).
- **Mid** — Mid-Band-Regler links/rechts (Gitarrenkörper-Register, ohne Pitch).
- **High** — High-Band-Regler links/rechts (mit Pitch-Stimme A/B, Formant-Erhalt).
- **Bus & Out** — Chorus/Doubler + Cabinet-Simulation (beide auf dem Gitarren-Bus,
  in Verarbeitungsreihenfolge) + Mix/Output/Bypass (finale Stufe).

## Parameter-Referenz

### Bass (Center, mono)

| Parameter | Bereich | Default | Beschreibung |
|---|---|---|---|
| Crossover Lo | 40–500 Hz (log) | 120 Hz | Trennfrequenz Bass ↔ Mid |
| Crossover Hi | 300–3000 Hz (log) | 800 Hz | Trennfrequenz Mid ↔ High |
| Bass Highpass | 20–200 Hz (log) | 50 Hz | Rumpel-/Subsonic-Filter vor dem Kompressor |
| Bass Threshold | −48–0 dB | −24 dB | Kompressor-Schwelle |
| Bass Ratio | 1–20 | 8 | Kompressor-Verhältnis |
| Bass Attack | 0,1–100 ms (log) | 5 ms | Kompressor-Attack |
| Bass Release | 10–500 ms (log) | 80 ms | Kompressor-Release |
| Bass Makeup | −12–24 dB | 6 dB | Makeup-Gain nach dem Kompressor |
| Bass Level | 0–2× | 1,0 | Ausgangspegel des Low-Bands |

### Noise Gate (Eingang, vor dem Crossover)

| Parameter | Bereich | Default |
|---|---|---|
| Gate Enable | An/Aus | Aus |
| Gate Threshold | −80–0 dB | −50 dB |
| Gate Ratio | 1–20 (log) | 4 |
| Gate Attack | 0,1–100 ms (log) | 2 ms |
| Gate Release | 10–1000 ms (log) | 150 ms |

### Mid-Band (pro Seite L/R — Gitarrenkörper, kein Pitch)

| Parameter | Bereich | Default |
|---|---|---|
| Mid Drive L/R | 0–1 | 0,4 |
| Mid Voicing L/R | Marshall / Mesa | Marshall |
| Mid Tone L/R | 0–1 (±6 dB Tilt @2,5 kHz) | 0,5 |
| Mid Level L/R | 0–2× | 1,0 |
| Mid Pan L/R | 0–100 % (0 = mittig/mono, 100 = hart eigene Seite) | 100 % |

### High-Band (pro Seite L/R — Gitarrensaiten-Register, mit Pitch)

| Parameter | Bereich | Default |
|---|---|---|
| High Drive L/R | 0–1 | 0,4 |
| High Voicing L/R | Marshall / Mesa | Marshall |
| High Tone L/R | 0–1 | 0,5 |
| High Level L/R | 0–2× | 1,0 |
| High Pan L/R | 0–100 % | 100 % |
| High Dry Level L/R | 0–1 | 1,0 | Anteil des ungepitchten Signals in der Mischung |
| High Pitch A Enable L/R | An/Aus | Aus |
| High Pitch A L/R | 0–12 Halbtöne | 12 | Transposition Stimme A |
| High Pitch A Level L/R | 0–1 | 1,0 |
| High Pitch B Enable L/R | An/Aus | Aus |
| High Pitch B L/R | 0–24 Halbtöne | 7 | Transposition Stimme B (z. B. Quinte für Powerchord) |
| High Pitch B Level L/R | 0–1 | 1,0 |
| High Formant Erhalt L/R | An/Aus | An | Verhindert „Chipmunk"-Effekt bei starker Transposition |

### Chorus / Doubler (auf dem Gitarren-Bus = Mid + High)

| Parameter | Bereich | Default |
|---|---|---|
| Chorus Enable | An/Aus | Aus |
| Chorus Rate | 0,05–5 Hz (log) | 0,8 Hz |
| Chorus Depth | 0–1 | 0,25 |
| Chorus Delay | 1–30 ms | 7 ms |
| Chorus Feedback | 0–0,9 | 0,0 |
| Chorus Mix | 0–1 | 0,35 |

### Cabinet-Simulation (auf dem Gitarren-Bus, nach dem Chorus)

| Parameter | Bereich | Default |
|---|---|---|
| Cab Enable | An/Aus | Aus |
| Cab Model | Bright 4x12 / Dark 2x12 / Mid Focus / Custom | Bright 4x12 |
| Cab Level | 0–2× | 1,0 |
| Load IR… (Button) | Datei-Dialog (.wav/.aiff/.aif) | — | Lädt eine eigene Impulsantwort, schaltet Cab Model automatisch auf „Custom" |

Die drei eingebauten Modelle sind aktuell **synthetische Platzhalter-Impulsantworten**
(gefiltertes Rauschen mit Decay), keine echten Cabinet-Mitschnitte — bis echte IR-Dateien
vorliegen, dienen sie nur dazu, die Kette hörbar/testbar zu machen.

### Ausgang (global)

| Parameter | Bereich | Default |
|---|---|---|
| Mix | 0–1 (Dry/Wet) | 1,0 |
| Output | −24–24 dB | 0 dB |
| Bypass | An/Aus | Aus |

## Build

```bash
cmake -B build
cmake --build build --config Debug     # oder --config Release
```

Artefakte:
- `build/ydna_stereodrive_artefacts/Debug|Release/VST3/stereodrive.vst3`
- `build/ydna_stereodrive_artefacts/Debug|Release/Standalone/stereodrive.exe`

`COPY_PLUGIN_AFTER_BUILD` ist deaktiviert (Admin-Rechte nötig) — die VST3 ggf. manuell nach
`%LOCALAPPDATA%\Programs\Common\VST3\` kopieren.

## Bekannte Einschränkungen / offene Punkte

- Kein automatisierter Test-Suite; Verifikation bisher: Build, Standalone-Start, Code-Review
  der Latenz-Invarianz. Kein Hörtest/GUI-Sichtprüfung in dieser Umgebung möglich.
- Cabinet-IRs sind Platzhalter (siehe oben).
- Alte gespeicherte Presets/States aus der Vor-3-Band-Version sind **nicht kompatibel**
  (Parameter-IDs wurden umbenannt/umstrukturiert).
