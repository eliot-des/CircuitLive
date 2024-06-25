/*
  ==============================================================================

    audioController.cpp
    Created: 28 May 2024 10:08:40am
    Author:  eliot

  ==============================================================================
*/

#include "AudioController.h"

AudioController::AudioController(juce::AudioProcessorValueTreeState& vts)
    : valueTreeState(vts)
{

    addAndMakeVisible(inputGain);
    inputGain.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    inputGain.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 17);
    inputGain.setTextValueSuffix(" dB");
    inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(valueTreeState, "input gain", inputGain);

    
    addAndMakeVisible(inputgainLabel);
    inputgainLabel.attachToComponent(&inputGain, false);
    inputgainLabel.setText("Input", juce::dontSendNotification);
    inputgainLabel.setJustificationType(juce::Justification::centredBottom);
    inputgainLabel.setBorderSize(juce::BorderSize<int>{2, 0, 0, 0 });

    addAndMakeVisible(outputGain);
    outputGain.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    outputGain.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    outputGain.setTextValueSuffix(" dB");
    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(valueTreeState, "output gain", outputGain);
    
    addAndMakeVisible(outputgainLabel);
    outputgainLabel.attachToComponent(&outputGain, false);
    outputgainLabel.setText("Output", juce::dontSendNotification);
    outputgainLabel.setJustificationType(juce::Justification::centredBottom);


    addAndMakeVisible(mixSlider);
    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    mixSlider.setTextValueSuffix(" %");
    mixSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(valueTreeState, "mix", mixSlider);

    addAndMakeVisible(mixLabel);
    mixLabel.attachToComponent(&mixSlider, false);
    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centredBottom);
}

AudioController::~AudioController() {}

void AudioController::paint(juce::Graphics& g)
{
}

void AudioController::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(20);
    //augment the width of the bounds to make room for the labels
    bounds.expand(10, 10);
    bounds.translate(0, 10);
    inputGain.setBounds(bounds.removeFromTop(bounds.getHeight() / 3).reduced(20));
    outputGain.setBounds(bounds.removeFromTop(bounds.getHeight() / 2).reduced(20));
    mixSlider.setBounds(bounds.reduced(20));
}

