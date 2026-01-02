#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace Ignis
{

//==============================================================================
// Clip Mode Types
//==============================================================================
enum class ClipMode
{
    Cubic = 0,    // y = 1.5x - 0.5x³ (Fruity Soft Clipper style)
    Tanh,        // Hyperbolic tangent (smooth saturation)
    Hard         // Digital hard clip (aggressive)
};

//==============================================================================
// Parameter IDs
//==============================================================================
namespace ParamIDs
{
    inline const juce::String inputGain { "inputGain" };
    inline const juce::String drive { "drive" };
    inline const juce::String threshold { "threshold" };
    inline const juce::String ceiling { "ceiling" };
    inline const juce::String mix { "mix" };
    inline const juce::String outputGain { "outputGain" };
    inline const juce::String clipMode { "clipMode" };
    inline const juce::String oversampling { "oversampling" };
    inline const juce::String linkChannels { "linkChannels" };
}

//==============================================================================
// Default Values
//==============================================================================
namespace Defaults
{
    constexpr float inputGain = 0.0f;       // dB
    constexpr float drive = 50.0f;          // % (0-100)
    constexpr float threshold = 0.0f;       // dB
    constexpr float ceiling = -0.1f;        // dB
    constexpr float mix = 100.0f;           // % (0-100)
    constexpr float outputGain = 0.0f;      // dB
    constexpr int clipMode = 0;             // Cubic
    constexpr int oversampling = 0;         // 1x
    constexpr bool linkChannels = true;
}

//==============================================================================
// Parameter Ranges
//==============================================================================
namespace Ranges
{
    // Gain ranges
    constexpr float inputGainMin = -24.0f;
    constexpr float inputGainMax = 24.0f;
    constexpr float outputGainMin = -24.0f;
    constexpr float outputGainMax = 12.0f;

    // Clipping thresholds
    constexpr float thresholdMin = -60.0f;
    constexpr float thresholdMax = 0.0f;
    constexpr float ceilingMin = -20.0f;
    constexpr float ceilingMax = 0.0f;

    // Percentages
    constexpr float driveMin = 0.0f;
    constexpr float driveMax = 100.0f;
    constexpr float mixMin = 0.0f;
    constexpr float mixMax = 100.0f;
}

//==============================================================================
// Parameter Layout
//==============================================================================
inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Input Gain
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::inputGain, 1 },
        "Input Gain",
        juce::NormalisableRange<float>(Ranges::inputGainMin, Ranges::inputGainMax, 0.1f),
        Defaults::inputGain,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // Drive
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::drive, 1 },
        "Drive",
        juce::NormalisableRange<float>(Ranges::driveMin, Ranges::driveMax, 0.1f),
        Defaults::drive,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Threshold
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::threshold, 1 },
        "Threshold",
        juce::NormalisableRange<float>(Ranges::thresholdMin, Ranges::thresholdMax, 0.1f),
        Defaults::threshold,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // Ceiling
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::ceiling, 1 },
        "Ceiling",
        juce::NormalisableRange<float>(Ranges::ceilingMin, Ranges::ceilingMax, 0.1f),
        Defaults::ceiling,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // Mix
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::mix, 1 },
        "Mix",
        juce::NormalisableRange<float>(Ranges::mixMin, Ranges::mixMax, 0.1f),
        Defaults::mix,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Output Gain
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ ParamIDs::outputGain, 1 },
        "Output Gain",
        juce::NormalisableRange<float>(Ranges::outputGainMin, Ranges::outputGainMax, 0.1f),
        Defaults::outputGain,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // Clip Mode
    juce::StringArray clipModeChoices;
    clipModeChoices.add("Cubic");
    clipModeChoices.add("Tanh");
    clipModeChoices.add("Hard");
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ ParamIDs::clipMode, 1 },
        "Clip Mode",
        clipModeChoices,
        Defaults::clipMode));

    // Oversampling
    juce::StringArray oversampleChoices;
    oversampleChoices.add("1x");
    oversampleChoices.add("2x");
    oversampleChoices.add("4x");
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ ParamIDs::oversampling, 1 },
        "Oversampling",
        oversampleChoices,
        Defaults::oversampling));

    // Link Channels
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ ParamIDs::linkChannels, 1 },
        "Link Channels",
        Defaults::linkChannels));

    return { params.begin(), params.end() };
}

} // namespace Ignis
