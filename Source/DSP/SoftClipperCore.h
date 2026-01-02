#pragma once

#include <juce_dsp/juce_dsp.h>
#include "../Utils/Parameters.h"

namespace Ignis
{

//==============================================================================
// Main Soft Clipper DSP Engine
//==============================================================================
class SoftClipperCore
{
public:
    SoftClipperCore();
    ~SoftClipperCore() = default;

    void prepare(double newSampleRate, int maxSamplesPerBlock);
    void reset();
    void process(juce::AudioBuffer<float>& buffer,
                float inputGainVal,
                float driveVal,
                float thresholdVal,
                float ceilingVal,
                float mixVal,
                float outputGainVal,
                int clipModeVal,
                bool linkChannelsVal);

    //==============================================================================
    // Clipping Algorithms
    //==============================================================================
    float cubicSoftClip(float x) const;
    float tanhSoftClip(float x) const;
    float hardClip(float x) const;
    float processClipper(float input, int mode) const;
    float applyCeiling(float sample) const;

    //==============================================================================
    // Metering
    //==============================================================================
    std::array<std::atomic<float>, 2> inputLevels = { 0.0f, 0.0f };
    std::array<std::atomic<float>, 2> outputLevels = { 0.0f, 0.0f };
    std::atomic<bool> isClipping = { false };

private:
    double sampleRate = 44100.0;

    // DC blocker state
    struct DCBlocker
    {
        float x1 = 0.0f;  // Previous input
        float y1 = 0.0f;  // Previous output
        float R = 0.995f;  // Feedback coefficient (~10Hz highpass)
    };

    std::array<DCBlocker, 2> dcBlockers{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoftClipperCore)
};

} // namespace Ignis
