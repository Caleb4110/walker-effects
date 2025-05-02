/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReverbAudioProcessorEditor::ReverbAudioProcessorEditor (ReverbAudioProcessor& p, juce::AudioProcessorValueTreeState& valueTree)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);
    addAndMakeVisible(decaySlider);
    decaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    decayAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTree, "decay", decaySlider));
    
    addAndMakeVisible(decayLabel);
    decayLabel.setText("DECAY", juce::dontSendNotification);
    decayLabel.attachToComponent(&decaySlider, false);
}

ReverbAudioProcessorEditor::~ReverbAudioProcessorEditor()
{
}

//==============================================================================
void ReverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
}

void ReverbAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    auto sliderWidth = 60;
    auto spacing = 15;
    
    decaySlider.setBounds(area.removeFromLeft(sliderWidth));
    area.removeFromLeft(spacing);
}
