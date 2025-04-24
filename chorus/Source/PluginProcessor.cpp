/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Chorus.h"

juce::AudioProcessorValueTreeState::ParameterLayout ChorusAudioProcessor::createParameterLayout()
{
    return {
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "rate",  1 }, "Rate", juce::NormalisableRange{0.0f, 1.0f, 0.005f}, 500.f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "depth",  2 }, "Depth", juce::NormalisableRange{0.0f, 0.01f, 0.0005f}, 500.f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "delayLeft",  3 }, "Delay Left", juce::NormalisableRange{0.005f, 0.05f, 0.005f}, 500.f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "delayRight",  4 }, "Delay Right", juce::NormalisableRange{0.005f, 0.05f, 0.005f}, 500.f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "mix",  4 }, "Mix", juce::NormalisableRange{0.0f, 1.0f, 0.1f}, 500.f),
    };
}

//==============================================================================
ChorusAudioProcessor::ChorusAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
    , parameters (*this, nullptr, juce::Identifier ("parameters"), createParameterLayout())
{
    rateParameter = parameters.getRawParameterValue("rate");
    depthParameter = parameters.getRawParameterValue("depth");
    delayLeftParameter = parameters.getRawParameterValue("delayLeft");
    delayRightParameter = parameters.getRawParameterValue("delayRight");
    mixParameter = parameters.getRawParameterValue("mix");
}

ChorusAudioProcessor::~ChorusAudioProcessor()
{
}

//==============================================================================
const juce::String ChorusAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ChorusAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ChorusAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ChorusAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ChorusAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ChorusAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ChorusAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ChorusAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ChorusAudioProcessor::getProgramName (int index)
{
    return {};
}

void ChorusAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ChorusAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    chorus.prepareToPlay(sampleRate);
}

void ChorusAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ChorusAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ChorusAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    auto numInputs = getTotalNumInputChannels();
    auto numOutputs = getTotalNumOutputChannels();

    // Avoid garbage output on unused channels
    for (int ch = numInputs; ch < numOutputs; ++ch)
        buffer.clear(ch, 0, buffer.getNumSamples());
    
    const auto rate = rateParameter->load();
    const auto depth = depthParameter->load();
    const auto delayLeft = delayLeftParameter->load();
    const auto delayRight = delayRightParameter->load();
    const auto mix = mixParameter->load();
    
    chorus.setLfoRate(rate);
    chorus.setLfoDepth(depth);
    chorus.setDelayTime(0, delayLeft);
    chorus.setDelayTime(1, delayRight);
    chorus.setMix(mix);
    
    chorus.processBlock(buffer);
}


//==============================================================================
bool ChorusAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ChorusAudioProcessor::createEditor()
{
    return new ChorusAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void ChorusAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ChorusAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChorusAudioProcessor();
}
