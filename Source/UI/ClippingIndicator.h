#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace Ignis
{

class ClippingIndicator : public juce::Component, public juce::Timer
{
public:
    ClippingIndicator();
    ~ClippingIndicator() override;

    void setActive(bool active);
    bool isActive() const { return isClipping; }

    void paint(juce::Graphics& g) override;
    void resized() override;

    void timerCallback() override;

private:
    bool targetClipping = false;
    bool isClipping = false;
    float currentAlpha = 0.0f;

    // Decay time for clip indicator (milliseconds)
    int clipDecayTime = 500;

    juce::Time lastClipTime;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClippingIndicator)
};

} // namespace Ignis
