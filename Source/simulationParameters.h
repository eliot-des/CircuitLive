/*
  ==============================================================================

    simulationParameters.h
    Created: 28 May 2024 10:08:57am
    Author:  eliot

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SimulationParameters : public juce::Component
{
public:
    SimulationParameters(juce::AudioProcessorValueTreeState& vts);
    ~SimulationParameters() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::ComboBox oversamplingBox;
    juce::Label osLabel;

    juce::ComboBox nrIterationsBox;
    juce::Label nrIterationsLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> oversamplingAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> nrIterationsAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimulationParameters)
};
