/*
  ==============================================================================

    netlistController.h
    Created: 28 May 2024 10:08:27am
    Author:  eliot

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class NetlistController : public juce::Component
{
public:
    NetlistController(juce::AudioProcessorValueTreeState& vts);
    ~NetlistController() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::AudioProcessorValueTreeState& valueTreeState;

    juce::Slider knob1;
    juce::Slider knob2;
    juce::Slider knob3;
    juce::Slider knob4;
    juce::Slider knob5;
    juce::Slider knob6;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> knob1Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> knob2Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> knob3Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> knob4Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> knob5Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> knob6Attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NetlistController)
};