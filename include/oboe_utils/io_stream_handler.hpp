#pragma once

#include <cstdint>

#include <oboe/Oboe.h>
#include <oboe_utils/player.hpp>
#include <oboe_utils/recorder.hpp>
#include <oboe_utils/shared_audio_buffer.hpp>
#include <oboe_utils/stream_manipulation.hpp>

namespace obut {

template <oboe::AudioFormat E>
struct AudioSampleTypeFor;  // primary left undefined so unmapped values fail to
                            // compile

template <>
struct AudioSampleTypeFor<oboe::AudioFormat::Float> {
  using type = float;
};
template <>
struct AudioSampleTypeFor<oboe::AudioFormat::I16> {
  using type = int16_t;
};
template <>
struct AudioSampleTypeFor<oboe::AudioFormat::I32> {
  using type = int32_t;
};

template <oboe::AudioFormat E>
using AudioSampleType = typename AudioSampleTypeFor<E>::type;

template <template <typename> class Impl, template <typename> class Base,
          oboe::AudioFormat AF>
concept DerivedFromForFormat =
    std::derived_from<Impl<AudioSampleType<AF>>, Base<AudioSampleType<AF>>>;

template <oboe::AudioFormat AF, template <typename> class R = Recorder,
          template <typename> class P = Player>
  requires DerivedFromForFormat<R, Recorder, AF> and
           DerivedFromForFormat<P, Player, AF>
class IOStreamHandler {
 public:
  using Sample_t = AudioSampleType<AF>;
  using Player_t = P<Sample_t>;
  using Recorder_t = R<Sample_t>;

  IOStreamHandler(const int32_t dsp_sample_rate, const int32_t dsp_buffer_size,
                  const size_t ring_buffer_scale = 2)
      : m_shared_buffer{(size_t)dsp_buffer_size * ring_buffer_scale},
        m_recorder{1, m_shared_buffer},
        m_player{1, m_shared_buffer} {
    if (m_in_sr = acquire_audio_stream(
            m_in_builder, oboe::Direction::Input,
            oboe::PerformanceMode::LowLatency, oboe::SharingMode::Exclusive,
            oboe::AudioFormat::Float, oboe::ChannelCount::Mono);
        m_in_sr < 0) {
      fprintf(stderr, "Could not acquire input stream\n");
    }

    if (m_out_sr = acquire_audio_stream(
            m_out_builder, oboe::Direction::Output,
            oboe::PerformanceMode::LowLatency, oboe::SharingMode::Exclusive,
            oboe::AudioFormat::Float, oboe::ChannelCount::Mono);
        m_out_sr < 0) {
      fprintf(stderr, "Could not acquire output stream\n");
    }

    m_in_sr = dsp_sample_rate;
    m_out_sr = dsp_sample_rate;

    m_in_builder.setDataCallback(&m_recorder)
        ->setSampleRate(dsp_sample_rate)
        ->setFramesPerCallback(dsp_buffer_size);

    m_out_builder.setDataCallback(&m_player)
        ->setSampleRate(dsp_sample_rate)
        ->setFramesPerCallback(dsp_buffer_size);

    create_streams(dsp_buffer_size, dsp_buffer_size);
  }

  ~IOStreamHandler() {
    m_outstream->close();
    m_instream->close();
  }

  const int32_t& get_in_sr() const { return m_in_sr; }
  const int32_t& get_out_sr() const { return m_out_sr; }

  bool start_streams() {
    auto result = m_instream->requestStart();
    if (result != oboe::Result::OK) {
      fprintf(stderr, "Failed to start stream: %s\n",
              oboe::convertToText(result));
      return false;
    }
    result = m_outstream->requestStart();
    if (result != oboe::Result::OK) {
      fprintf(stderr, "Failed to start stream: %s\n",
              oboe::convertToText(result));
      return false;
    }

    return true;
  }

  void stop_streams() {
    m_outstream->requestStop();
    m_instream->requestStop();
  }

 private:
  bool create_streams(const int32_t input_buffer_size,
                      const int32_t output_buffer_size) {
    auto result = m_in_builder.openManagedStream(m_instream);
    if (result != oboe::Result::OK) {
      fprintf(stderr, "Failed to open input audio stream: %s\n",
              oboe::convertToText(result));
      return false;
    }

    result = m_out_builder.openManagedStream(m_outstream);
    if (result != oboe::Result::OK) {
      fprintf(stderr, "Failed to open output audio stream: %s\n",
              oboe::convertToText(result));
      return false;
    }

    auto in_buf_result = m_instream->setBufferSizeInFrames(input_buffer_size);
    auto out_buf_result =
        m_outstream->setBufferSizeInFrames(output_buffer_size);

    if (in_buf_result) {
      printf("Input buffer size: %d frames\n", in_buf_result.value());
    } else {
      fprintf(stderr, "Failed to set input buffer size: %s\n",
              oboe::convertToText(in_buf_result.error()));
      return false;
    }
    if (out_buf_result) {
      printf("Output buffer size: %d frames\n", out_buf_result.value());
    } else {
      fprintf(stderr, "Failed to set output buffer size: %s\n",
              oboe::convertToText(out_buf_result.error()));
      return false;
    }

    return true;
  }

  bool create_streams(const int32_t audio_buffer_size) {
    return create_streams(audio_buffer_size, audio_buffer_size);
  }

  oboe::ManagedStream m_instream, m_outstream;
  oboe::AudioStreamBuilder m_in_builder, m_out_builder;
  SharedAudioBuffer<Sample_t> m_shared_buffer;
  Recorder_t m_recorder;
  Player_t m_player;

  int32_t m_in_sr, m_out_sr;
};
}  // namespace obut
