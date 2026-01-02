#include "SoftClipperCore.h"
#include <cmath>

namespace Ignis
{

//==============================================================================
SoftClipperCore::SoftClipperCore()
{
    // Initialize atomic values
    for (auto& level : inputLevels)
        level.store(0.0f);
    for (auto& level : outputLevels)
        level.store(0.0f);
}

//==============================================================================
void SoftClipperCore::prepare(double newSampleRate, int maxSamplesPerBlock)
{
    sampleRate = newSampleRate;

    // Reset DC blockers
    for (auto& dc : dcBlockers)
    {
        dc.x1 = 0.0f;
        dc.y1 = 0.0f;
    }
}

//==============================================================================
void SoftClipperCore::reset()
{
    for (auto& dc : dcBlockers)
    {
        dc.x1 = 0.0f;
        dc.y1 = 0.0f;
    }

    for (auto& level : inputLevels)
        level.store(0.0f);
    for (auto& level : outputLevels)
        level.store(0.0f);
    isClipping.store(false);
}

//==============================================================================
float SoftClipperCore::cubicSoftClip(float x) const
{
    // Cubic soft clipping for |x| < 1
    // y = 1.5x - 0.5x³
    if (std::abs(x) < 1.0f)
    {
        float x2 = x * x;
        return x * (1.5f - 0.5f * x2);
    }

    // Hard clip beyond threshold
    return std::copysign(1.0f, x);
}

//==============================================================================
float SoftClipperCore::tanhSoftClip(float x) const
{
    // Smooth hyperbolic tangent saturation
    return std::tanh(x * 1.5f) / 1.5f;
}

//==============================================================================
float SoftClipperCore::hardClip(float x) const
{
    // Aggressive digital hard clipping
    return juce::jlimit(-1.0f, 1.0f, x);
}

//==============================================================================
float SoftClipperCore::processClipper(float input, int mode) const
{
    switch (mode)
    {
        case 0:  // Cubic
            return cubicSoftClip(input);
        case 1:  // Tanh
            return tanhSoftClip(input);
        case 2:  // Hard
            return hardClip(input);
        default:
            return input;
    }
}

//==============================================================================
float SoftClipperCore::applyCeiling(float sample) const
{
    const float ceilingLinear = juce::Decibels::decibelsToGain(-0.1f); // Default ceiling

    if (std::abs(sample) > ceilingLinear)
    {
        // Soft knee approach to ceiling
        const float excess = std::abs(sample) - ceilingLinear;
        const float sign = (sample > 0) ? 1.0f : -1.0f;
        const float knee = 0.1f;  // Soft transition
        return sign * (ceilingLinear + excess * knee);
    }

    return sample;
}

//==============================================================================
void SoftClipperCore::process(juce::AudioBuffer<float>& buffer,
                               float inputGainVal,
                               float driveVal,
                               float thresholdVal,
                               float ceilingVal,
                               float mixVal,
                               float outputGainVal,
                               int clipModeVal,
                               bool linkChannelsVal)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // Convert parameters to linear scale
    const float inputGainLinear = juce::Decibels::decibelsToGain(inputGainVal);
    const float driveMultiplier = 1.0f + (driveVal / 100.0f) * 4.0f; // 1x to 5x
    const float ceilingLinear = juce::Decibels::decibelsToGain(ceilingVal);
    const float outputGainLinear = juce::Decibels::decibelsToGain(outputGainVal);
    const float wetMix = mixVal / 100.0f;
    const float dryMix = 1.0f - wetMix;

    // Store dry signal for wet/dry mixing
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);

    // Apply input gain
    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);
        for (int i = 0; i < numSamples; ++i)
            channelData[i] *= inputGainLinear;
    }

    // Process each channel
    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);
        DCBlocker& dc = dcBlockers[ch];

        // Track input level for metering
        float maxInputLevel = 0.0f;

        for (int i = 0; i < numSamples; ++i)
        {
            float input = channelData[i];
            maxInputLevel = juce::jmax(maxInputLevel, std::abs(input));

            // Apply drive
            float driven = input * driveMultiplier;

            // Apply waveshaping
            float clipped = processClipper(driven, clipModeVal);

            // Apply DC blocking
            float y0 = clipped - dc.x1 + dc.R * dc.y1;
            dc.x1 = clipped;
            dc.y1 = y0;

            // Apply ceiling limiter
            float limited = applyCeiling(y0);

            // Drive compensation (prevent excessive gain)
            const float compensation = 1.0f / (1.0f + driveVal * 0.005f);
            limited *= compensation;

            channelData[i] = limited;
        }

        // Update input level meter
        inputLevels[ch].store(maxInputLevel);
    }

    // Linked channel processing (peak detection across all channels)
    if (linkChannelsVal)
    {
        float maxOutputLevel = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
        {
            const float* channelData = buffer.getReadPointer(ch);
            for (int i = 0; i < numSamples; ++i)
                maxOutputLevel = juce::jmax(maxOutputLevel, std::abs(channelData[i]));
        }

        // Check if clipping
        if (maxOutputLevel > ceilingLinear)
            isClipping.store(true);
        else
            isClipping.store(false);
    }

    // Mix wet and dry
    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* wet = buffer.getWritePointer(ch);
        const float* dry = dryBuffer.getReadPointer(ch);

        for (int i = 0; i < numSamples; ++i)
        {
            wet[i] = dry[i] * dryMix + wet[i] * wetMix;
        }
    }

    // Apply output gain and update output meters
    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);
        float maxOutputLevel = 0.0f;

        for (int i = 0; i < numSamples; ++i)
        {
            channelData[i] *= outputGainLinear;
            maxOutputLevel = juce::jmax(maxOutputLevel, std::abs(channelData[i]));
        }

        outputLevels[ch].store(maxOutputLevel);
    }
}

} // namespace Ignis
