/*
  ==============================================================================

    audioController.h
    Created: 28 May 2024 10:08:40am
    Author:  eliot

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class AudioController : public juce::Component
{
public:
    AudioController(juce::AudioProcessorValueTreeState& vts);
    ~AudioController() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:

    juce::Slider inputGain;
    juce::Slider outputGain;
    juce::Slider mixSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixSliderAttachment;

    juce::Label inputgainLabel;
    juce::Label outputgainLabel;
    juce::Label mixLabel;

    juce::AudioProcessorValueTreeState& valueTreeState;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioController)
};