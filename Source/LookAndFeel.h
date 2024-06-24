/*
  ==============================================================================

    LookAndFeel.h
    Created: 24 Jun 2024 10:42:41am
    Author:  eliot

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>

class LookAndFeel : public juce::LookAndFeel_V4
{
public:
    LookAndFeel();

    void drawLinearSlider(juce::Graphics&, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        juce::Slider::SliderStyle, juce::Slider&) override;


    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider& slider) override;

    juce::Font getLabelFont(juce::Label& label) override;


private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LookAndFeel)
};