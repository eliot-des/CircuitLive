/*
  ==============================================================================

    simulationParameters.cpp
    Created: 28 May 2024 10:08:57am
    Author:  eliot

  ==============================================================================
*/

#include "SimulationParameters.h"

SimulationParameters::SimulationParameters(juce::AudioProcessorValueTreeState& vts)
{
    setLookAndFeel(&lookAndFeel);

    addAndMakeVisible(oversamplingBox);
    oversamplingBox.setScrollWheelEnabled(true);
    oversamplingBox.addItem("1x", 1);
    oversamplingBox.addItem("2x", 2);
    oversamplingBox.addItem("4x", 3);
    oversamplingBox.addItem("8x", 4);
    oversamplingBox.setSelectedId(1);
    
    addAndMakeVisible(osLabel);
    osLabel.attachToComponent(&oversamplingBox, true);
    osLabel.setText("Oversampling :", juce::dontSendNotification);
    osLabel.setJustificationType(juce::Justification::centred);

    oversamplingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(vts, "oversampling", oversamplingBox);


    addAndMakeVisible(nrIterationsBox);
    nrIterationsBox.setScrollWheelEnabled(true);
    nrIterationsBox.addItem("1", 1);
    nrIterationsBox.addItem("2", 2);
    nrIterationsBox.addItem("4", 3);
    nrIterationsBox.addItem("8", 4);
    nrIterationsBox.addItem("16", 5);
    nrIterationsBox.setSelectedId(3);

    addAndMakeVisible(nrIterationsLabel);
    nrIterationsLabel.attachToComponent(&nrIterationsBox, true);
    nrIterationsLabel.setText("Max iterations :", juce::dontSendNotification);
    nrIterationsLabel.setJustificationType(juce::Justification::centred);

    nrIterationsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(vts, "nrIterations", nrIterationsBox);
}

SimulationParameters::~SimulationParameters()
{
	setLookAndFeel(nullptr);
}


void SimulationParameters::paint(juce::Graphics& g)
{
}

void SimulationParameters::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    bounds.translate(0,5);
    oversamplingBox.setBounds(bounds.removeFromRight(getWidth()/2).removeFromRight(getWidth()/5));


    nrIterationsBox.setBounds(bounds.removeFromRight(getWidth() / 5));
}
