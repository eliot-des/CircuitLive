/*
  ==============================================================================

    netlistController.cpp
    Created: 28 May 2024 10:08:27am
    Author:  eliot

  ==============================================================================
*/

#include "NetlistController.h"

NetlistController::NetlistController(juce::AudioProcessorValueTreeState& vts)
    : valueTreeState(vts)
{

    juce::LookAndFeel::setDefaultLookAndFeel(&lookAndFeel);
    //rotary sliders
    addAndMakeVisible(knob1);
    knob1.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    //knob1.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 10, 10);
    knob1.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    knob1Attachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "knob1", knob1));
    knob1.setLookAndFeel(&lookAndFeel);


    addAndMakeVisible(knob2);
    knob2.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob2.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    knob2Attachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "knob2", knob2));

    addAndMakeVisible(knob3);
    knob3.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob3.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    knob3Attachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "knob3", knob3));

    addAndMakeVisible(knob4);
    knob4.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob4.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    knob4Attachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "knob4", knob4));

    //linear sliders
    addAndMakeVisible(knob5);
    knob5.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    knob5.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    knob5Attachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "knob5", knob5));

    
    knob6.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    knob6.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    knob6Attachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "knob6", knob6));

    knob6.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    //knob6.setPopupDisplayEnabled(true, true, this, -1);
    //knob6.setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(knob6);

}

NetlistController::~NetlistController()
{
    setLookAndFeel(nullptr);
}

void NetlistController::paint(juce::Graphics& g)
{
}

void NetlistController::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(10);
    auto bottomArea = bounds.removeFromBottom(bounds.getHeight() / 3);
    knob5.setBounds(bottomArea.removeFromTop(bottomArea.getHeight() / 2).reduced(5));
    knob6.setBounds(bottomArea.reduced(5));
    
    auto rightArea = bounds.removeFromRight(bounds.getWidth() / 2);
    knob1.setBounds(bounds.removeFromTop(bounds.getHeight() / 2).reduced(5));
    knob2.setBounds(bounds.reduced(5));
    knob3.setBounds(rightArea.removeFromTop(rightArea.getHeight() / 2).reduced(5));
    knob4.setBounds(rightArea.reduced(5));
   
}