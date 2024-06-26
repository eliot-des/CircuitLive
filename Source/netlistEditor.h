/*
  ==============================================================================

    netlistEditor.h
    Created: 28 May 2024 10:07:57am
    Author:  eliot

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"

class NetlistEditor : public juce::Component, 
                      public juce::FilenameComponentListener
{
public:
    NetlistEditor(CircuitLiveAudioProcessor& processor);
    ~NetlistEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void filenameComponentChanged(juce::FilenameComponent* fileComponentThatHasChanged) override;
    void updateButtonClicked();


    void displayNetlistFilePath(const juce::File& fileToDisplay);
    void displayFileContent(const juce::File& fileToDisplay);
    void readFile(const juce::File& fileToRead);

private:

    LookAndFeel lookAndFeel;

    CircuitLiveAudioProcessor& audioProcessor;

    std::unique_ptr<juce::FilenameComponent> fileComp;
    std::unique_ptr<juce::TextEditor> textContent;
    juce::TextButton updateButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NetlistEditor)
};

