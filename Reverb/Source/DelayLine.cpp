#include "DelayLine.h"
#include <juce_core/system/juce_PlatformDefs.h>
#include <cmath>

void DelayLine::push(float sample) {
  buffer[writeIndex] = sample;

  // Update the write index
  // NOTE:: We wrap around by subtracting as it makes it easier to
  // read/calculate linear interpolation later on
  writeIndex = writeIndex == 0 ? buffer.size() - 1 : writeIndex - 1;
}

void DelayLine::resize(float delayTimeInSamples) {
  auto newSize = static_cast<size_t>(std::ceil(delayTimeInSamples));
  buffer.resize(newSize, 0.0f);
  writeIndex = 0;
}

void DelayLine::clear() noexcept {
  std::fill(buffer.begin(), buffer.end(), 0.0f);
  writeIndex = 0;
}

float DelayLine::get(float delayTimeInSamples) {
  jassert(delayTimeInSamples <= buffer.size() && delayTimeInSamples >= 0);

  // Leftmost sample index
  size_t leftSampleIndex = static_cast<size_t>(std::floor(delayTimeInSamples));

  // Rightmost sample index
  size_t rightSampleIndex = (leftSampleIndex + 1) % buffer.size();

  // This is the fractional part of the delay time and represents how far
  // between the two samples the desired delay time is
  float fraction = delayTimeInSamples - static_cast<float>(leftSampleIndex);
  
  // Actual samples to interpolate. Since we write backwards, we access samples
  // forward from the write index
  auto leftSample = buffer[(writeIndex + 1 + leftSampleIndex) % buffer.size()];
  auto rightSample =
      buffer[(writeIndex + 1 + rightSampleIndex) % buffer.size()];

  // Linear interpolation
  return leftSample + fraction * (rightSample - leftSample);
}
