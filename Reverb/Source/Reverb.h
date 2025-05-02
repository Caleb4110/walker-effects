#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include "CombFilter.h"
#include "AllPassFilter.h"

/**
 * A reverb effect class based on Schroeder's Reverb.
 *
 * This class implements reverb effect using:
 * - 4 parallel comb filters
 * - 2 all-pass filters
 */
class Reverb {
public:
  Reverb();
  ~Reverb();
  void setSampleRate(float value);
  void setMix(float value);
  void setDecay(float value);

  void process(juce::AudioBuffer<float>& buffer);
  void prepare(float samplingRate);

private:
  float sampleRate;  // Sample rate in Hz
  juce::SmoothedValue<float> mix;         // Mix amount (0.0 to 1.0)
  juce::SmoothedValue<float> decay;     // reverb decay (0.0 to 5.0)
  
  std::vector<CombFilter> combFilters;  // Array of comb filters
  std::vector<AllPassFilter> allPassFilters; // Array of all-pass filters
};
