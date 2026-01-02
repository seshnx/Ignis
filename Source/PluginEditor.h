#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "UI/IgnisLookAndFeel.h"
#include "UI/StereoMeter.h"
#include "UI/ClippingIndicator.h"

//==============================================================================
/**
 * SeshNx Ignis - Soft Clipper Editor
 *
 * Fire-themed soft clipper interface with:
 * - I/O level meters
 * - Rotary knobs for all parameters
 * - Clipping indicator LED
 * - Warm orange/red color scheme
 */
class IgnisAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    IgnisAudioProcessorEditor(IgnisAudioProcessor&);
    ~IgnisAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    IgnisAudioProcessor& audioProcessor;
    IgnisLookAndFeel lookAndFeel;

    // Meters
    StereoMeter inputMeter;
    StereoMeter outputMeter;
    ClippingIndicator clipIndicator;

    // Parameter controls
    juce::Slider inputGainSlider;
    juce::Slider driveSlider;
    juce::Slider thresholdSlider;
    juce::Slider ceilingSlider;
    juce::Slider mixSlider;
    juce::Slider outputGainSlider;

    juce::ComboBox clipModeBox;
    juce::ComboBox oversampleBox;
    juce::ToggleButton linkChannelsButton;

    // Labels
    juce::Label inputGainLabel;
    juce::Label driveLabel;
    juce::Label thresholdLabel;
    juce::Label ceilingLabel;
    juce::Label mixLabel;
    juce::Label outputGainLabel;
    juce::Label clipModeLabel;
    juce::Label oversampleLabel;

    // Parameter attachments (must be declared after controls)
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ceilingAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> clipModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> oversampleAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> linkChannelsAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IgnisAudioProcessorEditor)
};
