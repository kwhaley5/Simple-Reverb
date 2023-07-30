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
    setLookAndFeel(&lnf);

    addAndMakeVisible(meter[0]);
    addAndMakeVisible(meter[1]);
    addAndMakeVisible(outMeter[0]);
    addAndMakeVisible(outMeter[1]);

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

    freeze.setClickingTogglesState(true);
    freeze.setTooltip("Freeze");

    setSize (800, 250);
    
    startTimerHz(24);
}

SimpleReverbAudioProcessorEditor::~SimpleReverbAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void SimpleReverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    auto grad = juce::ColourGradient::ColourGradient(juce::Colour(186u, 34u, 34u), bounds.toFloat().getTopLeft(), juce::Colour(186u, 34u, 34u), bounds.toFloat().getBottomRight(), false);
    grad.addColour(.5f, juce::Colours::transparentBlack);
    grad.addColour(.7f, juce::Colour(186u, 34u, 34u));

    g.setGradientFill(grad);
    g.fillAll();

    auto inputMeter = bounds.removeFromLeft(bounds.getWidth() * .1);
    auto meterLSide = inputMeter.removeFromLeft(inputMeter.getWidth() * .5);
    meter[0].setBounds(meterLSide);
    meter[1].setBounds(inputMeter);

    auto outputMeter = bounds.removeFromRight(bounds.getWidth() * .11);
    auto outMeterLSide = outputMeter.removeFromLeft(outputMeter.getWidth() * .5);
    outMeter[0].setBounds(outMeterLSide);
    outMeter[1].setBounds(outputMeter);

    //Making space for logo and text without distorting
    auto infoSpace = bounds.removeFromTop(bounds.getHeight() * .3);
    auto logoSpace = infoSpace.removeFromLeft(bounds.getWidth() * .425);
    auto textSpace = infoSpace.removeFromRight(bounds.getWidth() * .425);

    juce::FlexBox flexbox;
    flexbox.flexDirection = juce::FlexBox::Direction::row;
    flexbox.flexWrap = juce::FlexBox::Wrap::noWrap;

    flexbox.items.add(juce::FlexItem(roomSize).withFlex(1.f));
    flexbox.items.add(juce::FlexItem(damping).withFlex(1.f));
    flexbox.items.add(juce::FlexItem(width).withFlex(1.f));
    flexbox.items.add(juce::FlexItem(dryWet).withFlex(1.f));

    flexbox.performLayout(bounds);
   
    //add logo
    auto logo = juce::ImageCache::getFromMemory(BinaryData::KITIK_LOGO_NO_BKGD_png, BinaryData::KITIK_LOGO_NO_BKGD_pngSize);
    freeze.setImages(true, true, true, logo, 0, juce::Colours::white, juce::Image(), 0, juce::Colours::white, juce::Image(), 0, juce::Colour(64u, 194u, 230u));
    freeze.setBounds(infoSpace);

    //Add Text
    auto newFont = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::OFFSHORE_TTF, BinaryData::OFFSHORE_TTFSize));
    g.setColour(juce::Colours::whitesmoke);
    g.setFont(newFont);
    g.setFont(30.f);
    g.drawFittedText("Simple", logoSpace, juce::Justification::centredRight, 1);
    g.drawFittedText("Reverb", textSpace, juce::Justification::centredLeft, 1);

}

void SimpleReverbAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    auto inputMeter = bounds.removeFromLeft(bounds.getWidth() * .05);
    auto meterLSide = inputMeter.removeFromLeft(inputMeter.getWidth() * .5);
    meter[0].setBounds(meterLSide);
    meter[1].setBounds(inputMeter);

    auto outputMeter = bounds.removeFromRight(bounds.getWidth() * .053);
    auto outMeterLSide = outputMeter.removeFromLeft(outputMeter.getWidth() * .5);
    outMeter[0].setBounds(outMeterLSide);
    outMeter[1].setBounds(outputMeter);

    auto logoSpace = bounds.removeFromTop(bounds.getHeight() * .1);
    
    juce::FlexBox flexbox;
    flexbox.flexDirection = juce::FlexBox::Direction::row;
    flexbox.flexWrap = juce::FlexBox::Wrap::noWrap;

    flexbox.items.add(juce::FlexItem(roomSize).withFlex(1.f));
    flexbox.items.add(juce::FlexItem(damping).withFlex(1.f));
    flexbox.items.add(juce::FlexItem(width).withFlex(1.f));
    flexbox.items.add(juce::FlexItem(dryWet).withFlex(1.f));

    flexbox.performLayout(bounds);

}

void SimpleReverbAudioProcessorEditor::timerCallback()
{
    //these get our rms level, and the set level function tells you how much of the rect you want
    for (auto channel = 0; channel < audioProcessor.getTotalNumInputChannels(); channel++) {
        meter[channel].setLevel(audioProcessor.getRMS(channel));
        meter[channel].repaint();

        outMeter[channel].setLevel(audioProcessor.getOutRMS(channel));
        outMeter[channel].repaint();
    }
}