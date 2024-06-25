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

    //243, 239, 234 or 241, 239, 243
    auto mygrey = juce::Colour::fromRGB(243, 239, 234);
    

    //get lookandfeel colors of the light theme and not the default dark theme
    LookAndFeel::setColourScheme(LookAndFeel_V4::getLightColourScheme());

    setColour(juce::ResizableWindow::backgroundColourId, mygrey);

    //change default color of the label
    setColour(juce::Label::textColourId, juce::Colours::darkgrey);
   
    //change default color of slider's textbox
    setColour(juce::Slider::textBoxHighlightColourId, juce::Colours::darkgrey);
    setColour(juce::Slider::textBoxTextColourId, juce::Colours::darkgrey);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    
    //change default slider colors:
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::grey.brighter(0.5f));
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkorange);

    setColour(juce::Slider::backgroundColourId, juce::Colours::grey.brighter(0.5f));
    setColour(juce::Slider::trackColourId, juce::Colours::darkorange);
    setColour(juce::Slider::thumbColourId, juce::Colours::darkorange);

    //group component colors
    setColour(juce::GroupComponent::textColourId, juce::Colours::grey);
    setColour(juce::GroupComponent::outlineColourId, juce::Colours::grey.brighter(1.5f));

    //change default color of the text editor
    setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
    setColour(juce::TextEditor::textColourId, juce::Colours::darkgrey);

    LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName("Carlito");
    
}

void LookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style,
    juce::Slider& slider)
{
    if (slider.isBar())
    {
        g.setColour(slider.findColour(juce::Slider::trackColourId));
        g.fillRect(slider.isHorizontal() ? juce::Rectangle<float>(static_cast<float> (x), (float)y + 0.5f, sliderPos - (float)x, (float)height - 1.0f)
            : juce::Rectangle<float>((float)x + 0.5f, sliderPos, (float)width - 1.0f, (float)y + ((float)height - sliderPos)));

        drawLinearSliderOutline(g, x, y, width, height, style, slider);
    }
    else
    {
        auto isTwoVal = (style == juce::Slider::SliderStyle::TwoValueVertical || style == juce::Slider::SliderStyle::TwoValueHorizontal);
        auto isThreeVal = (style == juce::Slider::SliderStyle::ThreeValueVertical || style == juce::Slider::SliderStyle::ThreeValueHorizontal);

        auto trackWidth = juce::jmin(6.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);

        juce::Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
            slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

        juce::Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
            slider.isHorizontal() ? startPoint.y : (float)y);

        juce::Path backgroundTrack;
        backgroundTrack.startNewSubPath(startPoint);
        backgroundTrack.lineTo(endPoint);
        g.setColour(slider.findColour(juce::Slider::backgroundColourId));
        g.strokePath(backgroundTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

        juce::Path valueTrack;
        juce::Point<float> minPoint, maxPoint, thumbPoint;

        if (isTwoVal || isThreeVal)
        {
            minPoint = { slider.isHorizontal() ? minSliderPos : (float)width * 0.5f,
                         slider.isHorizontal() ? (float)height * 0.5f : minSliderPos };

            if (isThreeVal)
                thumbPoint = { slider.isHorizontal() ? sliderPos : (float)width * 0.5f,
                               slider.isHorizontal() ? (float)height * 0.5f : sliderPos };

            maxPoint = { slider.isHorizontal() ? maxSliderPos : (float)width * 0.5f,
                         slider.isHorizontal() ? (float)height * 0.5f : maxSliderPos };
        }
        else
        {
            auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
            auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;
           
            minPoint = startPoint;
            maxPoint = { kx, ky };
        }

        auto thumbWidth1 = getSliderThumbRadius(slider)*1.33f;
        auto thumbWidth2 = getSliderThumbRadius(slider)*0.66f;

        valueTrack.startNewSubPath(minPoint);
        valueTrack.lineTo(isThreeVal ? thumbPoint : maxPoint);
        g.setColour(slider.findColour(juce::Slider::trackColourId));
        g.strokePath(valueTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

        if (!isTwoVal)
        {
            //g.setColour(slider.findColour(juce::Slider::thumbColourId));
            g.setColour(juce::Colours::white);
            g.fillEllipse(juce::Rectangle<float>(static_cast<float> (thumbWidth1), static_cast<float> (thumbWidth1)).withCentre(isThreeVal ? thumbPoint : maxPoint));

            g.setColour(juce::Colours::darkgrey);
            g.fillEllipse(juce::Rectangle<float>(static_cast<float> (thumbWidth2), static_cast<float> (thumbWidth2)).withCentre(isThreeVal ? thumbPoint : maxPoint));

        }

        if (isTwoVal || isThreeVal)
        {
            auto sr = juce::jmin(trackWidth, (slider.isHorizontal() ? (float)height : (float)width) * 0.4f);
            auto pointerColour = slider.findColour(juce::Slider::thumbColourId);

            if (slider.isHorizontal())
            {
                drawPointer(g, minSliderPos - sr,
                    juce::jmax(0.0f, (float)y + (float)height * 0.5f - trackWidth * 2.0f),
                    trackWidth * 2.0f, pointerColour, 2);

                drawPointer(g, maxSliderPos - trackWidth,
                    juce::jmin((float)(y + height) - trackWidth * 2.0f, (float)y + (float)height * 0.5f),
                    trackWidth * 2.0f, pointerColour, 4);
            }
            else
            {
                drawPointer(g, juce::jmax(0.0f, (float)x + (float)width * 0.5f - trackWidth * 2.0f),
                    minSliderPos - trackWidth,
                    trackWidth * 2.0f, pointerColour, 1);

                drawPointer(g, juce::jmin((float)(x + width) - trackWidth * 2.0f, (float)x + (float)width * 0.5f), maxSliderPos - sr,
                    trackWidth * 2.0f, pointerColour, 3);
            }
        }

        if (slider.isBar())
            drawLinearSliderOutline(g, x, y, width, height, style, slider);
    }
}


void LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).reduced(7);



    auto outline = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
    auto fill = slider.findColour(juce::Slider::rotarySliderFillColourId);
    auto background = slider.findColour(juce::ResizableWindow::backgroundColourId);

    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;            //radius of the exterior dial path
    auto innerRadius = radius * 0.72f;                                                 //radius of the dial
    auto rx = bounds.getCentreX() - innerRadius;                                       //x position of the dial
    auto ry = bounds.getCentreY() - innerRadius;                                       //y position of the dial
    auto rw = innerRadius * 2.0f;                                                      //width of the dial

    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = radius * 0.08f;
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

        g.setColour(fill);
        g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Create the outer ellipse path
    juce::Path outerEllipsePath;
    outerEllipsePath.addEllipse(rx, ry, rw, rw);
    
    // Create the gradient for the inner circle
    auto gradient = juce::ColourGradient(
        background.brighter(2.f), bounds.getCentreX(), bounds.getCentreY() - innerRadius,
        background, bounds.getCentreX(), bounds.getCentreY() + innerRadius, false);

    // Apply the drop shadow to the outline path
    juce::DropShadow shadow(juce::Colours::black.withAlpha(0.1f), 20, { 0, 2 });
    shadow.drawForPath(g, outerEllipsePath);
    g.setGradientFill(gradient);
    g.fillPath(outerEllipsePath);
    // Draw the outline of the ellipse
    g.setColour(juce::Colours::white);
    g.strokePath(outerEllipsePath, juce::PathStrokeType(3.0f));

    // Dial pointer
    juce::Path dialPointer;
    auto pointerLength = innerRadius * 0.5f;
    auto pointerThickness = innerRadius * 0.15f;
    dialPointer.addRoundedRectangle(-pointerThickness * 0.5f, -innerRadius * 0.85f, pointerThickness, pointerLength, pointerThickness * 0.5f);

    dialPointer.applyTransform(juce::AffineTransform::rotation(toAngle).translated(bounds.getCentreX(), bounds.getCentreY()));

    // Pointer
    g.setColour(juce::Colours::darkgrey);
    g.fillPath(dialPointer);
}

juce::Font LookAndFeel::getLabelFont(juce::Label& label)
{
    return juce::Font(20.0f);
}
