#include "AllPassFilter.h"

void AllPassFilter::setDelayTime(float value) {
  delayTime = value;
  
  // Also update the delay time in samples
  delayTimeInSamples = (delayTime / 1000.0f) * sampleRate;
}

// When changing feedback, we also need to change
void AllPassFilter::setFeedback(float value) {
  // Ensure feedback is between 0.0 and 1.0
  feedback = std::clamp(value, 0.3f, 0.9f);
}

void AllPassFilter::setSampleRate(float value) {
  sampleRate = value;
  
  // Update the delay time in samples
  delayTimeInSamples = (delayTime / 1000.0f) * sampleRate;
}

void AllPassFilter::prepare(float samplingRate) {
  setSampleRate(samplingRate);
  
  // Assuming stereo output, initialise the vector with 2 delay lines
  dryDelayBuffers.resize(2);
  wetDelayBuffers.resize(2);
  
  // Resize each delay line for the correct delay time
  // and clear the the contents
  jassert(delayTimeInSamples > 0.0f);
  
  for (int i = 0; i < dryDelayBuffers.size(); i++) {
    dryDelayBuffers[i].resize(delayTimeInSamples);
    dryDelayBuffers[i].clear();
    wetDelayBuffers[i].resize(delayTimeInSamples);
    wetDelayBuffers[i].clear();
  }
}

void AllPassFilter::process(juce::AudioBuffer<float>& buffer) {
  int numSamples = buffer.getNumSamples();
  int numChannels = buffer.getNumChannels();
  
  // For each channel....
  for (auto channel = 0; channel < numChannels; ++channel) {
    auto* channelData = buffer.getWritePointer(channel);
    
    // For each sample in the channel...
    for (int i = 0; i < numSamples; ++i) {
      auto inputSample = channelData[i];\
      // Add the dry sample to the dry delay buffer
      dryDelayBuffers[channel].push(inputSample);
      
      // Apply the all pass filter
      float filteredSample =
        -1 * feedback * inputSample +
        dryDelayBuffers[channel].get(delayTimeInSamples) +
      feedback * wetDelayBuffers[channel].get(delayTimeInSamples);
      
      // Add the filtered sample to the wet delay buffer
      wetDelayBuffers[static_cast<size_t>(channel)].push(filteredSample);
      
      // Write the filtered sample to the output 
      channelData[i] = filteredSample;
    }
  }
}


