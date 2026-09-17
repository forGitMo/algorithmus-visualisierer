#include "SoundEngine.h"
#include <cmath>
#include <vector>
#include <algorithm>

SoundEngine::SoundEngine() {
    SDL_AudioSpec spec{};
    spec.format   = SDL_AUDIO_F32;
    spec.channels = CHANNELS;
    spec.freq     = SAMPLE_RATE;

    // Gerät öffnen und Stream binden (Push-Modus ohne Callback)
    m_stream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);

    if (m_stream) {
        SDL_ResumeAudioStreamDevice(m_stream);
    }
}

SoundEngine::~SoundEngine() {
    if (m_stream) {
        // DestroyAudioStream schließt auch das zugehörige Gerät
        SDL_DestroyAudioStream(m_stream);
    }
}

void SoundEngine::playTone(float frequency, float durationMs, float pan, float volume) {
    if (m_muted || !m_stream) return;

    // Puffer-Überlauf verhindern: zu viel wartende Daten → leeren
    const int maxQueuedBytes =
        (SAMPLE_RATE * CHANNELS * static_cast<int>(sizeof(float)) * MAX_QUEUED_MS) / 1000;
    if (SDL_GetAudioStreamQueued(m_stream) > maxQueuedBytes) {
        SDL_ClearAudioStream(m_stream);
    }

    const int numSamples =
        static_cast<int>(static_cast<float>(SAMPLE_RATE) * durationMs / 1000.0f);
    if (numSamples <= 0) return;

    // Stereo-Buffer (interleaved: L0, R0, L1, R1, …)
    std::vector<float> buf(static_cast<std::size_t>(numSamples * CHANNELS), 0.0f);

    // Equal-Power-Panning: konstante Gesamtlautstärke bei Panorama-Verschiebung
    //   pan –1 → angle=0    → cos(0)=1,   sin(0)=0    → alles links
    //   pan  0 → angle=π/4  → cos=sin=√½              → Mitte
    //   pan +1 → angle=π/2  → cos(π/2)=0, sin(π/2)=1 → alles rechts
    const float angle      = (pan + 1.0f) * 0.5f * PI * 0.5f;
    const float leftGain   = std::cos(angle);
    const float rightGain  = std::sin(angle);

    // Hüllkurve: kurzes Attack + Release verhindert hörbare Knack-Geräusche
    const int attackSamples  = std::min(static_cast<int>(SAMPLE_RATE * 0.005f), numSamples / 4);
    const int releaseSamples = std::min(static_cast<int>(SAMPLE_RATE * 0.010f), numSamples / 3);

    const float phaseStep = 2.0f * PI * frequency / static_cast<float>(SAMPLE_RATE);
    float phase = 0.0f;

    for (int i = 0; i < numSamples; ++i) {
        // Amplituden-Hüllkurve berechnen
        float env = volume;
        if (i < attackSamples) {
            env *= static_cast<float>(i) / static_cast<float>(attackSamples);
        } else if (i >= numSamples - releaseSamples) {
            env *= static_cast<float>(numSamples - i) / static_cast<float>(releaseSamples);
        }

        const float sample = std::sin(phase) * env;
        phase += phaseStep;
        if (phase >= 2.0f * PI) phase -= 2.0f * PI;

        const auto base     = static_cast<std::size_t>(i) * 2;
        buf[base]           = sample * leftGain;
        buf[base + 1]       = sample * rightGain;
    }

    SDL_PutAudioStreamData(m_stream,
                           buf.data(),
                           static_cast<int>(buf.size() * sizeof(float)));
}

void SoundEngine::toggleMute() {
    m_muted = !m_muted;
    if (m_muted && m_stream) {
        // Bereits gepuffertes Audio sofort verwerfen
        SDL_ClearAudioStream(m_stream);
    }
}
