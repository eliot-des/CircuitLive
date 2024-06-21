/*
  ==============================================================================

    system.cpp
    Created: 28 May 2024 10:07:36am
    Author:  eliot

  ==============================================================================
*/

#include "system.h"
#include "component.h"
#include "chrono"

System::System(const std::string& filename) {
    init(filename);
}

void System::init(const std::string& filename) {
    allComponents = createComponentListFromTxt(filename);
    categorizeComponents(); // categorize components into static, dynamic, and nonlinear, and get all voltage probes

    //have to improve this
    if (allComponents.empty() || voltageProbes.empty()) {
        isInitialized = false;
        return;
    }
    else isInitialized = true;


    knobPositions = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    setupSystem();
    setProcessBlockStrategy();
}

//================================================================================================
//================================================================================================

void System::reset() {
    staticComponents.clear();
    dynamicComponents.clear();
    variableComponents.clear();
    nonlinearComponents.clear();
    externalVoltageSources.clear();
    voltageProbes.clear();

    
    A.resize(0, 0);
    A.data().squeeze(); // don't know if it's necessary, have to ask on the Eigen's discord
    x.resize(0);
    b.resize(0);

    A.setZero();
    x.setZero();
    b.setZero();

    channelBStates.clear();
    channelXStates.clear();

    n = 0;
    indexNbrs = 0;

    isInitialized = false;
}


//================================================================================================
//================================================================================================

void System::categorizeComponents() {
    // Reserve space for vectors ?
    //staticComponents.reserve(allComponents.size());
    //dynamicComponents.reserve(allComponents.size());
    //nonlinearComponents.reserve(allComponents.size());

    for (const auto& comp : allComponents) {
        if (comp->is_static()) {
            staticComponents.push_back(comp);
        }
        else if (comp->is_dynamic()) {
            dynamicComponents.push_back(comp);
        }
        else if (comp->is_variable()) {
            variableComponents.push_back(comp);
        }
        else if (comp->is_nonlinear()) {
            nonlinearComponents.push_back(comp);
        }
    }

    externalVoltageSources = getComponents<ExternalVoltageSource>();
    voltageProbes = getComponents<VoltageProbe>();
}

void System::setupSystem() {
    n = getNodeNbr();

    unsigned int n_ = n - 1;
    unsigned int m = indexNbrs;

    A.resize(n_ + m, n_ + m);
    x.resize(n_ + m);
    b.resize(n_ + m);

    x.setZero();
    b.setZero();

    for (const auto& comp : allComponents) {
        comp->register_component(*this);
    }

    A.makeCompressed();  // Optimize the storage of A only once

    for (const auto& comp : allComponents) {
        comp->setup(*this);
    }
}


void System::fillStaticSystem() {
    for (const auto& comp : staticComponents) {
        comp->stamp_A(*this);
        comp->stamp_b(*this);
    }
    A_static = A;
    b_static = b;
}

void System::fillBaseSystem() {
    A = A_static;//make a copy of the static base matrix
    b = b_static;

    for (const auto& comp : dynamicComponents) {
        comp->stamp_A(*this);
        comp->stamp_b(*this);
    }
    A_dyn = A;//make a copy of the base matrix
    b_dyn = b;
}

void System::fillVariableSystem() {
    A = A_dyn;
    b = b_dyn;

    for (const auto& comp : variableComponents) {
        comp->stamp_A(*this); //normally the stamp_A function should include something to update the component value
        comp->stamp_b(*this);
    }
    A_var = A; //make a copy of the variable matrix
    b_var = b;
}

void System::fillNonlinearSystem() {
	A = A_var;
    b = b_var;

	for (const auto& comp : nonlinearComponents) {
		comp->stamp_A(*this);
    comp->stamp_b(*this);
	}
}

void System::setSparseMatrixEntry(unsigned row, unsigned col) {
    if (row == 0 || col == 0) return;
    A.coeffRef(row - 1, col - 1) += 0;
}

//================================================================================================
//================================================================================================


void System::solveSystem() {
    solver.analyzePattern(A);
    solver.factorize(A);
}


