/*
  ==============================================================================

    LookAndFeel.cpp
    Created: 24 Jun 2024 10:42:41am
    Author:  eliot

  ==============================================================================
*/

#include "LookAndFeel.h"

LookAndFeel::LookAndFeel()
{
    // Color for the text inside the popup
    setColour(juce::TooltipWindow::textColourId, juce::Colours::lightgrey);

    // Colors for the popup bubble
    setColour(juce::BubbleComponent::backgroundColourId, juce::Colours::black);
    setColour(juce::BubbleComponent::outlineColourId, juce::Colours::lightgrey);

    auto dropShadow = juce::DropShadow(juce::Colours::black.withAlpha(0.5f), 2, { 0, 1 });
    bubbleShadow.setShadowProperties(dropShadow);
}


void LookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style,
    juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
    auto rect = bounds.withWidth(std::ceil(sliderPos - bounds.getX()));
    auto cornerRadius = 3.0f;

    auto gradient = juce::ColourGradient(
        { 0, 112, 255 }, bounds.getX(), 0.0f,
        { 0, 255, 186 }, bounds.getRight(), 0.0f, false);

    juce::Path path;
    path.addRoundedRectangle(bounds, cornerRadius);

    g.saveState();
    g.reduceClipRegion(path);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(rect, cornerRadius);
    g.restoreState();
}

void LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);

    auto outline = slider.findColour(juce::Slider::rotarySliderOutlineColourId);

    auto gradient = juce::ColourGradient(
        { 0, 112, 255 }, bounds.getX(), 0.0f,
        { 0, 255, 186 }, bounds.getRight(), 0.0f, false);
    

    /*
    auto gradient = juce::ColourGradient(
        { 255, 0, 110 }, bounds.getX(), 0.0f,
        { 255, 220, 0 }, bounds.getRight(), 0.0f, false);
    
    
    */

    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = radius * 0.1f;
    auto arcRadius = radius - lineW * 0.5f;

    //background dial path
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryStartAngle,
        rotaryEndAngle,
        true);

    g.setColour(outline);
    g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    //front dial path
    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(),
            bounds.getCentreY(),
            arcRadius,
            arcRadius,
            0.0f,
            rotaryStartAngle,
            toAngle,
            true);

        g.setGradientFill(gradient);
        g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    juce::Path knobIndent;
    auto pointerLength = radius * 0.5f;
    auto pointerThickness = radius * 0.1f;
    knobIndent.addRoundedRectangle(-pointerThickness * 0.5f, -radius*0.8f, pointerThickness, pointerLength, pointerThickness * 0.5f);

    knobIndent.applyTransform(juce::AffineTransform::rotation(toAngle).translated(bounds.getCentreX(), bounds.getCentreY()));

    // pointer
    g.setColour(juce::Colours::white);
    g.fillPath(knobIndent);

}


juce::Label* LookAndFeel::createSliderTextBox(juce::Slider& slider)
{
	auto* l = juce::LookAndFeel_V2::createSliderTextBox(slider);
    if (getCurrentColourScheme() == LookAndFeel_V4::getGreyColourScheme() && (slider.getSliderStyle() == juce::Slider::LinearBar
        || slider.getSliderStyle() == juce::Slider::LinearBarVertical))
    {
        l->setColour(juce::Label::textColourId, juce::Colours::pink.withAlpha(0.7f));
        l->setBounds(l->getBounds().reduced(20));
    }

    return l;
}




juce::Font LookAndFeel::getSliderPopupFont(juce::Slider&)
{
    return juce::Font(11.0f);
}

int LookAndFeel::getSliderPopupPlacement(juce::Slider& slider)
{
    if (slider.isRotary()) {
        return juce::BubbleComponent::above;
    }
    else {
        return juce::BubbleComponent::below;
    }
}

void LookAndFeel::drawBubble(
    juce::Graphics& g,
    juce::BubbleComponent& comp,
    const juce::Point<float>& tip,
    const juce::Rectangle<float>& body)
{
    // Draw tip above or below the bubble?
    float adjustedY = tip.y + ((tip.y > body.getBottom()) ? -6.0f : 6.0f);

    juce::Path path;
    path.addBubble(
        body.reduced(0.5f),
        body.getUnion(juce::Rectangle<float>(tip.x, tip.y, 1.0f, 1.0f)),
        { tip.x, adjustedY },
        2.0f,
        juce::jmin(15.0f, body.getWidth() * 0.2f, body.getHeight() * 0.2f));

    g.setColour(comp.findColour(juce::BubbleComponent::backgroundColourId));
    g.fillPath(path);

    g.setColour(comp.findColour(juce::BubbleComponent::outlineColourId));
    g.strokePath(path, juce::PathStrokeType(1.0f));
}

void LookAndFeel::setComponentEffectForBubbleComponent(juce::BubbleComponent& bubbleComponent)
{
    bubbleComponent.setComponentEffect(&bubbleShadow);
}
