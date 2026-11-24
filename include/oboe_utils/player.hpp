#pragma once

#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <span>

#include <oboe/Oboe.h>
#include <oboe_utils/shared_audio_buffer.hpp>

namespace obut {
// ---------------------------------------------------------------------------
// Player
//   Oboe output callback.  Reads raw audio from SharedAudioBuffer, and
//   writes samples back to Oboe.
// ---------------------------------------------------------------------------
template <typename AudioSampleType>
class Player : public oboe::AudioStreamDataCallback {
 public:
  Player(int32_t channels, audio_buffer& buffer, const bool dbg = false)
      : m_channels{channels}, m_buffer{buffer} {}

  oboe::DataCallbackResult onAudioReady(oboe::AudioStream* /*stream*/,
                                        void* audio_data,
                                        int32_t num_frames) override {
    using namespace std::chrono;
    audio_sample_t* out = static_cast<audio_sample_t*>(audio_data);
    const int32_t num_samples = num_frames * m_channels;

    // Read raw input from the shared ring-buffer
    m_buffer.read(out, num_samples);
    processOnRead({out, num_samples});
    return oboe::DataCallbackResult::Continue;
  }

 protected:
 virtual size_t processOnRead(std::span<float> samples) {
    return samples.size();
 }

 private:
  int32_t m_channels;
  SharedAudioBuffer<AudioSampleType>& m_buffer;
};
}