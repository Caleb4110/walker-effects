/*
  ==============================================================================

    DelayLine.h
    Created: 24 Apr 2025 8:57:07am
    Author:  Caleb

  ==============================================================================
*/

#pragma once
#include <iostream>
using namespace std;

template <typename Type>
class DelayLine {
public:
    void push(Type value) noexcept {
        // Add the new value at the oldest position
        rawData_[leastRecentIndex_] = value;
        
        // Update the least recent index in a circular motion
        leastRecentIndex_ = leastRecentIndex_ == 0 ? size() - 1 : leastRecentIndex_ - 1;
    }
    
    Type get(size_t delayInSamples) const noexcept {
        // Ensure we do not exceed the size of the buffer
        jassert(delayInSamples >= 0 && delayInSamples < size());
        
        return rawData_[(leastRecentIndex_ + 1 + delayInSamples) % size()];
    }
    
    // Linearly interpolation for reading values between samples
    Type read(Type delayInSamples) const noexcept {
        auto i0 = static_cast<size_t>(std::floor(delayInSamples));
        auto i1 = (i0 + 1) % size();
        auto frac = delayInSamples - static_cast<Type>(i0);

        auto sample0 = rawData_[(leastRecentIndex_ + 1 + i0) % size()];
        auto sample1 = rawData_[(leastRecentIndex_ + 1 + i1) % size()];

        return sample0 + frac * (sample1 - sample0); // Linear interpolation
    }
    
    void set(size_t delayInSamples, Type value) noexcept {
        // Ensure we do not exceed the size of the buffer
        jassert(delayInSamples >= 0 && delayInSamples < size());
        rawData_[(leastRecentIndex_ + 1 + delayInSamples) % size()] = value;
    }
    
    void resize(size_t newSize) {
        rawData_.resize(newSize, Type(0));
        leastRecentIndex_ = 0;
    }
    
    void clear() noexcept {
        std::fill(rawData_.begin(), rawData_.end(), Type(0));
        leastRecentIndex_ = 0;
    }
    
private:
    size_t size() const noexcept {
        return rawData_.size();
    }
    
    vector<Type> rawData_;
    size_t leastRecentIndex_ = 0;
};
