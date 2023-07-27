/*
  ==============================================================================

    KiTiKLNF.h
    Created: 26 Jul 2023 6:20:24pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
struct Laf : juce::LookAndFeel_V4 {

    Laf() {}

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    struct LevelMeter : juce::Component
    {
        void paint(juce::Graphics& g) override;
        
        //default value so the meters  are black when the plugin is launched
        void setLevel(float value) { level = value; }

    private:
        float level = -60.f;
    };
};