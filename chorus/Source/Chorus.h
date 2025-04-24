/*
  ==============================================================================

    Chorus.h
    Created: 24 Apr 2025 9:40:10am
    Author:  Caleb

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DelayLine.h"

template <typename Type, size_t maxNumChannels = 2>
class Chorus {
public:
    Chorus() {
        setDelayTime(0, 0.15f);
        setDelayTime(1, 0.40f);
        setMaxDelayTime(0.05f);
    }
    
    void setDelayTime(int channel, Type value) {
        // Check value is positive
        jassert(value >= Type(0));
        
        // NOTE: For now set each channel the same
        delayTimes[channel] = value;
        
        // Update the delay time
        updateDelayTime();
    }
    
    void setMaxDelayTime(Type value) {
        // Check value is positive
        jassert(value > Type(0));
        
        // Set the max delay time
        maxDelayTime = value;
        
        // Update the size of the delay line
        updateDelayLineSize();
    }
    
    void setLfoRate(Type value) {
        // Check value is positive
        jassert(value > Type(0));
        
        lfoRateHz = value;
    }
    
    void setLfoDepth(Type value) {
        // Check value is positive
        jassert(value > Type(0));
        
        lfoDepth = value;
    }
    
    void setMix(Type value) {
        // Check value is positive
        jassert(value > Type(0));
        
        mix = value;
    }
    
    void prepareToPlay(double newSampleRate) {
        // Set the sample rate
        sampleRate = static_cast<Type>(newSampleRate);
        
        // Update the delayline size and time
        updateDelayLineSize();
        updateDelayTime();
        
        // Set all phases to 0
        for (auto& phase : lfoPhase)
            phase = Type(0);
    }
    
    void processBlock(juce::AudioBuffer<float>& buffer) {
        // Get number of channels and samples
        const auto numChannels = std::min((int)maxNumChannels, buffer.getNumChannels());
        const auto numSamples = buffer.getNumSamples();
        
        // Iterate through each channel
        for (auto channel = 0; channel < numChannels; ++channel) {
            auto* channelData = buffer.getWritePointer(channel);
            
            // Iterate over each sample within each channel
            for (int i = 0; i < numSamples; ++i) {
                auto inputSample = channelData[i];
                
                // Calculate the lfo Value
                auto lfoVal = std::sin(lfoPhase[channel] * juce::MathConstants<Type>::twoPi);
                
                // Modulate the delay time based on the lfos value and depth
                auto modulatedDelayTime = delayTimes[channel] + lfoVal * lfoDepth;
                
                // Calculate the modulated delay time in samples
                auto modulatedDelayInSamples = modulatedDelayTime * sampleRate;
                
                // Push the raw sample value to the current channels delay line
                delayLines[channel].push(inputSample);
                
                // read the modulated delay time sample from the delay line
                auto delayedSample = delayLines[channel].read(modulatedDelayInSamples);
                
                // mix the raw sample with the delayed sample at a ratio
                channelData[i] = inputSample * (1.0-mix) + delayedSample * mix;
                
                // update the phase
                lfoPhase[channel] += lfoRateHz / sampleRate;
                            if (lfoPhase[channel] >= 1.0)
                                lfoPhase[channel] -= 1.0;
            }
        }
    }
private:
    void updateDelayLineSize() {
        auto delayLineSizeSamples = (size_t) std::ceil(maxDelayTime * sampleRate);
        for (auto& delayLine : delayLines)
            delayLine.resize(delayLineSizeSamples);
    }
    
    void updateDelayTime() noexcept {
        for (size_t ch = 0; ch < maxNumChannels; ++ch)
            delayTimesSample[ch] = (size_t) juce::roundToInt (delayTimes[ch] * sampleRate);
    }
    
    std::array<DelayLine<Type>, maxNumChannels> delayLines;
    std::array<size_t, maxNumChannels> delayTimesSample;
    std::array<Type, maxNumChannels> delayTimes;
    
    Type sampleRate { Type (44.1e3) };
    Type maxDelayTime { Type (0.50) };
    
    std::array<Type, maxNumChannels> lfoPhase {};
    Type lfoRateHz { 0.25 };
    Type lfoDepth { 0.005 };
    Type mix { 0.5 };
    
};
