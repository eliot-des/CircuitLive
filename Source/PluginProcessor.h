/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "System.h"
//==============================================================================
/**
*/
class CircuitLiveAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    CircuitLiveAudioProcessor();
    ~CircuitLiveAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    std::shared_ptr<System> system;
    std::mutex systemMutex;

    void loadNetlistFile(const juce::String& path);

    double currentSampleRate = 0.0;

    juce::String netlistPath;



private:

     juce::AudioProcessorValueTreeState parameters;

    std::atomic<float>* inputGainParameter = nullptr;
    std::atomic<float>* outputGainParameter = nullptr;
    std::atomic<float>* mixPercentageParameter = nullptr;

    std::atomic<float>* oversamplingParameter = nullptr; 
    std::atomic<float>* nrIterationsParameter = nullptr; //maximal number of Newton-Raphson iterations for the solver

    std::atomic<float>* knob1Parameter = nullptr;
    std::atomic<float>* knob2Parameter = nullptr;
    std::atomic<float>* knob3Parameter = nullptr;
    std::atomic<float>* knob4Parameter = nullptr;
    std::atomic<float>* knob5Parameter = nullptr;
    std::atomic<float>* knob6Parameter = nullptr;

    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler[3];
    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CircuitLiveAudioProcessor)
};
