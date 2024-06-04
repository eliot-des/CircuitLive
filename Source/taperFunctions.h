/*
  ==============================================================================

    taperFunctions.h
    Created: 28 May 2024 5:25:34pm
    Author:  eliot

  ==============================================================================
*/

#pragma once

#include <cmath>

class TaperFunctions
{
public:
    static double applyTaperA(double x)
    {
        return 0.03214285714 * std::pow(32.1111111111,    x) - 0.03214285714;
    }
    static double applyTaperB(double x)
    {
        return 0.03214285714 * std::pow(32.1111111111, -x+1) - 0.03214285714;
    }
};
