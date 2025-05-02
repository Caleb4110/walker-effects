#include "CombFilter.h"

void CombFilter::setDelayTime(float value) {
  delayTime = value;

  // Also update the delay time in samples
  delayTimeInSamples = (delayTime / 1000.0f) * sampleRate;
}

/**
 * Sets the feedback of a comb filter based on a given decay
 *  @param decay is the desired decay of the filter in seconds
 */
void CombFilter::setFeedback(float decay) {

  // Don't need to multiply by sample rate as we
  // already have delay time in samples
  float numerator = -3.0f * delayTimeInSamples;
  
  // Need to multiply by sample rate as we need decay time in samples
  float denominator = decay * sampleRate;
  float newFeedback = std::powf(10.0f, numerator / denominator);

  feedback = juce::jlimit(-0.95f, 0.95f, newFeedback);
}

void CombFilter::setSampleRate(float value) {
  sampleRate = value;

  // Update the delay time in samples
  delayTimeInSamples = (delayTime / 1000.0f) * sampleRate;
}

void CombFilter::prepare(float samplingRate, bool flipPhase) {
  setSampleRate(samplingRate);

  // Assuming stereo output, initialise the vector with 2 delay lines
  delayBuffers.resize(2);
  
  if (flipPhase) {
    phaseFlipped = -1;
  } else {
    phaseFlipped = 1;
  }

  // Resize each delay line for the correct delay time
  // and clear the the contents
  jassert(delayTimeInSamples > 0.0f);
  
  for (auto& delayBuffer : delayBuffers) {
    delayBuffer.resize(delayTimeInSamples);
    delayBuffer.clear();
  }
}

void CombFilter::process(juce::AudioBuffer<float>& buffer) {
  int numSamples = buffer.getNumSamples();
  int numChannels = buffer.getNumChannels();

  // For each channel....
  for (auto channel = 0; channel < numChannels; ++channel) {
    auto* channelData = buffer.getWritePointer(channel);

    // For each sample in the channel...
    for (int i = 0; i < numSamples; ++i) {
      auto inputSample = channelData[i];
    
      // Apply the comb filter effect
      float filteredSample =
          inputSample +
          ((phaseFlipped * feedback) *
           delayBuffers[static_cast<size_t>(channel)].get(delayTimeInSamples));
      
      // Push the combined filtered sample back onto the delay line 
      delayBuffers[static_cast<size_t>(channel)].push(filteredSample);

      // Mix the filtered sample with the original input sample
      //channelData[i] = (1.0f - mix) * inputSample + (mix * filteredSample);
      channelData[i] = filteredSample;
    }
  }
}