void System::setProcessBlockStrategy(){
    if (nonlinearComponents.empty()) {
        processBlock = std::bind(&System::processBlockLinear, this, std::placeholders::_1);
    }
    else {
        processBlock = std::bind(&System::processBlockNonlinear, this, std::placeholders::_1);
    }
}

void System::processBlockLinear(juce::dsp::AudioBlock<float>& audioBlock) {

    const auto mix = mixPercentage / 100.0f;

    for (auto channel = 0; channel < audioBlock.getNumChannels(); ++channel) {
        auto* channelSamples = audioBlock.getChannelPointer(channel);

        // Use saved states for this channel
        b = channelBStates[channel];
        x = channelXStates[channel];

        //core of the algorithm
        for (auto n = 0; n < audioBlock.getNumSamples(); n++) {
            const auto inputSample = channelSamples[n];
            const auto inputCircuitSample = inputSample * std::pow(10.0f, inputGain / 20.0f);

            for (const auto& comp : externalVoltageSources) comp->updateVoltage(inputCircuitSample);
            for (const auto& comp : dynamicComponents) comp->stamp_b(*this);

            x = solver.solve(b);

            //test with the first voltage probe
            float outputCircuitSample = voltageProbes[0]->getVoltage();

            float outputSample = outputCircuitSample * std::pow(10.0f, outputGain / 20.0f);
            channelSamples[n] = outputSample * mix + (1 - mix) * inputSample;
        }

        // Save the updated states for this channel
        channelBStates[channel] = b;
        channelXStates[channel] = x;
    }
}


void System::processBlockNonlinear(juce::dsp::AudioBlock<float>& audioBlock) {
	const auto mix = mixPercentage / 100.0f;

	for (auto channel = 0; channel < audioBlock.getNumChannels(); ++channel) {
		auto* channelSamples = audioBlock.getChannelPointer(channel);

		b = channelBStates[channel];
		x = channelXStates[channel];

		for (auto n = 0; n < audioBlock.getNumSamples(); n++) {
			const auto inputSample = channelSamples[n];
			const auto inputCircuitSample = inputSample * std::pow(10.0f, inputGain / 20.0f);

			for (const auto& comp : externalVoltageSources) comp->updateVoltage(inputCircuitSample);
			for (const auto& comp : dynamicComponents) comp->stamp_b(*this);

            for (auto i = 0; i < nrIterations; i++) { // Newton-Raphson iterations

                fillNonlinearSystem();


                x_old = x;


                if (i == 0) {
					solver.analyzePattern(A);
				}
                //definitely have to improve this

                solver.factorize(A);
                x = solver.solve(b);

                if ((x - x_old).norm() < 1e-6) {
					break;
				}
            }

			//test with the first voltage probe
			float outputCircuitSample = voltageProbes[0]->getVoltage();

			float outputSample = outputCircuitSample * std::pow(10.0f, outputGain / 20.0f);
			channelSamples[n] = outputSample * mix + (1 - mix) * inputSample;
		}

		// Save the updated states for this channel
		channelBStates[channel] = b;
		channelXStates[channel] = x;
	}
}

//====================================================================================================
//====================================================================================================

void System::setInputGain(float inputGain) {
    this->inputGain = inputGain;
}


void System::setOutputGain(float outputGain) {
    this->outputGain = outputGain;
}

void System::setMixPercentage(float mixPercentage) {
    this->mixPercentage = mixPercentage;
}

void System::setSampleRate(double sampleRate) {
    this->sampleRate = sampleRate;
    for (const auto& comp : dynamicComponents) {
        if (dynamic_cast<ReactiveComponent*>(comp.get()) != nullptr) {
            dynamic_cast<ReactiveComponent*>(comp.get())->setResistance(1 / sampleRate);
        }
    }
}

void System::setNrIterations(unsigned int nrIterations) {
	this->nrIterations = nrIterations;
}

void System::setKnob1(float knob1) {
    this->knobPositions[0] = knob1;
}

void System::setKnob2(float knob2) {
    this->knobPositions[1] = knob2;
}

void System::setKnob3(float knob3) {
    this->knobPositions[2] = knob3;
}

