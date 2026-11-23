#pragma once
#include <JuceHeader.h>

class BrightsideAudioProcessor final : public juce::AudioProcessor
{
public:
    BrightsideAudioProcessor();
    ~BrightsideAudioProcessor() override = default;
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "KLUTCH 11 BRIGHTSIDE"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 4; }
    int getCurrentProgram() override { return currentProgram; }
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState parameters;
    void loadPreset(int index);
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    int currentProgram = 0;
    juce::AudioBuffer<float> dryBuffer;
    juce::SmoothedValue<float> inputGain, outputGain, drive, tube, tape, body, presence, air, tone, density, mix;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BrightsideAudioProcessor)
};
