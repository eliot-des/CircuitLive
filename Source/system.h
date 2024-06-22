/*
  ==============================================================================

    system.h
    Created: 28 May 2024 10:07:36am
    Author:  eliot

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <unordered_set>
#include "EigenWrapper.h"
//#include <Eigen/KLUSupport> //seem to be by far the best LU solver for sparse matrices for circuit simulation available in Eigen, don't know why it don't build the vst3...
#include <Eigen/Sparse>

// Forward declarations to avoid circular dependencies
class Component;
class ExternalVoltageSource;
class VoltageProbe;


class System {
public:
    std::vector<std::shared_ptr<Component>> allComponents;
    std::vector<std::shared_ptr<Component>> staticComponents;
    std::vector<std::shared_ptr<Component>> variableComponents;
    std::vector<std::shared_ptr<Component>> dynamicComponents;
    std::vector<std::shared_ptr<Component>> nonlinearComponents;
    std::vector<std::shared_ptr<ExternalVoltageSource>> externalVoltageSources;
    std::vector<std::shared_ptr<VoltageProbe>> voltageProbes;

    Eigen::SparseMatrix<double> A, A_static, A_base, A_var, A_temp;
    Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
    Eigen::VectorXd x, x_old, b, b_static, b_temp;

    std::vector<Eigen::VectorXd> channelBStates;
    std::vector<Eigen::VectorXd> channelXStates;

    unsigned n;         // Number of unique nodes including the ground node (0)
    unsigned indexNbrs; // Number of additionnal indexes for the matrix A

    float inputGain;
    float outputGain;
    float mixPercentage;

    double sampleRate;
    unsigned int nrIterations;
    //knobs positions (size of 6 for now, but can be extended)
    std::vector<float> knobPositions;



    bool isInitialized = false;


    System() = default;
    explicit System(const std::string& filename);

    //This three methods are used to initialize the system
    void init(const std::string& filename);
    void fillStaticSystem();
    void fillBaseSystem();
    void fillVariableSystem();

    //used to remove all the components from the system, reset A, x and b, and reset the solver (to avoid memory leaks)
    void reset();


    //call by the components objects to set the non-zero entries of the matrix A
    void setSparseMatrixEntry(unsigned row, unsigned col);

    void solveSystem();

    void setInputGain(float inputGain);
    void setOutputGain(float outputGain);
    void setMixPercentage(float mixPercentage);

    void setKnob1(float knob1);
    void setKnob2(float knob2);
    void setKnob3(float knob3);
    void setKnob4(float knob4);
    void setKnob5(float knob5);
    void setKnob6(float knob6);

    void setSampleRate(double sampleRate);
    void prepareChannels(int numChannels);
    void setNrIterations(unsigned nrIterations);


    void setProcessBlockStrategy(); 
    std::function<void(juce::dsp::AudioBlock<float>&)> processBlock; // Function pointer to the appropriate processBlock method
    void processBlockLinear(juce::dsp::AudioBlock<float>& audioblock);
    void processBlockNonlinear(juce::dsp::AudioBlock<float>& audioblock);


    // Generic function to get components of a specific type inside allComponents, e.g. getComponents<Resistance>()
    template <typename T>
    std::vector<std::shared_ptr<T>> getComponents() {
        std::vector<std::shared_ptr<T>> specificComponents;

        for (const auto& component : allComponents) {
            std::shared_ptr<T> specificComponent = std::dynamic_pointer_cast<T>(component);
            if (specificComponent) {
                specificComponents.push_back(specificComponent);
            }
        }
        return specificComponents;
    }

private:

    void categorizeComponents();
    void setupSystem();
    unsigned getNodeNbr();

    std::vector<std::string> split(const std::string& s, char delimiter);
    std::vector<std::shared_ptr<Component>> createComponentListFromTxt(const std::string& filename);
    std::shared_ptr<Component> createComponent(const std::string& netlistLine, unsigned idx);
};
