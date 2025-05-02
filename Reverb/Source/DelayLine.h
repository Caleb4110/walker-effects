#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>

class DelayLine {
public:
  void push(float sample);
  void resize(float delayTimeInSamples);
  void clear() noexcept;
  float get(float delayTimeInSamples);

private:
  std::vector<float> buffer;
  size_t writeIndex = 0;
};
