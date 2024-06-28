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

    juce::Typeface::Ptr getTypefaceForFont(const juce::Font&) override;

    void drawLinearSlider(juce::Graphics&, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        juce::Slider::SliderStyle, juce::Slider&) override;


    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider& slider) override;

    juce::Font getLabelFont(juce::Label& label) override;

    
    void drawGroupComponentOutline(juce::Graphics& g, int width, int height,
        		const juce::String& text, const juce::Justification& position,
        		juce::GroupComponent& group) override;
private:

    juce::Typeface::Ptr carlitoRegular;
    juce::Typeface::Ptr carlitoBold;
    juce::Typeface::Ptr raleway;
    juce::Typeface::Ptr consolas;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LookAndFeel)
};