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
class SimpleReverbAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    SimpleReverbAudioProcessorEditor (SimpleReverbAudioProcessor&);
    ~SimpleReverbAudioProcessorEditor() override;

    //==============================================================================
    void timerCallback() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    

private:

    SimpleReverbAudioProcessor& audioProcessor;
    Laf lnf;
    std::array<Laf::LevelMeter, 2> meter;
    std::array<Laf::LevelMeter, 2> outMeter;

    juce::Slider roomSize, damping, dryWet, width;
    juce::ImageButton freeze;

    juce::AudioProcessorValueTreeState::SliderAttachment roomSizeAT, dampingAT, dryWetAT, widthAT;
    juce::AudioProcessorValueTreeState::ButtonAttachment freezeAT;
    juce::TooltipWindow tt {this, 1000};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleReverbAudioProcessorEditor)
};
