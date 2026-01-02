#include "PresetManager.h"
#include "Parameters.h"

namespace Ignis
{

PresetManager::PresetManager(juce::AudioProcessorValueTreeState& apvts)
    : valueTreeState(apvts)
{
    createFactoryPresets();
}

//==============================================================================
void PresetManager::createFactoryPresets()
{
    // Helper to create a preset with specific parameter values
    auto createPreset = [this](const juce::String& name, const juce::String& category,
                               float inputGain, float drive, float threshold,
                               float ceiling, float mix, float outputGain,
                               int clipMode, int oversampling, bool linkChannels)
    {
        auto xml = std::make_unique<juce::XmlElement>("PARAMETERS");

        xml->setAttribute(ParamIDs::inputGain, inputGain);
        xml->setAttribute(ParamIDs::drive, drive);
        xml->setAttribute(ParamIDs::threshold, threshold);
        xml->setAttribute(ParamIDs::ceiling, ceiling);
        xml->setAttribute(ParamIDs::mix, mix);
        xml->setAttribute(ParamIDs::outputGain, outputGain);
        xml->setAttribute(ParamIDs::clipMode, clipMode);
        xml->setAttribute(ParamIDs::oversampling, oversampling);
        xml->setAttribute(ParamIDs::linkChannels, linkChannels ? 1 : 0);

        factoryPresets.push_back({ name, category, std::move(xml) });
    };

    //==============================================================================
    // Factory Presets for Ignis Soft Clipper
    // Format: name, category, inputGain, drive, threshold, ceiling, mix, outputGain,
    //         clipMode (0=Cubic,1=Tanh,2=Hard), oversampling (0=1x,1=2x,2=4x), linkChannels

    // 1. Init - Default neutral settings
    createPreset("Init", "Default",
        0.0f,      // Input gain
        50.0f,     // Drive
        0.0f,      // Threshold
        -0.1f,     // Ceiling
        100.0f,    // Mix
        0.0f,      // Output gain
        0,         // Cubic mode
        0,         // 1x oversample
        true);     // Link channels

    // 2. Subtle Warmth - Gentle saturation for adding warmth
    createPreset("Subtle Warmth", "Saturation",
        0.0f,      // No input gain
        25.0f,     // Light drive
        0.0f,      // No threshold reduction
        -0.5f,     // Slight ceiling
        100.0f,    // Fully wet
        0.0f,      // No output compensation
        0,         // Cubic (smoothest)
        0,         // 1x oversample
        true);     // Linked

    // 3. Tube Boost - Warm tube-like saturation
    createPreset("Tube Boost", "Saturation",
        3.0f,      // Slight input boost
        60.0f,     // Moderate-high drive
        -3.0f,     // Start clipping early
        -1.0f,     // Ceiling
        100.0f,    // Fully wet
        2.0f,      // Make up gain
        1,         // Tanh (tube-like curve)
        1,         // 2x oversample
        true);     // Linked

    // 4. Tape Saturation - Smooth tape-like compression
    createPreset("Tape Saturation", "Saturation",
        0.0f,      // Unity input
        50.0f,     // Medium drive
        -6.0f,     // Soft threshold
        -2.0f,     // Ceiling
        100.0f,    // Full effect
        1.5f,      // Slight boost
        0,         // Cubic (tape-like)
        1,         // 2x oversample for quality
        true);     // Linked

    // 5. Glue - Parallel compression-style blending
    createPreset("Glue", "Mix Bus",
        0.0f,      // Unity
        40.0f,     // Moderate drive
        -12.0f,    // Low threshold for soft clipping
        -1.0f,     // Ceiling
        40.0f,     // 40% mix for parallel
        0.0f,      // Unity output
        0,         // Cubic
        0,         // 1x
        true);     // Linked stereo

    // 6. Aggressive - Hard clipping for distortion
    createPreset("Aggressive", "Distortion",
        6.0f,      // Input boost
        90.0f,     // High drive
        -3.0f,     // Threshold
        -6.0f,     // Ceiling
        100.0f,    // Fully wet
        -2.0f,     // Reduce output slightly
        2,         // Hard clip mode
        1,         // 2x oversample
        true);     // Linked

    // 7. Bass Enhancer - Adds harmonics to bass
    createPreset("Bass Enhancer", "Bass",
        0.0f,      // Unity input
        70.0f,     // High drive for harmonics
        0.0f,      // No threshold reduction
        -3.0f,     // Ceiling to control peaks
        100.0f,    // Full effect
        3.0f,      // Boost output
        0,         // Cubic (musical harmonics)
        0,         // 1x (low latency)
        true);     // Linked

    // 8. Drum Punch - Adds punch to drums
    createPreset("Drum Punch", "Drums",
        4.0f,      // Input boost
        65.0f,     // Strong drive
        -3.0f,     // Soft clipping
        -1.5f,     // Ceiling
        100.0f,    // Full wet
        0.0f,      // Unity output
        0,         // Cubic (fast transients)
        0,         // 1x (lowest latency)
        true);     // Linked

    // 9. Master Limiter - Ceiling limiting for mastering
    createPreset("Master Limiter", "Mastering",
        -2.0f,     // Slight input reduction
        20.0f,     // Low drive for limiting
        -1.0f,     // High threshold
        -0.3f,     // Ceiling (mastering level)
        100.0f,    // Full wet
        2.0f,      // Make up gain
        0,         // Cubic (transparent limiting)
        2,         // 4x oversample (highest quality)
        true);     // Linked

    // 10. Lo-Fi - Creative lo-fi effect
    createPreset("Lo-Fi", "Creative",
        0.0f,      // Unity
        100.0f,    // Maximum drive
        0.0f,      // No threshold (full distortion)
        -12.0f,    // Low ceiling (squash)
        100.0f,    // Full wet
        -6.0f,     // Reduce output
        2,         // Hard clip (aggressive)
        0,         // 1x (add aliasing for grit)
        true);     // Linked
}

//==============================================================================
void PresetManager::loadFactoryPreset(int index)
{
    if (juce::isPositiveAndBelow(index, factoryPresets.size()))
    {
        const auto& preset = factoryPresets[index];
        loadPresetFromXml(*preset.state);
        currentPresetName = preset.name;
        currentPresetIndex = index;
        presetModified = false;
    }
}

//==============================================================================
juce::StringArray PresetManager::getFactoryPresetNames() const
{
    juce::StringArray names;
    for (const auto& preset : factoryPresets)
        names.add(preset.name);
    return names;
}

//==============================================================================
juce::StringArray PresetManager::getAllPresetNames() const
{
    return getFactoryPresetNames();
}

//==============================================================================
void PresetManager::loadPreset(const juce::String& presetName)
{
    // Search in factory presets
    for (size_t i = 0; i < factoryPresets.size(); ++i)
    {
        if (factoryPresets[i].name == presetName)
        {
            loadFactoryPreset(static_cast<int>(i));
            return;
        }
    }

    // TODO: Load from user presets directory
    juce::ignoreUnused(presetName);
}

//==============================================================================
void PresetManager::savePreset(const juce::String& presetName)
{
    juce::ignoreUnused(presetName);
    // TODO: Implement user preset saving to disk
    jassertfalse; // Not implemented yet
}

//==============================================================================
void PresetManager::deletePreset(const juce::String& presetName)
{
    juce::ignoreUnused(presetName);
    // TODO: Implement user preset deletion
    jassertfalse; // Not implemented yet
}

//==============================================================================
void PresetManager::initializeDefaultPreset()
{
    loadFactoryPreset(0); // Load "Init" preset
}

//==============================================================================
void PresetManager::loadPresetFromXml(const juce::XmlElement& xml)
{
    // Helper lambda to set parameter if it exists in XML
    auto setParam = [this](const juce::String& paramID)
    {
        if (xml.hasAttribute(paramID))
        {
            auto* param = valueTreeState.getParameter(paramID);
            if (param != nullptr)
            {
                auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param);
                if (rangedParam != nullptr)
                {
                    float value = xml.getDoubleAttribute(paramID);
                    valueTreeState.getParameter(paramID)->setValueNotifyingHost(value);
                }
            }
        }
    };

    // Load all parameters from preset
    setParam(ParamIDs::inputGain);
    setParam(ParamIDs::drive);
    setParam(ParamIDs::threshold);
    setParam(ParamIDs::ceiling);
    setParam(ParamIDs::mix);
    setParam(ParamIDs::outputGain);
    setParam(ParamIDs::clipMode);
    setParam(ParamIDs::oversampling);
    setParam(ParamIDs::linkChannels);
}

} // namespace Ignis
