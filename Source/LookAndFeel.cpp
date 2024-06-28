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
    juce::Colour myGrey= juce::Colour::fromRGB(243, 239, 234);
    juce::Colour myBrightColor = juce::Colour::fromRGB(171, 105, 136);
    
    //LookAndFeel::setDefaultLookAndFeel(this);
    //LookAndFeel::setColourScheme(LookAndFeel_V4::getLightColourScheme());
    //setDefaultSansSerifTypefaceName(getCarlitoRegularFont().getTypefaceName());
    
    setColour(juce::ResizableWindow::backgroundColourId, myGrey);

    //change default color of the label
    setColour(juce::Label::textColourId, juce::Colours::darkgrey);
   
    //change default color of slider's textbox
    setColour(juce::Slider::textBoxHighlightColourId, juce::Colours::darkgrey);
    setColour(juce::Slider::textBoxTextColourId, juce::Colours::darkgrey);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
    setColour(juce::Slider::textBoxBackgroundColourId,juce::Colours::transparentBlack);

    //setColour of the text when it is edited in the textbox of the sliders
    //setColour(juce:)

    
    //change default slider colors:
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::grey.brighter(0.5f));
    setColour(juce::Slider::backgroundColourId, juce::Colours::grey.brighter(0.5f));
    setColour(juce::Slider::trackColourId, myBrightColor);
    setColour(juce::Slider::thumbColourId, myBrightColor);
    setColour(juce::Slider::rotarySliderFillColourId, myBrightColor);

    //group component colors
    setColour(juce::GroupComponent::textColourId, juce::Colours::grey);
    setColour(juce::GroupComponent::outlineColourId, juce::Colours::grey.brighter(2.0f));

    //change default color of the text editor
    setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
    setColour(juce::TextEditor::textColourId, juce::Colours::darkgrey);
    setColour(juce::TextButton::ColourIds::textColourOffId, myBrightColor);

    setColour(juce::CaretComponent::caretColourId, juce::Colours::darkgrey);
    //setColour(juce::Component);
    //change moving cursor color of the text editor:
}


const juce::Font LookAndFeel::getCarlitoRegularFont()
{
	static auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::CarlitoRegular_ttf, BinaryData::CarlitoRegular_ttfSize);
    return juce::Font(typeface);
}

const juce::Font LookAndFeel::getCarlitoBoldFont()
{
	static auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::CarlitoBold_ttf, BinaryData::CarlitoBold_ttfSize);
    return juce::Font(typeface);
}

/*
juce::Typeface::Ptr LookAndFeel::getTypefaceForFont(const juce::Font& font)
{
   return getCarlitoRegularFont().getTypeface();
}
*/



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

        auto thumbWidth1 = getSliderThumbRadius(slider)*1.0f;
        auto thumbWidth2 = getSliderThumbRadius(slider)*0.4f;

        valueTrack.startNewSubPath(minPoint);
        valueTrack.lineTo(isThreeVal ? thumbPoint : maxPoint);
        g.setColour(slider.findColour(juce::Slider::trackColourId));
        g.strokePath(valueTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

        if (!isTwoVal)
        {
            // Create the thumb path
            juce::Path thumbPath;
            thumbPath.addRoundedRectangle(juce::Rectangle<float>(static_cast<float> (thumbWidth1), static_cast<float> (thumbWidth1 * 2)).withCentre(isThreeVal ? thumbPoint : maxPoint), 5.0f);

            // Apply shadow to the thumb path
            juce::DropShadow shadow(juce::Colours::black.withAlpha(0.5f), 5, { 0, 2 });
            shadow.drawForPath(g, thumbPath);

            // Draw the thumb
            g.setColour(juce::Colours::white);
            g.fillPath(thumbPath);

            // Draw inner thumb path for additional styling
            juce::Path innerThumbPath;
            innerThumbPath.addRoundedRectangle(juce::Rectangle<float>(static_cast<float> (thumbWidth2), static_cast<float> (thumbWidth2 * 3)).withCentre(isThreeVal ? thumbPoint : maxPoint), 3.0f);
            g.setColour(juce::Colours::darkgrey);
            g.fillPath(innerThumbPath);

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
    juce::DropShadow shadow(juce::Colours::black.withAlpha(0.5f), 20, { 0, 2 });
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


void LookAndFeel::drawGroupComponentOutline(juce::Graphics& g, int width, int height,
    const juce::String& text, const juce::Justification& position,
    juce::GroupComponent& group)
{
    const float textH = 20.0f;
    const float indent = 3.0f;
    const float textEdgeGap = 4.0f;
    auto cs = 5.0f;


    //make the text font bold and bigger
    juce::Font f(textH, juce::Font::bold);

    juce::Path p;
    auto x = indent;
    auto y = f.getAscent() - 3.0f;
    auto w = juce::jmax(0.0f, (float)width - x * 2.0f);
    auto h = juce::jmax(0.0f, (float)height - y - indent);
    cs = juce::jmin(cs, w * 0.5f, h * 0.5f);
    auto cs2 = 2.0f * cs;

    auto textW = text.isEmpty() ? 0
        : juce::jlimit(0.0f,
            juce::jmax(0.0f, w - cs2 - textEdgeGap * 2),
            (float)f.getStringWidth(text) + textEdgeGap * 2.0f);
    auto textX = cs + textEdgeGap;

    if (position.testFlags(juce::Justification::horizontallyCentred))
        textX = cs + (w - cs2 - textW) * 0.5f;
    else if (position.testFlags(juce::Justification::right))
        textX = w - cs - textW - textEdgeGap;

    p.startNewSubPath(x + textX + textW, y);
    p.lineTo(x + w - cs, y);

    p.addArc(x + w - cs2, y, cs2, cs2, 0, juce::MathConstants<float>::halfPi);
    p.lineTo(x + w, y + h - cs);

    p.addArc(x + w - cs2, y + h - cs2, cs2, cs2, juce::MathConstants<float>::halfPi, juce::MathConstants<float>::pi);
    p.lineTo(x + cs, y + h);

    p.addArc(x, y + h - cs2, cs2, cs2, juce::MathConstants<float>::pi, juce::MathConstants<float>::pi * 1.5f);
    p.lineTo(x, y + cs);

    p.addArc(x, y, cs2, cs2, juce::MathConstants<float>::pi * 1.5f, juce::MathConstants<float>::twoPi);
    p.lineTo(x + textX, y);

    auto alpha = group.isEnabled() ? 1.0f : 0.5f;

    g.setColour(group.findColour(juce::GroupComponent::outlineColourId)
        .withMultipliedAlpha(alpha));

    g.strokePath(p, juce::PathStrokeType(2.0f));

    g.setColour(group.findColour(juce::GroupComponent::textColourId)
        .withMultipliedAlpha(alpha));
    g.setFont(f);
    g.drawText(text,
        juce::roundToInt(x + textX), 0,
        juce::roundToInt(textW),
        juce::roundToInt(textH),
        juce::Justification::centred, true);
}
