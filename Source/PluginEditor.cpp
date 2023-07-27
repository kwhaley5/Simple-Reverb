/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleReverbAudioProcessorEditor::SimpleReverbAudioProcessorEditor (SimpleReverbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), roomSizeAT(audioProcessor.apvts, "roomSize", roomSize),
    dampingAT(audioProcessor.apvts, "damping", damping), dryWetAT(audioProcessor.apvts, "dryWet", dryWet),
    widthAT(audioProcessor.apvts, "width", width), freezeAT(audioProcessor.apvts, "freeze", freeze)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setLookAndFeel(&lnf);

    roomSize.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    roomSize.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    roomSize.setName("Size");
    addAndMakeVisible(roomSize);

    damping.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    damping.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    damping.setName("Damping");
    addAndMakeVisible(damping);

    width.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    width.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    width.setName("Width");
    addAndMakeVisible(width);

    dryWet.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dryWet.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    dryWet.setName("Dry/Wet");
    addAndMakeVisible(dryWet);

    freeze.setToggleState(false, juce::dontSendNotification);
    freeze.setButtonText("Freeze");
    addAndMakeVisible(freeze);

    setSize (600, 400);


}

SimpleReverbAudioProcessorEditor::~SimpleReverbAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void SimpleReverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void SimpleReverbAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();
    auto top = bounds.removeFromTop(bounds.getHeight() * .5);
    auto bottom = bounds;

    auto roomBounds = top.removeFromLeft(top.getWidth() * .25);
    roomSize.setBounds(roomBounds);

    auto dampingBounds = top.removeFromLeft(top.getWidth() * .33);
    damping.setBounds(dampingBounds);

    auto widthBounds = top.removeFromLeft(top.getWidth() * .5);
    width.setBounds(widthBounds);

    auto dryWetBounds = top.removeFromLeft(top.getWidth());
    dryWet.setBounds(dryWetBounds);

    freeze.setBounds(bottom);

}
