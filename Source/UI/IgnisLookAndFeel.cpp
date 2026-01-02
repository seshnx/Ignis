#include "IgnisLookAndFeel.h"

namespace Ignis
{

//==============================================================================
IgnisLookAndFeel::IgnisLookAndFeel()
{
    // Fire color scheme
    const juce::Colour fireOrange = juce::Colour(0xFFFF6600);
    const juce::Colour deepRed = juce::Colour(0xFFCC3300);
    const juce::Colour darkBg = juce::Colour(0xFF1A0F0A);

    setColour(juce::Slider::rotarySliderFillColourId, fireOrange);
    setColour(juce::Slider::thumbColourId, fireOrange);
    setColour(juce::Slider::backgroundColourId, darkBg);
    setColour(juce::Slider::trackColourId, juce::Colours::darkgrey);
}

//==============================================================================
void IgnisLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPosProportional, float rotaryStartAngle,
                                         float rotaryEndAngle, juce::Slider& slider)
{
    auto radius = (float)juce::jmin(width, height) / 2.0f;
    auto centreX = (float)x + (float)width * 0.5f;
    auto centerY = (float)y + (float)height * 0.5f;
    auto rw = radius * 2.0f;

    // Draw background circle
    g.setColour(juce::Colours::darkgrey);
    g.drawEllipse(centreX - radius, centerY - radius, rw, rw, 2.0f);

    // Draw filled arc based on slider position
    juce::Path arc;
    arc.addCentredArc(centreX, centerY, radius - 4, radius - 4,
                      rotaryStartAngle, rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle),
                      true);

    g.setColour(findColour(juce::Slider::rotarySliderFillColourId));
    g.strokePath(arc, juce::PathStrokeType(4.0f));

    // Draw thumb
    auto thumbAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    auto thumbX = centreX + radius * std::cos(thumbAngle - juce::MathConstants<float>::halfPi);
    auto thumbY = centerY + radius * std::sin(thumbAngle - juce::MathConstants<float>::halfPi);

    g.setColour(findColour(juce::Slider::thumbColourId));
    g.fillEllipse(thumbX - 6, thumbY - 6, 12, 12);
}

} // namespace Ignis
