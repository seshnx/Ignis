#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
IgnisAudioProcessor::IgnisAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, juce::Identifier("IgnisParams"),
                 Ignis::createParameterLayout()),
      presetManager(parameters)
{
    // Cache parameter pointers
    inputGainParam = parameters.getRawParameterValue(Ignis::ParamIDs::inputGain);
    driveParam = parameters.getRawParameterValue(Ignis::ParamIDs::drive);
    thresholdParam = parameters.getRawParameterValue(Ignis::ParamIDs::threshold);
    ceilingParam = parameters.getRawParameterValue(Ignis::ParamIDs::ceiling);
    mixParam = parameters.getRawParameterValue(Ignis::ParamIDs::mix);
    outputGainParam = parameters.getRawParameterValue(Ignis::ParamIDs::outputGain);
    clipModeParam = parameters.getRawParameterValue(Ignis::ParamIDs::clipMode);
    oversamplingParam = parameters.getRawParameterValue(Ignis::ParamIDs::oversampling);
    linkChannelsParam = parameters.getRawParameterValue(Ignis::ParamIDs::linkChannels);
}

IgnisAudioProcessor::~IgnisAudioProcessor() {}

//==============================================================================
const juce::String IgnisAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool IgnisAudioProcessor::acceptsMidi() const { return false; }
bool IgnisAudioProcessor::producesMidi() const { return false; }
bool IgnisAudioProcessor::isMidiEffect() const { return false; }
double IgnisAudioProcessor::getTailLengthSeconds() const { return 0.0; }

//==============================================================================
int IgnisAudioProcessor::getNumPrograms() { return 1; }
int IgnisAudioProcessor::getCurrentProgram() { return 0; }
void IgnisAudioProcessor::setCurrentProgram(int) {}
const juce::String IgnisAudioProcessor::getProgramName(int) { return {}; }
void IgnisAudioProcessor::changeProgramName(int, const juce::String&) {}

//==============================================================================
void IgnisAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare DSP components
    softClipper.prepare(sampleRate, samplesPerBlock);

    int oversampleFactor = 1 << static_cast<int>(oversamplingParam->load()); // 1, 2, or 4
    antiAliasing.prepare(sampleRate, samplesPerBlock, oversampleFactor);

    // Prepare dry buffer for wet/dry mixing
    dryBuffer.setSize(2, samplesPerBlock);

    // Initialize smoothed values (20-50ms for smooth transitions)
    smoothedInputGain.reset(sampleRate, 0.02);
    smoothedDrive.reset(sampleRate, 0.05);
    smoothedThreshold.reset(sampleRate, 0.02);
    smoothedCeiling.reset(sampleRate, 0.02);
    smoothedMix.reset(sampleRate, 0.05);
    smoothedOutputGain.reset(sampleRate, 0.02);

    // Set initial values
    smoothedInputGain.setCurrentAndTargetValue(
        juce::Decibels::decibelsToGain(inputGainParam->load()));
    smoothedDrive.setCurrentAndTargetValue(driveParam->load());
    smoothedThreshold.setCurrentAndTargetValue(thresholdParam->load());
    smoothedCeiling.setCurrentAndTargetValue(ceilingParam->load());
    smoothedMix.setCurrentAndTargetValue(mixParam->load() / 100.0f);
    smoothedOutputGain.setCurrentAndTargetValue(
        juce::Decibels::decibelsToGain(outputGainParam->load()));
}

void IgnisAudioProcessor::releaseResources()
{
    softClipper.reset();
    antiAliasing.reset();
}

bool IgnisAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void IgnisAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                        juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear unused channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Get parameter values
    float inputGainDb = inputGainParam->load();
    float drive = driveParam->load();
    float threshold = thresholdParam->load();
    float ceiling = ceilingParam->load();
    float mix = mixParam->load();
    float outputGainDb = outputGainParam->load();
    int clipMode = static_cast<int>(clipModeParam->load());
    bool linkChannels = linkChannelsParam->load() > 0.5f;

    // Update smoothed values
    smoothedInputGain.setTargetValue(juce::Decibels::decibelsToGain(inputGainDb));
    smoothedDrive.setTargetValue(drive);
    smoothedThreshold.setTargetValue(threshold);
    smoothedCeiling.setTargetValue(ceiling);
    smoothedMix.setTargetValue(mix / 100.0f);
    smoothedOutputGain.setTargetValue(juce::Decibels::decibelsToGain(outputGainDb));

    // Process soft clipping (all smoothing and processing is done inside)
    softClipper.process(buffer,
                       smoothedInputGain.getNextValue(),
                       smoothedDrive.getNextValue(),
                       smoothedThreshold.getNextValue(),
                       smoothedCeiling.getNextValue(),
                       smoothedMix.getNextValue(),
                       smoothedOutputGain.getNextValue(),
                       clipMode,
                       linkChannels);

    // TODO: Apply oversampling when implemented
    // int oversampleFactor = 1 << static_cast<int>(oversamplingParam->load());
    // antiAliasing.process(buffer, oversampleFactor);
}

//==============================================================================
void IgnisAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save plugin state
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void IgnisAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Load plugin state
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

    if (xml && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

//==============================================================================
juce::AudioProcessorEditor* IgnisAudioProcessor::createEditor()
{
    return new IgnisAudioProcessorEditor(*this);
}

bool IgnisAudioProcessor::hasEditor() const
{
    return true;
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new IgnisAudioProcessor();
}
