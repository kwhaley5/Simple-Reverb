/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "KiTiKLNF.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SimpleReverbAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SimpleReverbAudioProcessorEditor (SimpleReverbAudioProcessor&);
    ~SimpleReverbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleReverbAudioProcessor& audioProcessor;
    Laf lnf;

    juce::Slider roomSize, damping, dryWet, width, lowPass, highPass;
    juce::ToggleButton freeze;

    juce::AudioProcessorValueTreeState::SliderAttachment roomSizeAT, dampingAT, dryWetAT, widthAT, lowPassAT, highPassAT;
    juce::AudioProcessorValueTreeState::ButtonAttachment freezeAT;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleReverbAudioProcessorEditor)
};
