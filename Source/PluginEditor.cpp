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
    audioControllerGroup("Audio", "Audio"),
    simulationParametersGroup("Simulation Parameters", "Simulation Parameters")
{

    //setLookAndFeel(&lookAndFeel);


    addAndMakeVisible(logoText);
    logoText.setText("Circuit-Live");
    logoText.setColour(juce::Colour::fromRGB(243, 239, 234));
    logoText.setJustification(juce::Justification::centredLeft);
    logoText.setFont(juce::Font("Century Gothic", 53.0f, juce::Font::bold), true);

  
    addAndMakeVisible(netlistEditorGroup);
    netlistEditorGroup.setTextLabelPosition(juce::Justification::centred);
    netlistEditorGroup.addAndMakeVisible(netlistEditor);
    netlistEditorGroup.setLookAndFeel(&lookAndFeel);
    




    addAndMakeVisible(simulationParametersGroup);
    simulationParametersGroup.setTextLabelPosition(juce::Justification::centred);
    simulationParametersGroup.addAndMakeVisible(simulationParameters);
    simulationParametersGroup.setLookAndFeel(&lookAndFeel);
    simulationParametersGroup.setColour(juce::GroupComponent::outlineColourId, juce::Colours::darkgrey);
    simulationParametersGroup.setColour(juce::GroupComponent::textColourId, juce::Colours::darkgrey);
    simulationParameters.getLookAndFeel().setColour(juce::Label::ColourIds::textColourId, juce::Colours::grey.brighter(1.0f));
    simulationParameters.getLookAndFeel().setColour(juce::ComboBox::backgroundColourId, juce::Colours::black.brighter(0.09f));
    simulationParameters.getLookAndFeel().setColour(juce::ComboBox::outlineColourId, juce::Colours::black.brighter(0.09f));
    simulationParameters.getLookAndFeel().setColour(juce::ComboBox::textColourId, juce::Colours::grey.brighter(1.0f));
    simulationParameters.getLookAndFeel().setColour(juce::ComboBox::arrowColourId, juce::Colours::grey.brighter(1.0f));
    simulationParameters.getLookAndFeel().setColour(juce::PopupMenu::backgroundColourId, juce::Colours::black.brighter(0.09f));
    simulationParameters.getLookAndFeel().setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colours::black.brighter(0.15f));
    simulationParameters.getLookAndFeel().setColour(juce::PopupMenu::textColourId, juce::Colours::whitesmoke);

    addAndMakeVisible(netlistControllerGroup);
    netlistControllerGroup.setTextLabelPosition(juce::Justification::centred);
    netlistControllerGroup.addAndMakeVisible(netlistController);
    netlistControllerGroup.setLookAndFeel(&lookAndFeel);
    

    addAndMakeVisible(audioControllerGroup);
    audioControllerGroup.setTextLabelPosition(juce::Justification::centred);
    audioControllerGroup.addAndMakeVisible(audioController);
    audioControllerGroup.setLookAndFeel(&lookAndFeel);

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
    //background color
    g.fillAll(juce::Colour::fromRGB(243, 239, 234)); 


    //top rectangle
    auto xRect = static_cast<int> (getLocalBounds().getWidth());
    auto yRect = static_cast<int> (getLocalBounds().getHeight() * 0.1428f); //(*1/7)
    juce::Rectangle<int> topArea(juce::Point<int>(0, 0), juce::Point<int>(xRect, yRect));
    g.setColour(juce::Colour::fromRGB(38, 38, 38)); //top rectangle color
    g.fillRect(topArea);

    //draw aline on the bottom of the top rectangle
    g.setColour(juce::Colours::black.brighter(0.09f));
    g.drawLine(0, yRect, xRect, yRect, 4);

    //draw a line between the logo and the simulation parameters
    //g.drawLine(getLocalBounds().getWidth()*0.41, 0, getLocalBounds().getWidth() * 0.41, getLocalBounds().getHeight() * 0.1428f, 4);
}

void CircuitLiveAudioProcessorEditor::resized()
{
    auto topArea = getLocalBounds().reduced(10);
    topArea.removeFromBottom(topArea.getHeight() * 0.857f);

    auto bottomArea = getLocalBounds().reduced(10);
    bottomArea.removeFromTop(bottomArea.getHeight() * 0.1428f);


    //Top Area
    auto areaTemp = topArea.removeFromLeft(topArea.getWidth() *0.36);
    logoText.setBoundingBox(juce::Parallelogram(areaTemp.translated(7, -areaTemp.getHeight() * 0.15f).toFloat()));
    logoText.setFontHeight(logoText.getHeight()*0.8f);

    simulationParametersGroup.setBounds(topArea.reduced(5));
    simulationParametersGroup.setBounds(simulationParametersGroup.getBounds().translated(0, -simulationParametersGroup.getHeight() * 0.2f));
    simulationParameters.setBounds(simulationParametersGroup.getLocalBounds().reduced(10));

    
    //Bottom Area
    auto editorBounds = bottomArea.removeFromLeft(bottomArea.getWidth() * 0.5);
    netlistEditorGroup.setBounds(editorBounds.reduced(5));
    netlistEditor.setBounds(netlistEditorGroup.getLocalBounds().reduced(10));

    auto controllerBounds = bottomArea.removeFromLeft(bottomArea.getWidth() *2/3);
    netlistControllerGroup.setBounds(controllerBounds.reduced(5));
    netlistController.setBounds(netlistControllerGroup.getLocalBounds().reduced(10));

    auto audioBounds = bottomArea;
    audioControllerGroup.setBounds(audioBounds.reduced(5));
    audioController.setBounds(audioControllerGroup.getLocalBounds().reduced(10));
}
