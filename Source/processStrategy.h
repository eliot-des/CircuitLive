/*
  ==============================================================================

    processStrategy.h
    Created: 21 Jun 2024 11:09:34am
    Author:  eliot

  ==============================================================================
*/
#pragma once
#include "component.h"

#include <JuceHeader.h>

class System;



class ProcessStrategy {
public:
    virtual void processBlock(System& system, juce::dsp::AudioBlock<float>& audioBlock) = 0;
    virtual ~ProcessStrategy() = default;
};

class LinearProcessStrategy : public ProcessStrategy {
public:
    void processBlock(System& system, juce::dsp::AudioBlock<float>& audioBlock) override;
};


class NonLinearProcessStrategy : public ProcessStrategy {
public:
    void processBlock(System& system, juce::dsp::AudioBlock<float>& audioBlock) override;
};

