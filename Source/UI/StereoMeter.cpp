#include "StereoMeter.h"

namespace Ignis
{

//==============================================================================
StereoMeter::StereoMeter()
{
    // 30Hz update rate for smooth meter movement
    startTimerHz(30);
}

StereoMeter::~StereoMeter()
{
    stopTimer();
}

//==============================================================================
void StereoMeter::setLevels(float leftLevel, float rightLevel)
{
    targetLeftLevel = juce::jlimit(0.0f, 1.0f, leftLevel);
    targetRightLevel = juce::jlimit(0.0f, 1.0f, rightLevel);
}

//==============================================================================
void StereoMeter::timerCallback()
{
    // Smooth interpolation for meter movement
    float attack = 0.3f;
    float release = 0.1f;

    // Left channel
    if (targetLeftLevel > currentLeftLevel)
        currentLeftLevel += (targetLeftLevel - currentLeftLevel) * attack;
    else
        currentLeftLevel += (targetLeftLevel - currentLeftLevel) * release;

    // Right channel
    if (targetRightLevel > currentRightLevel)
        currentRightLevel += (targetRightLevel - currentRightLevel) * attack;
    else
        currentRightLevel += (targetRightLevel - currentRightLevel) * release;

    repaint();
}

//==============================================================================
void StereoMeter::resized()
{
}

//==============================================================================
void StereoMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    auto width = bounds.getWidth();
    auto height = bounds.getHeight();

    // Background
    g.fillAll(juce::Colours::black.withBrightness(0.2f));

    // Draw two meters (left and right)
    int meterWidth = (width - 20) / 2;
    int meterHeight = height - 20;

    auto leftMeterBounds = bounds.toFloat().withWidth(meterWidth)
                                               .withHeight(meterHeight)
                                               .translated(10, 10);
    auto rightMeterBounds = bounds.toFloat().withWidth(meterWidth)
                                                .withHeight(meterHeight)
                                                .translated(meterWidth + 10, 10);

    drawMeter(g, leftMeterBounds, currentLeftLevel);
    drawMeter(g, rightMeterBounds, currentRightLevel);
}

void StereoMeter::drawMeter(juce::Graphics& g, juce::Rectangle<float> bounds, float level)
{
    const int numLeds = 20;
    float ledHeight = bounds.getHeight() / numLeds;
    float ledWidth = bounds.getWidth() - 4.0f;
    float xPos = bounds.getX() + 2.0f;

    // Convert level (0-1) to number of lit LEDs
    // Using dB scale for better visual
    float dbLevel = juce::Decibels::gainToDecibels(level + 0.0001f);
    int numLit = juce::jmap(dbLevel, -60.0f, 0.0f, 0, numLeds);
    numLit = juce::jlimit(0, numLeds, numLit);

    for (int i = 0; i < numLeds; ++i)
    {
        float yPos = bounds.getBottom() - (i + 1) * ledHeight - 2.0f;

        // Color based on level
        juce::Colour ledColour;
        if (i < numLeds * 0.7)          // Bottom 70% - green
            ledColour = juce::Colour(0xFF00FF00);
        else if (i < numLeds * 0.9)     // 70-90% - yellow
            ledColour = juce::Colour(0xFFFFFF00);
        else                            // Top 10% - red
            ledColour = juce::Colour(0xFFFF0000);

        // Dim the color if not lit
        if (i >= numLit)
            ledColour = ledColour.withBrightness(0.2f);

        g.setColour(ledColour);
        g.fillRect(xPos, yPos, ledWidth, ledHeight - 1.0f);
    }
}

} // namespace Ignis
