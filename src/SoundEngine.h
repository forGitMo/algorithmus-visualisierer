#pragma once

#include <SDL3/SDL.h>

// SoundEngine: Erzeugt synthetische Sinustöne zur audiovisuellen Begleitung
// der Sortieralgorithmen. Verwendet SDL3 Audio im Push-Modus (kein Callback).
// Linker Algorithmus wird auf den linken, rechter auf den rechten Kanal gepannt.
class SoundEngine {
public:
    SoundEngine();
    ~SoundEngine();

    // Nicht kopierbar
    SoundEngine(const SoundEngine&)            = delete;
    SoundEngine& operator=(const SoundEngine&) = delete;

    // Spielt einen kurzen Sinuston.
    //   frequency : Tonhöhe in Hz  (z.B. 150–1000)
    //   durationMs: Dauer in Millisekunden
    //   pan       : Stereo-Position  –1.0 = links | 0.0 = Mitte | +1.0 = rechts
    //   volume    : Lautstärke 0.0–1.0
    void playTone(float frequency,
                  float durationMs,
                  float pan    = 0.0f,
                  float volume = 0.45f);

    // Stummschaltung umschalten
    void toggleMute();

    bool isMuted()     const { return m_muted;           }
    bool isAvailable() const { return m_stream != nullptr; }

private:
    SDL_AudioStream* m_stream = nullptr;
    bool             m_muted  = false;

    static constexpr int   SAMPLE_RATE    = 48000;
    static constexpr int   CHANNELS       = 2;     // Stereo
    static constexpr float PI             = 3.14159265358979323846f;
    // Maximale gepufferte Audio-Zeit (ms) bevor der Puffer geleert wird,
    // damit Audio und Visualisierung nicht auseinanderlaufen.
    static constexpr int   MAX_QUEUED_MS  = 60;
};
