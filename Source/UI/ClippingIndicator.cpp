#include "ClippingIndicator.h"

namespace Ignis
{

//==============================================================================
ClippingIndicator::ClippingIndicator()
{
    // 60Hz update rate for responsive LED
    startTimerHz(60);
}

ClippingIndicator::~ClippingIndicator()
{
    stopTimer();
}

//==============================================================================
void ClippingIndicator::setActive(bool active)
{
    if (active && !targetClipping)
    {
        // Just started clipping
        lastClipTime = juce::Time::getCurrentTime();
    }

    targetClipping = active;
}

//==============================================================================
void ClippingIndicator::timerCallback()
{
    // Check if we should decay the clipping indicator
    if (!targetClipping && isClipping)
    {
        auto timeSinceClip = juce::RelativeTime::milliseconds(lastClipTime.getMillisecondCounter());

        if (timeSinceClip.inMilliseconds() > clipDecayTime)
        {
            // Decay the LED
            isClipping = false;
        }
    }
    else if (targetClipping)
    {
        isClipping = true;
        lastClipTime = juce::Time::getCurrentTime();
    }

    // Animate alpha for smooth transitions
    float targetAlpha = isClipping ? 1.0f : 0.0f;
    currentAlpha += (targetAlpha - currentAlpha) * 0.2f;

    if (std::abs(currentAlpha - targetAlpha) > 0.01f)
        repaint();
}

//==============================================================================
void ClippingIndicator::resized()
{
}

//==============================================================================
void ClippingIndicator::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background (dark when not clipping)
    auto bgColour = juce::Colours::black.withBrightness(0.3f);
    g.setColour(bgColour);
    g.fillRoundedRectangle(bounds, 5.0f);

    // If not active, just show dimmed text
    if (currentAlpha < 0.01f)
    {
        g.setColour(juce::Colours::darkred.withBrightness(0.3f));
        g.setFont(juce::Font(12.0f, juce::Font::bold));
        g.drawText("CLIP", bounds, juce::Justification::centred);
        return;
    }

    // Clip indicator is active - show glowing red LED
    auto ledColour = juce::Colour(0xFFFF0000).withAlpha(currentAlpha);

    // Glow effect (multiple layers with decreasing alpha)
    for (int i = 3; i > 0; --i)
    {
        float glowAlpha = currentAlpha / (float)i * 0.3f;
        g.setColour(ledColour.withAlpha(glowAlpha));
        g.fillRoundedRectangle(bounds.reduced((float)i * 2), 5.0f);
    }

    // Bright LED center
    g.setColour(ledColour);
    g.fillRoundedRectangle(bounds.reduced(3.0f), 5.0f);

    // White text on top
    g.setColour(juce::Colours::white.withAlpha(currentAlpha));
    g.setFont(juce::Font(12.0f, juce::Font::bold));
    g.drawText("CLIP", bounds, juce::Justification::centred);
}

} // namespace Ignis
