#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
float getParam(juce::AudioProcessorValueTreeState& state, const char* id)
{
    if (auto* p = state.getRawParameterValue(id)) return p->load();
    return 0.0f;
}
float tubeCurve(float x)
{
    return std::tanh(x * 1.35f) / std::tanh(1.35f);
}
float tapeCurve(float x)
{
    const float a = std::abs(x);
    return x / (1.0f + 0.28f * a + 0.12f * a * a);
}
}

juce::AudioProcessorValueTreeState::ParameterLayout BrightsideAudioProcessor::createParameterLayout()
{
    using F = juce::AudioParameterFloat;
    using C = juce::AudioParameterChoice;
    juce::AudioProcessorValueTreeState::ParameterLayout p;
    auto add = [&p](const char* id, const char* name, float lo, float hi, float def)
    { p.add(std::make_unique<F>(juce::ParameterID{id, 1}, name, juce::NormalisableRange<float>(lo, hi), def)); };
    add("DRIVE", "Drive", 0.0f, 18.0f, 5.0f);
    add("TUBE", "Tube", 0.0f, 100.0f, 50.0f);
    add("TAPE", "Tape", 0.0f, 100.0f, 35.0f);
    add("BODY", "Body", -6.0f, 6.0f, 0.0f);
    add("PRESENCE", "Presence", -6.0f, 6.0f, 0.0f);
    add("AIR", "Air", -6.0f, 6.0f, 0.0f);
    add("TONE", "Tone", -1.0f, 1.0f, 0.0f);
    add("DENSITY", "Density", 0.0f, 100.0f, 25.0f);
    add("MIX", "Mix", 0.0f, 100.0f, 100.0f);
    add("INPUT", "Input", -18.0f, 18.0f, 0.0f);
    add("OUTPUT", "Output", -18.0f, 18.0f, 0.0f);
    p.add(std::make_unique<C>(juce::ParameterID{"QUALITY", 1}, "Quality",
        juce::StringArray{"2x", "4x", "8x"}, 1));
    return p;
}

BrightsideAudioProcessor::BrightsideAudioProcessor()
 : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                  .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
   parameters(*this, nullptr, "BRIGHTSIDE_STATE", createParameterLayout())
{
}

void BrightsideAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    dryBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock);
    const auto sr = sampleRate;
    for (auto* s : {&inputGain, &outputGain, &drive, &tube, &tape, &body, &presence, &air, &tone, &density, &mix})
        s->reset(sr, 0.025);
    inputGain.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(getParam(parameters, "INPUT")));
    outputGain.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(getParam(parameters, "OUTPUT")));
    drive.setCurrentAndTargetValue(getParam(parameters, "DRIVE"));
    tube.setCurrentAndTargetValue(getParam(parameters, "TUBE") / 100.0f);
    tape.setCurrentAndTargetValue(getParam(parameters, "TAPE") / 100.0f);
    body.setCurrentAndTargetValue(getParam(parameters, "BODY"));
    presence.setCurrentAndTargetValue(getParam(parameters, "PRESENCE"));
    air.setCurrentAndTargetValue(getParam(parameters, "AIR"));
    tone.setCurrentAndTargetValue(getParam(parameters, "TONE"));
    density.setCurrentAndTargetValue(getParam(parameters, "DENSITY") / 100.0f);
    mix.setCurrentAndTargetValue(getParam(parameters, "MIX") / 100.0f);
}
void BrightsideAudioProcessor::releaseResources() {}
bool BrightsideAudioProcessor::isBusesLayoutSupported(const BusesLayout& l) const
{
    const auto in = l.getMainInputChannelSet();
    const auto out = l.getMainOutputChannelSet();
    return in == out && (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo());
}

void BrightsideAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int channels = juce::jmin(buffer.getNumChannels(), getTotalNumInputChannels());
    const int n = buffer.getNumSamples();
    if (dryBuffer.getNumChannels() != buffer.getNumChannels() || dryBuffer.getNumSamples() < n)
        dryBuffer.setSize(buffer.getNumChannels(), n, false, false, true);
    dryBuffer.makeCopyOf(buffer, true);

    inputGain.setTargetValue(juce::Decibels::decibelsToGain(getParam(parameters, "INPUT")));
    outputGain.setTargetValue(juce::Decibels::decibelsToGain(getParam(parameters, "OUTPUT")));
    drive.setTargetValue(getParam(parameters, "DRIVE"));
    tube.setTargetValue(getParam(parameters, "TUBE") / 100.0f);
    tape.setTargetValue(getParam(parameters, "TAPE") / 100.0f);
    body.setTargetValue(getParam(parameters, "BODY"));
    presence.setTargetValue(getParam(parameters, "PRESENCE"));
    air.setTargetValue(getParam(parameters, "AIR"));
    tone.setTargetValue(getParam(parameters, "TONE"));
    density.setTargetValue(getParam(parameters, "DENSITY") / 100.0f);
    mix.setTargetValue(getParam(parameters, "MIX") / 100.0f);

    const float qualityScale = 1.0f + 0.0f * getParam(parameters, "QUALITY"); // Quality selector reserved for future oversampling engine.
    juce::ignoreUnused(qualityScale);
    for (int i = 0; i < n; ++i)
    {
        const float inG = inputGain.getNextValue();
        const float outG = outputGain.getNextValue();
        const float drv = juce::Decibels::decibelsToGain(drive.getNextValue());
        const float t = tube.getNextValue();
        const float tp = tape.getNextValue();
        const float b = body.getNextValue();
        const float pr = presence.getNextValue();
        const float a = air.getNextValue();
        const float tn = tone.getNextValue();
        const float den = density.getNextValue();
        const float wet = mix.getNextValue();
        const float lowTilt = juce::Decibels::decibelsToGain(b * 0.45f + tn * -2.0f);
        const float midTilt = juce::Decibels::decibelsToGain(pr * 0.35f);
        const float highTilt = juce::Decibels::decibelsToGain(a * 0.45f + tn * 2.0f);
        for (int ch = 0; ch < channels; ++ch)
        {
            const float dry = dryBuffer.getSample(ch, i);
            float x = dry * inG * drv;
            const float tubeY = tubeCurve(x);
            const float tapeY = tapeCurve(x);
            float shaped = x * (1.0f - t) * (1.0f - tp) + tubeY * t * (1.0f - tp) + tapeY * tp;
            shaped = shaped * (1.0f + den * 0.22f);
            shaped = std::tanh(shaped * (1.0f + den * 0.35f));
            shaped *= (lowTilt + midTilt + highTilt) / 3.0f;
            const float processed = shaped * outG;
            buffer.setSample(ch, i, dry * (1.0f - wet) + processed * wet);
        }
    }
    for (int ch = channels; ch < buffer.getNumChannels(); ++ch) buffer.clear(ch, 0, n);
}

void BrightsideAudioProcessor::loadPreset(int index)
{
    static const float presets[4][11] = {
        {4.5f, 48, 28, 0.5f, 1.0f, 1.0f, 0.0f, 22, 100, 0, 0},
        {7.0f, 78, 12, 1.0f, 1.5f, 0.5f, -0.1f, 38, 85, 0, -1},
        {6.0f, 18, 82, 1.5f, 0.0f, -0.5f, -0.15f, 45, 90, 0, -1},
        {3.5f, 42, 20, 0.0f, 2.5f, 3.0f, 0.25f, 18, 90, 0, -1}
    };
    const char* ids[] = {"DRIVE","TUBE","TAPE","BODY","PRESENCE","AIR","TONE","DENSITY","MIX","INPUT","OUTPUT"};
    index = juce::jlimit(0, 3, index);
    for (int i = 0; i < 11; ++i)
        if (auto* p = parameters.getParameter(ids[i]))
            p->setValueNotifyingHost(p->convertTo0to1(presets[index][i]));
    currentProgram = index;
}
void BrightsideAudioProcessor::setCurrentProgram(int index) { loadPreset(index); }
const juce::String BrightsideAudioProcessor::getProgramName(int index)
{
    const juce::StringArray names{"VOCAL WARMTH", "TUBE GLOW", "TAPE THICKNESS", "AIR & PRESENCE"};
    return names[juce::jlimit(0, 3, index)];
}
void BrightsideAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, dest);
}
void BrightsideAudioProcessor::setStateInformation(const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, size));
    if (xml && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}
juce::AudioProcessorEditor* BrightsideAudioProcessor::createEditor()
{
    return new BrightsideAudioProcessorEditor(*this);
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BrightsideAudioProcessor();
}
