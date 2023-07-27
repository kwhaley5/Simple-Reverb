/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleReverbAudioProcessor::SimpleReverbAudioProcessor()
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
{
    roomSize = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("roomSize"));
    damping = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("damping"));
    dryWet = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("dryWet"));
    width = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("width"));
    freeze = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("freeze"));
}

SimpleReverbAudioProcessor::~SimpleReverbAudioProcessor()
{
}

//==============================================================================
const juce::String SimpleReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleReverbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleReverbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleReverbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleReverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleReverbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleReverbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SimpleReverbAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleReverbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SimpleReverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    auto coef = juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(sampleRate, 1000);
    leftChain.get<filterIndex>().coefficients = coef;
    rightChain.get<filterIndex>().coefficients = coef;
}

void SimpleReverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleReverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void SimpleReverbAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    params.damping = damping->get();
    params.dryLevel = (1 - dryWet->get());
    params.wetLevel = dryWet->get();
    params.freezeMode = freeze->get();
    params.roomSize = roomSize->get();
    params.width = width->get();

    auto& verbLeft = leftChain.get<reverbIndex>();
    verbLeft.setParameters(params);
    auto& verbRight = rightChain.get<reverbIndex>();
    verbRight.setParameters(params);

    juce::dsp::AudioBlock<float> block(buffer);
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);

    
}

//==============================================================================
bool SimpleReverbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleReverbAudioProcessor::createEditor()
{
    return new SimpleReverbAudioProcessorEditor (*this);
}

//==============================================================================
void SimpleReverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SimpleReverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout SimpleReverbAudioProcessor::createParameterLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout layout;

    auto range = NormalisableRange<float>(0, 1, .01, 1);

    layout.add(std::make_unique<AudioParameterFloat>("roomSize", "Room Size", range, .5));
    layout.add(std::make_unique<AudioParameterFloat>("damping", "Damping", range, .5));
    layout.add(std::make_unique<AudioParameterFloat>("dryWet", "Dry/Wet", range, .5));
    layout.add(std::make_unique<AudioParameterFloat>("width", "Width", range, .5));
    layout.add(std::make_unique<AudioParameterBool>("freeze", "Freeze", false));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleReverbAudioProcessor();
}
