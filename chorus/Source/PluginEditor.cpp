/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChorusAudioProcessorEditor::ChorusAudioProcessorEditor (ChorusAudioProcessor& p, juce::AudioProcessorValueTreeState& valueTree)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);
    
    // Rate
    addAndMakeVisible(rateSlider);
    rateSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    rateAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTree, "rate", rateSlider));
    
    addAndMakeVisible(rateLabel);
    rateLabel.setText("RATE", juce::dontSendNotification);
    rateLabel.attachToComponent(&rateSlider, false);
    
    // Depth
    addAndMakeVisible(depthSlider);
    depthSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    depthAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTree, "depth", depthSlider));
    
    addAndMakeVisible(depthLabel);
    depthLabel.setText("DEPTH", juce::dontSendNotification);
    depthLabel.attachToComponent(&depthSlider, false);
    
    // Delay Left
    addAndMakeVisible(delayLeftSlider);
    delayLeftSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    delayLeftAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTree, "delayLeft", delayLeftSlider));

    addAndMakeVisible(delayLeftLabel);
    delayLeftLabel.setText("DELAY L", juce::dontSendNotification);
    delayLeftLabel.attachToComponent(&delayLeftSlider, false);

    // Delay Right
    addAndMakeVisible(delayRightSlider);
    delayRightSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    delayRightAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTree, "delayRight", delayRightSlider));

    addAndMakeVisible(delayRightLabel);
    delayRightLabel.setText("DELAY R", juce::dontSendNotification);
    delayRightLabel.attachToComponent(&delayRightSlider, false);
    
    // Mix
    addAndMakeVisible(mixSlider);
    mixSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    mixAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTree, "mix", mixSlider));

    addAndMakeVisible(mixLabel);
    mixLabel.setText("MIX", juce::dontSendNotification);
    mixLabel.attachToComponent(&mixSlider, false);
    

}

ChorusAudioProcessorEditor::~ChorusAudioProcessorEditor()
{
}

//==============================================================================
void ChorusAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
}

void ChorusAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto area = getLocalBounds().reduced(20);
    auto sliderWidth = 60;
    auto spacing = 15;

    rateSlider.setBounds(area.removeFromLeft(sliderWidth));
    area.removeFromLeft(spacing);
    depthSlider.setBounds(area.removeFromLeft(sliderWidth));
    area.removeFromLeft(spacing);
    delayLeftSlider.setBounds(area.removeFromLeft(sliderWidth));
    area.removeFromLeft(spacing);
    delayRightSlider.setBounds(area.removeFromLeft(sliderWidth));
    area.removeFromLeft(spacing);
    mixSlider.setBounds(area.removeFromLeft(sliderWidth));

}
