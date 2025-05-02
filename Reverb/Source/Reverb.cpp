#include "Reverb.h"
#include <algorithm>

Reverb::Reverb() {
  combFilters.resize(4);  // Initialize 4 comb filters
  allPassFilters.resize(2);
}

Reverb::~Reverb() {}

void Reverb::setSampleRate(float value) {
  sampleRate = value;

  // Set the sample rate for each filter
  for (auto& combFilter : combFilters) {
    combFilter.setSampleRate(sampleRate);
  }
  
  for (auto& allPassFilter : allPassFilters) {
    allPassFilter.setSampleRate(sampleRate);
  }
}

void Reverb::setMix(float value) {
  // Ensure mix is between 0.0 and 1.0
  mix.setTargetValue(std::clamp(value, 0.0f, 1.0f));
}

void Reverb::setDecay(float value) {
  // NOTE: We treat depth as the base value
  // and set each comb filter's feedback from offsets
  decay.setTargetValue(value);
  
  // make sure the base feedback is within range
  float decayValue = decay.getNextValue();
  for (auto& combFilter : combFilters) {
    combFilter.setFeedback(decayValue);
  }
}

void Reverb::prepare(float samplingRate) {
  sampleRate = samplingRate;
  
  // Smoothed value setup
  mix.reset(sampleRate, 0.05);
  setMix(0.8f);
  
  // Same for decay
  decay.reset(sampleRate, 0.001);
  setDecay(2.5f);

  // Set the delay time and feedback for each comb filter
  float combFilterDelayTimes[4] = { 30.1f, 34.2f, 39.1f, 45.1f };
  
  for (int i = 0; i < combFilters.size(); i++) {
    combFilters[i].setDelayTime(combFilterDelayTimes[i]);
    combFilters[i].setFeedback(decay.getNextValue());
    combFilters[i].prepare(sampleRate, i % 2 == 0);
  }
  
  allPassFilters[0].setDelayTime(1.2f);
  allPassFilters[0].setFeedback(0.5f);
  
  allPassFilters[1].setDelayTime(3.6f);
  allPassFilters[1].setFeedback(0.5f);
  
  for (auto& allPassFilter : allPassFilters) {
    allPassFilter.prepare(sampleRate);
  }
}

void Reverb::process(juce::AudioBuffer<float>& buffer) {
  int numSamples = buffer.getNumSamples();
  int numChannels = buffer.getNumChannels();

  // Process the input sample through each comb filter
  // NOTE:: Since the comb filters are in parallel, we have to
  // process each comb filter separately on the input sample
  // and then mix the output samples together
  juce::AudioBuffer<float> wetBuffer(buffer.getNumChannels(),
                                     buffer.getNumSamples());
  wetBuffer.clear();

  for (auto& combFilter : combFilters) {
    // Create a temporary buffer to hold the output of the comb filter
    // ensuring each comb filter is processed in parallel
    juce::AudioBuffer<float> tempBuffer;
    tempBuffer.makeCopyOf(buffer);

    // Process the comb filter
    combFilter.process(tempBuffer);

    // Mix the output of the comb filter with the wet buffer
    for (int channel = 0; channel < numChannels; ++channel) {
      wetBuffer.addFrom(channel, 0, tempBuffer, channel, 0,
                        buffer.getNumSamples());
    }
  }
  
  // Average out the gain levels
  wetBuffer.applyGain(1.0f / static_cast<float>(combFilters.size()));
  
  // Apply allpass filter
  allPassFilters[0].process(wetBuffer);
  
  // Apply allpass filter
  allPassFilters[1].process(wetBuffer);
  
  // Mix the wet buffer with the original input buffer
  for (int channel = 0; channel < numChannels; ++channel) {
    auto* channelData = buffer.getWritePointer(channel);
    auto* wetChannelData = wetBuffer.getReadPointer(channel);

    for (int i = 0; i < numSamples; ++i) {
      auto drySample = channelData[i];
      auto wetSample = wetChannelData[i];

      // Mix the wet sample with the input sample
      float mixVal = mix.getNextValue();
      channelData[i] = (1.0f - mixVal) * drySample + mixVal * wetSample;
    }
  }
  
}
