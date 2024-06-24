/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"


#include "NetlistEditor.h"
#include "NetlistController.h"
#include "AudioController.h"
#include "SimulationParameters.h"


class CircuitLiveAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    CircuitLiveAudioProcessorEditor(CircuitLiveAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~CircuitLiveAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    NetlistEditor netlistEditor;

private:
    LookAndFeel lookAndFeel;

    CircuitLiveAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& parameters;

    
    NetlistController netlistController;
    AudioController audioController;
    SimulationParameters simulationParameters;

    juce::GroupComponent netlistEditorGroup;
    juce::GroupComponent netlistControllerGroup;
    juce::GroupComponent audioControllerGroup;
    juce::GroupComponent simulationParametersGroup;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CircuitLiveAudioProcessorEditor)
};
