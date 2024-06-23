/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CircuitLiveAudioProcessor::CircuitLiveAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
     ),
#else 
    :

#endif
parameters(*this, nullptr, juce::Identifier("CircuitLive"), {
        std::make_unique<juce::AudioParameterFloat>("input gain", "Input Gain", juce::NormalisableRange{ -40.f, 40.f ,0.1f, 1.f, false }, 0.f),
        std::make_unique<juce::AudioParameterFloat>("output gain", "Output Gain", juce::NormalisableRange{ -40.f, 40.f ,0.1f, 1.f, false }, 0.f),
        std::make_unique<juce::AudioParameterInt>("mix", "Mix", 0, 100, 100),
        std::make_unique<juce::AudioParameterInt>("oversampling","Oversampling", 1, 4, 1),
        std::make_unique<juce::AudioParameterInt>("nrIterations", "Number of iterations", 1, 5, 1),
        std::make_unique<juce::AudioParameterFloat>("knob1", "Knob 1", juce::NormalisableRange{0.0f, 1.0f, 0.001f, 1.f, false }, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("knob2", "Knob 2", juce::NormalisableRange{0.0f, 1.0f, 0.001f, 1.f, false }, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("knob3", "Knob 3", juce::NormalisableRange{0.0f, 1.0f, 0.001f, 1.f, false }, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("knob4", "Knob 4", juce::NormalisableRange{0.0f, 1.0f, 0.001f, 1.f, false }, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("knob5", "Knob 5", juce::NormalisableRange{0.0f, 1.0f, 0.001f, 1.f, false }, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("knob6", "Knob 6", juce::NormalisableRange{0.0f, 1.0f, 0.001f, 1.f, false }, 0.5f),
    }) {

    inputGainParameter = parameters.getRawParameterValue("input gain");
    outputGainParameter = parameters.getRawParameterValue("output gain");
    mixPercentageParameter = parameters.getRawParameterValue("mix");

    oversamplingParameter = parameters.getRawParameterValue("oversampling");
    nrIterationsParameter = parameters.getRawParameterValue("nrIterations");

    //knob that can be associated to varistances or potentiometer
    knob1Parameter = parameters.getRawParameterValue("knob1");
    knob2Parameter = parameters.getRawParameterValue("knob2");
    knob3Parameter = parameters.getRawParameterValue("knob3");
    knob4Parameter = parameters.getRawParameterValue("knob4");
    knob5Parameter = parameters.getRawParameterValue("knob5");
    knob6Parameter = parameters.getRawParameterValue("knob6");  

    //initialize all the "oversamplers"
    for (int i = 0; i < 3; ++i) {
        oversampler[i] = std::make_unique<juce::dsp::Oversampling<float>>(getTotalNumInputChannels(), i + 1,
            juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR);
    }
};

CircuitLiveAudioProcessor::~CircuitLiveAudioProcessor()
{
}

//==============================================================================
const juce::String CircuitLiveAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CircuitLiveAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CircuitLiveAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CircuitLiveAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CircuitLiveAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CircuitLiveAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CircuitLiveAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CircuitLiveAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CircuitLiveAudioProcessor::getProgramName (int index)
{
    return {};
}

void CircuitLiveAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}


void CircuitLiveAudioProcessor::loadNetlistFile(const juce::String& path) {

    auto newSystem = std::make_shared<System>(); // Create a new netlist instance

    try {
        newSystem->init(path.toStdString()); // Initialize the new system with the netlist file
        if (newSystem->isInitialized) {
            newSystem->prepareChannels(getTotalNumInputChannels());
            newSystem->setSampleRate(currentSampleRate);
            newSystem->fillStaticSystem();
            newSystem->fillBaseSystem();
            newSystem->fillVariableSystem();
            newSystem->solveSystem();
            //netlistPath = path;
        }

    }
    catch (...) {
        // Handle exceptions or errors later...
        //netlistPath = {};  // Clear the path if loading failed.
    }
    // Update the path to the current netlist file even if loading the netlist have failed
    netlistPath = path;
    // Lock and swap
    std::lock_guard<std::mutex> lock(systemMutex);
    system = newSystem; // Atomically replace the old system with the new one
}

//==============================================================================
void CircuitLiveAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    for (auto& os : oversampler)
        os->initProcessing(samplesPerBlock);
}

void CircuitLiveAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CircuitLiveAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CircuitLiveAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    std::shared_ptr<System> localSystem;

    {
        std::lock_guard<std::mutex> lock(systemMutex);
        localSystem = system; // Copy the shared_ptr
    }

    if (!localSystem || !localSystem->isInitialized) {
        return;
    }


    const auto inputGain = inputGainParameter->load();
    const auto outputGain = outputGainParameter->load();
    const auto mixPercentage = mixPercentageParameter->load();

    const auto oversamplingIndex = static_cast<int>(oversamplingParameter->load());
    const auto nrIterations = nrIterationsParameter->load();

    const auto knob1 = knob1Parameter->load();
    const auto knob2 = knob2Parameter->load();
    const auto knob3 = knob3Parameter->load();
    const auto knob4 = knob4Parameter->load();
    const auto knob5 = knob5Parameter->load();
    const auto knob6 = knob6Parameter->load();


    localSystem->setInputGain(inputGain);
    localSystem->setOutputGain(outputGain);
    localSystem->setMixPercentage(mixPercentage);
    localSystem->setNrIterations(nrIterations);

    const double effectiveSampleRate = currentSampleRate * std::pow(2.0, oversamplingIndex - 1);
    if (effectiveSampleRate != localSystem->sampleRate) {
        localSystem->setSampleRate(effectiveSampleRate);
        localSystem->fillBaseSystem();
        localSystem->fillVariableSystem();
        localSystem->solveSystem();
        //for optimization we could just make a copy of A without reactive elements, and then add 
        //the stamp contributions of the reactive elements, rather than refilling the whole matrix
    }

    //if the knob positions change, we have to restamp A and recompute the system
    if (localSystem->knobPositions[0] != knob1 || localSystem->knobPositions[1] != knob2 ||
        localSystem->knobPositions[2] != knob3 || localSystem->knobPositions[3] != knob4 ||
        localSystem->knobPositions[4] != knob5 || localSystem->knobPositions[5] != knob6) {
        localSystem->setKnob1(knob1);
        localSystem->setKnob2(knob2);
        localSystem->setKnob3(knob3);
        localSystem->setKnob4(knob4);
        localSystem->setKnob5(knob5);
        localSystem->setKnob6(knob6);
        localSystem->fillVariableSystem();
        localSystem->solveSystem();
    }

    juce::dsp::AudioBlock<float> inputblock{ buffer };

    if (oversamplingIndex > 1) {
        auto oversampledblock = oversampler[oversamplingIndex - 2]->processSamplesUp(inputblock);
        localSystem->processBlock(oversampledblock);
        oversampler[oversamplingIndex - 2]->processSamplesDown(inputblock);
    }
    else {
        localSystem->processBlock(inputblock);
    }
}

//==============================================================================
bool CircuitLiveAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CircuitLiveAudioProcessor::createEditor()
{
    return new CircuitLiveAudioProcessorEditor (*this, parameters);
}

//==============================================================================

void CircuitLiveAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    // Add the netlist path as a new child element.
    xml->setAttribute("netlistPath", netlistPath);

    copyXmlToBinary(*xml, destData);
}

void CircuitLiveAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr && xmlState->hasTagName(parameters.state.getType())) {
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));

        // Read the netlist path attribute and update the member variable.
        if (xmlState->hasAttribute("netlistPath")) {
            netlistPath = xmlState->getStringAttribute("netlistPath");
            //loadNetlistFile(netlistPath);  // Optionally reload the netlist file

            // Notify the editor to update its content if it's visible
            if (auto* editor = dynamic_cast<CircuitLiveAudioProcessorEditor*>(getActiveEditor())) {

                editor->netlistEditor.displayNetlistFilePath(juce::File(netlistPath));
                editor->netlistEditor.displayFileContent(juce::File(netlistPath));
                editor->netlistEditor.readFile(juce::File(netlistPath));
            }
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CircuitLiveAudioProcessor();
}
