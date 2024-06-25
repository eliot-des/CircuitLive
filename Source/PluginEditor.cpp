/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

CircuitLiveAudioProcessorEditor::CircuitLiveAudioProcessorEditor(CircuitLiveAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), audioProcessor(p), parameters(vts),
    netlistEditor(p),
    netlistController(vts),
    audioController(vts),
    simulationParameters(vts),
    netlistEditorGroup("Netlist Editor", "Netlist Editor"),
    netlistControllerGroup("Netlist Controller", "Netlist Controller"),
    audioControllerGroup("Audio Controls", "Audio Controls"),
    simulationParametersGroup("Simulation Parameters", "Simulation Parameters")
{

    setLookAndFeel(&lookAndFeel);
    
    addAndMakeVisible(netlistEditorGroup);
    netlistEditorGroup.setTextLabelPosition(juce::Justification::centred);
    netlistEditorGroup.addAndMakeVisible(netlistEditor);
    netlistEditorGroup.sendLookAndFeelChange();
    


    addAndMakeVisible(netlistControllerGroup);
    netlistControllerGroup.setTextLabelPosition(juce::Justification::centred);
    netlistControllerGroup.addAndMakeVisible(netlistController);
    netlistControllerGroup.sendLookAndFeelChange();


    addAndMakeVisible(audioControllerGroup);
    audioControllerGroup.setTextLabelPosition(juce::Justification::centred);
    audioControllerGroup.addAndMakeVisible(audioController);
    audioController.sendLookAndFeelChange();

    addAndMakeVisible(simulationParametersGroup);
    simulationParametersGroup.setTextLabelPosition(juce::Justification::centred);
    simulationParametersGroup.addAndMakeVisible(simulationParameters);
    simulationParametersGroup.sendLookAndFeelChange();

    double ratio = 3.0 /2.0;
    
    setSize(800, 533);
    setResizable(true, true);
    setResizeLimits(600, 400, 1200, 800);
    getConstrainer()->setFixedAspectRatio(ratio);

}

CircuitLiveAudioProcessorEditor::~CircuitLiveAudioProcessorEditor() {
    setLookAndFeel(nullptr);
}

void CircuitLiveAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::darkgrey);
    g.setFont(juce::Font("Century Gothic", 53.0f, juce::Font::bold));
    g.drawFittedText("Circuit-Live", 20, 20, 300, 60, juce::Justification::bottomLeft, 1);
}

void CircuitLiveAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(10);
    area.removeFromBottom(area.getHeight() *0.857);  //(*6/7)

    auto simuBounds = area.removeFromRight(area.getWidth() / 2);
    simulationParametersGroup.setBounds(simuBounds.reduced(5));
    simulationParameters.setBounds(simulationParametersGroup.getLocalBounds().reduced(10));

    auto bounds = getLocalBounds().reduced(10);
    bounds.removeFromTop(bounds.getHeight() * 0.1428); //(*1/7)

    auto editorBounds = bounds.removeFromLeft(bounds.getWidth() * 0.5);
    netlistEditorGroup.setBounds(editorBounds.reduced(5));
    netlistEditor.setBounds(netlistEditorGroup.getLocalBounds().reduced(10));

    auto controllerBounds = bounds.removeFromLeft(bounds.getWidth() *2/3);
    netlistControllerGroup.setBounds(controllerBounds.reduced(5));
    netlistController.setBounds(netlistControllerGroup.getLocalBounds().reduced(10));

    auto audioBounds = bounds;
    audioControllerGroup.setBounds(audioBounds.reduced(5));
    audioController.setBounds(audioControllerGroup.getLocalBounds().reduced(10));
}
