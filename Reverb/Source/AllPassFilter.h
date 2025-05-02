#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include "DelayLine.h"

class AllPassFilter {
public:
  void setDelayTime(float value);
  void setFeedback(float value);
  void setSampleRate(float value);
  
  void prepare(float samplingRate);
  void process(juce::AudioBuffer<float>& buffer);
private:
  float delayTime;           // in milliseconds
  float delayTimeInSamples;  // in samples
  
  float feedback;  // feedback amount (0.3 to 0.9)
  
  float sampleRate;  // sample rate in Hz
  
  std::vector<DelayLine> dryDelayBuffers;  // dry delay buffers for each channel;
  std::vector<DelayLine> wetDelayBuffers;  // wet delay buffers for each channel;
};
