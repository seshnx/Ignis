#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace Ignis
{

class StereoMeter : public juce::Component, public juce::Timer
{
public:
    StereoMeter();
    ~StereoMeter() override;

    void setLevels(float leftLevel, float rightLevel);

    void paint(juce::Graphics& g) override;
    void resized() override;

    void timerCallback() override;

private:
    void drawMeter(juce::Graphics& g, juce::Rectangle<float> bounds, float level);
    float targetLeftLevel = 0.0f;
    float targetRightLevel = 0.0f;
    float currentLeftLevel = 0.0f;
    float currentRightLevel = 0.0f;

    // LED segments (0-20 for -inf to 0 dB)
    int numSegments = 20;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereoMeter)
};

} // namespace Ignis
