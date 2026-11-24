#pragma once

#pragma once

#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <thread>
#include <vector>

#include <oboe/Oboe.h>
#include <oboe_utils/shared_audio_buffer.hpp>

namespace obut {
/////
// ---------------------------------------------------------------------------
// Callback: pulls float samples from Oboe, converts to int16, stores them
// ---------------------------------------------------------------------------
template <typename AudioSampleType>
class Recorder : public oboe::AudioStreamDataCallback {
 public:
  Recorder(int32_t channels, SharedAudioBuffer<AudioSampleType>& buffer)
      : m_channels{channels}, m_buffer{buffer} {}

  oboe::DataCallbackResult onAudioReady(oboe::AudioStream* /*stream*/,
                                        void* audio_data,
                                        int32_t num_frames) override {
    const AudioSampleType* in = static_cast<const AudioSampleType*>(audio_data);
    int32_t num_samples = num_frames * m_channels;

    m_buffer.write(in, num_samples);
    return oboe::DataCallbackResult::Continue;
  }

 private:
  int32_t m_channels;
  SharedAudioBuffer<AudioSampleType>& m_buffer;
};
}