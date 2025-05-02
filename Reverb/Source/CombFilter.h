#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include "DelayLine.h"

class CombFilter {
public:
  void setDelayTime(float value);
  void setFeedback(float decay);
  void setSampleRate(float value);
  
  void prepare(float samplingRate, bool flipPhase);
  void process(juce::AudioBuffer<float>& buffer);

private:
  float delayTime;           // in ms
  float delayTimeInSamples;  // in samples
  float feedback;  // feedback amount (-0.9 to 0.9)

  float sampleRate;  // sample rate in Hz
  
  int phaseFlipped;
  std::vector<DelayLine> delayBuffers;  // delay buffers for each channel;
};
