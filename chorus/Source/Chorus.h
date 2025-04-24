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
        setDelayTime(0, 0.01f);
        setDelayTime(1, 0.03f);
        setMaxDelayTime(0.05f);
    }
    
    void setDelayTime(int channel, Type value) {
        // Check value is positive
        jassert(value >= Type(0));
        
        delayTimes[channel].setTargetValue(value);
        
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
        
        lfoRateHz.setTargetValue(value);
    }
    
    void setLfoDepth(Type value) {
        // Check value is positive
        jassert(value > Type(0));
        
        lfoDepth.setTargetValue(value);
    }
    
    void setMix(Type value) {
        // Check value is positive
        jassert(value > Type(0));
        
        mix.setTargetValue(value);
    }
    
    void prepareToPlay(double newSampleRate) {
        // Set the sample rate
        sampleRate = static_cast<Type>(newSampleRate);
        
        delayTimes[0].reset(sampleRate, 0.1);
        delayTimes[1].reset(sampleRate, 0.1);
        lfoRateHz.reset(sampleRate, 0.05);
        lfoDepth.reset(sampleRate, 0.05);
        mix.reset(sampleRate, 0.05);
        
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
                auto modulatedDelayTime = delayTimes[channel].getNextValue() + lfoVal * lfoDepth.getNextValue();
                
                // Calculate the modulated delay time in samples
                auto modulatedDelayInSamples = modulatedDelayTime * sampleRate;
                
                // Push the raw sample value to the current channels delay line
                delayLines[channel].push(inputSample);
                
                // read the modulated delay time sample from the delay line
                auto delayedSample = delayLines[channel].read(modulatedDelayInSamples);
                
                // mix the raw sample with the delayed sample at a ratio
                channelData[i] = inputSample * (1.0-mix.getNextValue()) + delayedSample * mix.getNextValue();
                
                // update the phase
                lfoPhase[channel] += lfoRateHz.getNextValue() / sampleRate;
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
        for (size_t channel = 0; channel < maxNumChannels; ++channel)
            delayTimesSample[channel] = (size_t) juce::roundToInt (delayTimes[channel].getNextValue() * sampleRate);
    }
    
    std::array<DelayLine<Type>, maxNumChannels> delayLines;
    std::array<size_t, maxNumChannels> delayTimesSample;
    std::array<juce::SmoothedValue<Type>, maxNumChannels> delayTimes;
    
    Type sampleRate { Type (44.1e3) };
    Type maxDelayTime { Type (0.50) };
    
    std::array<Type, maxNumChannels> lfoPhase {};
    juce::SmoothedValue<Type> lfoRateHz { 0.25 };
    juce::SmoothedValue<Type> lfoDepth { 0.005 };
    juce::SmoothedValue<Type> mix { 0.5 };
    
};
