/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

/*
    TODO:
    Create Graphical Filter
    Add Logo and Such
    Update prefixes on slider.
    Have it only filter the reverb -> Needs a highpass
*/

//==============================================================================
SimpleReverbAudioProcessorEditor::SimpleReverbAudioProcessorEditor (SimpleReverbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), roomSizeAT(audioProcessor.apvts, "roomSize", roomSize),
    dampingAT(audioProcessor.apvts, "damping", damping), dryWetAT(audioProcessor.apvts, "dryWet", dryWet),
    widthAT(audioProcessor.apvts, "width", width), freezeAT(audioProcessor.apvts, "freeze", freeze),
    lowPassAT(audioProcessor.apvts, "lowPass", lowPass), highPassAT(audioProcessor.apvts, "highPass", highPass)
    //filtersAT(audioProcessor.apvts, "filters", filters)
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

    highPass.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    highPass.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    highPass.setName("Dry/Wet");
    addAndMakeVisible(highPass);

    lowPass.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lowPass.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    lowPass.setName("Dry/Wet");
    addAndMakeVisible(lowPass);
    
    filters.addItem("x2", 1);
    filters.addItem("x4", 2);
    filters.addItem("x8", 3);
    filters.addItem("x16", 4);
    addAndMakeVisible(filters);
    filtersAT = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "filters", filters);

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

    auto highPassBounds = bottom.removeFromLeft(bottom.getWidth() * .25);
    highPass.setBounds(highPassBounds);

    auto lowPassBounds = bottom.removeFromRight(bottom.getWidth() * .33);
    lowPass.setBounds(lowPassBounds);

    auto filtersBounds = bottom.removeFromBottom(bottom.getHeight() * .5);
    filters.setBounds(filtersBounds);

    freeze.setBounds(bottom);

}