void System::setKnob4(float knob4) {
    this->knobPositions[3] = knob4;
}
void System::setKnob5(float knob5) {
    this->knobPositions[4] = knob5;
}

void System::setKnob6(float knob6) {
    this->knobPositions[5] = knob6;
}

void System::setNrIterations(unsigned nrIterations) {
	this->nrIterations = nrIterations;
}

void System::prepareChannels(int numChannels) {
    channelBStates.resize(numChannels, b);
    channelXStates.resize(numChannels, x);
}


//================================================================================================
//================================================================================================


unsigned System::getNodeNbr() {
    std::unordered_set<unsigned> nodes;
    for (const auto& comp : allComponents) {
        nodes.insert(comp->start_node);
        nodes.insert(comp->end_node);
    }
    return nodes.size();
}


//================================================================================================
//================================================================================================


std::vector<std::string> System::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::shared_ptr<Component> System::createComponent(const std::string& netlistLine, unsigned idx) {
    auto tokens = split(netlistLine, ' ');
    std::string symbol = tokens[0];
    unsigned start_node = std::stoi(tokens[1]);
    unsigned end_node = std::stoi(tokens[2]);
    double value = std::stod(tokens[3]);


    switch (symbol[0]) {
    case 'R':
        if (tokens.size() == 4) {
            // It's a resistance
            return std::make_shared<Resistance>(start_node, end_node, value);
        }
        else if (tokens.size() == 5) {
            return std::make_shared<Varistance>(start_node, end_node, value, std::stoi(tokens[4]));
        }
        else if (tokens.size() == 6 && std::isalpha(tokens[5][0])) {
            return std::make_shared<Varistance>(start_node, end_node, value, std::stoi(tokens[4]), tokens[5][0]);
        }
        else {
            throw std::runtime_error("Incorrect number of tokens for res/varistance: " + netlistLine);
        }
	case 'P':
        if (tokens.size() == 6) {
            return std::make_shared<Potentiometer>(start_node, end_node, std::stoi(tokens[3]), std::stod(tokens[4]), std::stoi(tokens[5]));
        }
        else if (tokens.size() == 7 && std::isalpha(tokens[6][0])) {
            return std::make_shared<Potentiometer>(start_node, end_node, std::stoi(tokens[3]), std::stod(tokens[4]), std::stoi(tokens[5]), tokens[6][0]);
        }
        else {
            throw std::runtime_error("Incorrect number of tokens for potentiometer: " + netlistLine);
        }
    case 'V':
        if (symbol[1] == 'n') {
            return std::make_shared<ExternalVoltageSource>(start_node, end_node, value, idx);
        }
        else if (symbol[1] == 'o') {
            return std::make_shared<VoltageProbe>(start_node, end_node);
        }
        else {
            return std::make_shared<VoltageSource>(start_node, end_node, value, idx);
        }
    case 'C':
        return std::make_shared<Capacitor>(start_node, end_node, value, idx);
    case 'L':
        return std::make_shared<Inductance>(start_node, end_node, value, idx);
    case 'O':
        return std::make_shared<IdealOPA>(start_node, end_node, value, idx);
    case 'D':
        return std::make_shared<Diode>(start_node, end_node);
    case '#':
        return nullptr;
    default:
        throw std::runtime_error("Unknown component symbol: " + symbol);
    }
}

std::vector<std::shared_ptr<Component>> System::createComponentListFromTxt(const std::string& filename) {
    std::vector<std::shared_ptr<Component>> components;
    std::ifstream netlistTxt(filename);
    std::string line;
    indexNbrs = 0;

    if (netlistTxt.is_open()) {
        while (std::getline(netlistTxt, line)) {
            if (!line.empty()) {
                auto component = createComponent(line, indexNbrs);

                if (dynamic_cast<VoltageSource*>(component.get()) != nullptr ||
                    dynamic_cast<ReactiveComponent*>(component.get()) != nullptr ||
                    dynamic_cast<IdealOPA*>(component.get()) != nullptr) {
                    indexNbrs++;
                }

                if (component) {
                    components.push_back(std::move(component));
                }
            }
        }
        netlistTxt.close();
    }
    else {
        std::cout << "Unable to open the netlist file" << std::endl;
    }
    return components;
}

