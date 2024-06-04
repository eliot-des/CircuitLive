/*
  ==============================================================================

    netlistEditor.cpp
    Created: 28 May 2024 10:07:57am
    Author:  eliot

  ==============================================================================
*/

#include "netlistEditor.h"

NetlistEditor::NetlistEditor(CircuitLiveAudioProcessor& processor)
    : audioProcessor(processor)
{
    // Initialize and add components
    fileComp.reset(new juce::FilenameComponent("fileComp",
        {},                       // current file
        false,                     // can edit file name,
        false,                    // is directory,
        false,                    // is for saving,
        "*.txt",                  // browser wildcard suffix,
        {},                       // enforced suffix,
        "Select netlist to open"));  // text when nothing selected
    fileComp->setDefaultBrowseTarget(juce::File::getSpecialLocation(juce::File::userDesktopDirectory));
    addAndMakeVisible(fileComp.get());
    fileComp->addListener(this);

    //======================TextEditor==================================

    textContent.reset(new juce::TextEditor());
    addAndMakeVisible(textContent.get());
    textContent->setMultiLine(true);
    textContent->setReadOnly(false);
    textContent->setCaretVisible(true);
    textContent->setPopupMenuEnabled(true);
    textContent->setScrollbarsShown(true);
    textContent->setReturnKeyStartsNewLine(true);
    textContent->setFont(juce::Font("Consolas", 20.0f, juce::Font::plain));

    //======================Save Button==================================
    addAndMakeVisible(updateButton);
    updateButton.setButtonText("Update");
    updateButton.onClick = [this] { updateButtonClicked(); };
}

NetlistEditor::~NetlistEditor() {}

void NetlistEditor::paint(juce::Graphics& g)
{
    //g.fillAll(juce::Colours::darkgrey);
}

void NetlistEditor::resized()
{
    
    auto bounds = getLocalBounds();
    bounds.removeFromTop(10);
    //auto textContentHeight = bounds.getHeight()/10);
    textContent->setBounds(bounds.removeFromBottom(bounds.getHeight() *0.9).reduced(5));
    updateButton.setBounds(bounds.removeFromRight(bounds.getWidth() / 5).reduced(5));
    fileComp->setBounds(bounds.reduced(5));

}

void NetlistEditor::filenameComponentChanged(juce::FilenameComponent* fileComponentThatHasChanged)
{
    if (fileComponentThatHasChanged == fileComp.get())
        readFile(fileComp->getCurrentFile());
}

void NetlistEditor::readFile(const juce::File& fileToRead)
{
    if (!fileToRead.existsAsFile())
        return;

    audioProcessor.loadNetlistFile(fileToRead.getFullPathName());
    auto fileText = fileToRead.loadFileAsString();
    textContent->setText(fileText);
}

void NetlistEditor::updateButtonClicked()
{
    if (audioProcessor.netlistPath.isNotEmpty()) {
        juce::File file(audioProcessor.netlistPath);
        file.replaceWithText(textContent->getText());
        readFile(file);
    }
}
