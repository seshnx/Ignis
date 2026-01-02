#include "PluginEditor.h"

//==============================================================================
IgnisAudioProcessorEditor::IgnisAudioProcessorEditor(IgnisAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set look and feel
    setLookAndFeel(&lookAndFeel);

    // Configure sliders as rotary knobs
    auto initSlider = [this](juce::Slider& slider) {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
        slider.setPopupDisplayEnabled(true);
        slider.setName("Parameter");
    };

    initSlider(inputGainSlider);
    initSlider(driveSlider);
    initSlider(thresholdSlider);
    initSlider(ceilingSlider);
    initSlider(mixSlider);
    initSlider(outputGainSlider);

    // Configure labels
    auto initLabel = [](juce::Label& label, const juce::String& text) {
        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::Font(11.0f));
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colours::white);
    };

    initLabel(inputGainLabel, "INPUT GAIN");
    initLabel(driveLabel, "DRIVE");
    initLabel(thresholdLabel, "THRESHOLD");
    initLabel(ceilingLabel, "CEILING");
    initLabel(mixLabel, "MIX");
    initLabel(outputGainLabel, "OUTPUT GAIN");
    initLabel(clipModeLabel, "CLIP MODE");
    initLabel(oversampleLabel, "OVERSAMPLE");

    // Configure combo boxes
    clipModeBox.addItem("Cubic", 1);
    clipModeBox.addItem("Tanh", 2);
    clipModeBox.addItem("Hard", 3);
    clipModeBox.setJustificationType(juce::Justification::centred);

    oversampleBox.addItem("1x", 1);
    oversampleBox.addItem("2x", 2);
    oversampleBox.addItem("4x", 3);
    oversampleBox.setJustificationType(juce::Justification::centred);

    // Configure toggle button
    linkChannelsButton.setButtonText("Link Channels");
    linkChannelsButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);

    // Add all components
    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);
    addAndMakeVisible(clipIndicator);

    addAndMakeVisible(inputGainSlider);
    addAndMakeVisible(driveSlider);
    addAndMakeVisible(thresholdSlider);
    addAndMakeVisible(ceilingSlider);
    addAndMakeVisible(mixSlider);
    addAndMakeVisible(outputGainSlider);

    addAndMakeVisible(clipModeBox);
    addAndMakeVisible(oversampleBox);
    addAndMakeVisible(linkChannelsButton);

    addAndMakeVisible(inputGainLabel);
    addAndMakeVisible(driveLabel);
    addAndMakeVisible(thresholdLabel);
    addAndMakeVisible(ceilingLabel);
    addAndMakeVisible(mixLabel);
    addAndMakeVisible(outputGainLabel);
    addAndMakeVisible(clipModeLabel);
    addAndMakeVisible(oversampleLabel);

    // Attach parameters to controls
    auto& params = audioProcessor.getParameters();

    inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        Ignis::ParamIDs::inputGain, inputGainSlider, params);
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        Ignis::ParamIDs::drive, driveSlider, params);
    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        Ignis::ParamIDs::threshold, thresholdSlider, params);
    ceilingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        Ignis::ParamIDs::ceiling, ceilingSlider, params);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        Ignis::ParamIDs::mix, mixSlider, params);
    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        Ignis::ParamIDs::outputGain, outputGainSlider, params);

    clipModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        Ignis::ParamIDs::clipMode, clipModeBox, params);
    oversampleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        Ignis::ParamIDs::oversampling, oversampleBox, params);
    linkChannelsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        Ignis::ParamIDs::linkChannels, linkChannelsButton, params);

    // Set plugin size
    setSize(600, 450);

    // Start timer for UI updates (30Hz)
    startTimerHz(30);
}

IgnisAudioProcessorEditor::~IgnisAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

//==============================================================================
void IgnisAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background with fire theme gradient
    auto bounds = getLocalBounds().toFloat();

    juce::ColourGradient gradient(
        juce::Colour(0xFF1A0A05),  // Dark brown/black
        bounds.getTopLeft(),
        juce::Colour(0xFF2D1105),  // Slightly lighter dark red
        bounds.getBottomRight());

    g.setGradientFill(gradient);
    g.fillRect(bounds);

    // Title area
    auto titleArea = bounds.removeFromTop(60);

    // Title
    g.setColour(juce::Colour(0xFFFF6600)); // Fire orange
    g.setFont(juce::Font(36.0f, juce::Font::bold));
    g.drawText("IGNIS", titleArea.removeFromTop(40).reduced(20, 0),
               juce::Justification::centred, true);

    // Subtitle
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.setFont(juce::Font(14.0f));
    g.drawText("Soft Clipper", titleArea.reduced(20, 0),
               juce::Justification::centred, true);

    // Border
    g.setColour(juce::Colour(0xFFFF6600).withAlpha(0.3f));
    g.drawRect(bounds, 2.0f);
}

void IgnisAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    auto contentArea = bounds.reduced(10);

    // Reserve space for title at top
    contentArea.removeFromTop(70);

    // Reserve space for clipping indicator at bottom
    auto bottomArea = contentArea.removeFromBottom(50);
    clipIndicator.setBounds(bottomArea.removeFromLeft(100).reduced(5));

    // Meter positions (left and right sides)
    int meterWidth = 60;
    auto leftMeterArea = contentArea.removeFromLeft(meterWidth);
    auto rightMeterArea = contentArea.removeFromRight(meterWidth);

    inputMeter.setBounds(leftMeterArea.withTrimmedTop(20).withTrimmedBottom(20));
    outputMeter.setBounds(rightMeterArea.withTrimmedTop(20).withTrimmedBottom(20));

    // Center controls area
    auto controlArea = contentArea.reduced(20, 10);

    // Layout: 3 rows of 2 knobs each
    int knobSize = 100;
    int spacing = 20;

    // Top row: Input Gain, Drive
    auto row1 = controlArea.removeFromTop(knobSize + 40);
    auto row1Left = row1.removeFromWidth(knobSize + 60);
    auto row1Right = row1;

    inputGainSlider.setBounds(row1Left.removeFromTop(knobSize));
    inputGainLabel.setBounds(row1Left.withTrimmedTop(knobSize).reduced(0, 5));

    driveSlider.setBounds(row1Right.removeFromTop(knobSize));
    driveLabel.setBounds(row1Right.withTrimmedTop(knobSize).reduced(0, 5));

    // Middle row: Threshold, Ceiling
    controlArea.removeFromTop(spacing);
    auto row2 = controlArea.removeFromTop(knobSize + 40);
    auto row2Left = row2.removeFromWidth(knobSize + 60);
    auto row2Right = row2;

    thresholdSlider.setBounds(row2Left.removeFromTop(knobSize));
    thresholdLabel.setBounds(row2Left.withTrimmedTop(knobSize).reduced(0, 5));

    ceilingSlider.setBounds(row2Right.removeFromTop(knobSize));
    ceilingLabel.setBounds(row2Right.withTrimmedTop(knobSize).reduced(0, 5));

    // Bottom row: Mix, Output Gain, plus controls
    controlArea.removeFromTop(spacing);
    auto row3 = controlArea.removeFromTop(knobSize + 40);
    auto row3Left = row3.removeFromWidth(knobSize + 60);
    auto row3Right = row3.removeFromWidth(knobSize + 60);

    mixSlider.setBounds(row3Left.removeFromTop(knobSize));
    mixLabel.setBounds(row3Left.withTrimmedTop(knobSize).reduced(0, 5));

    outputGainSlider.setBounds(row3Right.removeFromTop(knobSize));
    outputGainLabel.setBounds(row3Right.withTrimmedTop(knobSize).reduced(0, 5));

    // Extra controls (Clip Mode, Oversample, Link Channels)
    auto extraControls = row3;
    int comboWidth = 120;
    int comboHeight = 30;

    // Clip Mode
    auto clipModeArea = extraControls.removeFromLeft(comboWidth + 20);
    clipModeLabel.setBounds(clipModeArea.removeFromTop(20));
    clipModeBox.setBounds(clipModeArea.withHeight(comboHeight));

    // Oversample
    extraControls.removeFromLeft(10);
    auto oversampleArea = extraControls.removeFromLeft(comboWidth + 20);
    oversampleLabel.setBounds(oversampleArea.removeFromTop(20));
    oversampleBox.setBounds(oversampleArea.withHeight(comboHeight));

    // Link Channels button
    linkChannelsButton.setBounds(extraControls.withTrimmedLeft(10).withHeight(30));
}

void IgnisAudioProcessorEditor::timerCallback()
{
    // Update meters from processor
    inputMeter.setLevels(
        audioProcessor.getInputLevel(0),
        audioProcessor.getInputLevel(1));

    outputMeter.setLevels(
        audioProcessor.getOutputLevel(0),
        audioProcessor.getOutputLevel(1));

    // Update clipping indicator
    clipIndicator.setActive(audioProcessor.getIsClipping());
}
