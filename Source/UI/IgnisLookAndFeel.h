#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace Ignis
{

class IgnisLookAndFeel : public juce::LookAndFeel_V4
{
public:
    IgnisLookAndFeel();
    ~IgnisLookAndFeel() override = default;

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IgnisLookAndFeel)
};

} // namespace Ignis
