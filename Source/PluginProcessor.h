#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "DSP/SoftClipperCore.h"
#include "DSP/AntiAliasingFilter.h"
#include "Utils/Parameters.h"
#include "Utils/PresetManager.h"

//==============================================================================
/**
 * SeshNx Ignis - Soft Clipper
 *
 * A soft clipping saturation plugin featuring:
 * - Cubic soft clipping (Fruity Soft Clipper style)
 * - Tanh and hard clipping modes
 * - Drive, threshold, and ceiling controls
 * - Wet/dry mix for parallel processing
 * - Input/output metering
 * - Clipping indicator
 */
class IgnisAudioProcessor : public juce::AudioProcessor
{
public:
    IgnisAudioProcessor();
    ~IgnisAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameter access
    juce::AudioProcessorValueTreeState& getParameters() { return parameters; }
    Ignis::PresetManager& getPresetManager() { return presetManager; }

    // Metering data
    float getInputLevel(int channel) const { return softClipper.inputLevels[channel].load(); }
    float getOutputLevel(int channel) const { return softClipper.outputLevels[channel].load(); }
    bool getIsClipping() const { return softClipper.isClipping.load(); }

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState parameters;
    Ignis::PresetManager presetManager;

    // Cached parameter pointers
    std::atomic<float>* inputGainParam = nullptr;
    std::atomic<float>* driveParam = nullptr;
    std::atomic<float>* thresholdParam = nullptr;
    std::atomic<float>* ceilingParam = nullptr;
    std::atomic<float>* mixParam = nullptr;
    std::atomic<float>* outputGainParam = nullptr;
    std::atomic<float>* clipModeParam = nullptr;
    std::atomic<float>* oversamplingParam = nullptr;
    std::atomic<float>* linkChannelsParam = nullptr;

    // DSP components
    Ignis::SoftClipperCore softClipper;
    Ignis::AntiAliasingFilter antiAliasing;

    // Dry buffer for wet/dry mixing
    juce::AudioBuffer<float> dryBuffer;

    // Smoothed parameters
    juce::SmoothedValue<float> smoothedInputGain;
    juce::SmoothedValue<float> smoothedDrive;
    juce::SmoothedValue<float> smoothedThreshold;
    juce::SmoothedValue<float> smoothedCeiling;
    juce::SmoothedValue<float> smoothedMix;
    juce::SmoothedValue<float> smoothedOutputGain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IgnisAudioProcessor)
};
