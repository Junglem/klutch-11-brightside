#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class BrightsideAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit BrightsideAudioProcessorEditor(BrightsideAudioProcessor&);
    ~BrightsideAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    BrightsideAudioProcessor& processor;
    juce::LookAndFeel_V4 look;
    std::array<std::unique_ptr<juce::Slider>, 11> knobs;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 11> attachments;
    std::unique_ptr<juce::ComboBox> quality;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> qualityAttachment;
    std::unique_ptr<juce::ComboBox> presets;
    std::unique_ptr<juce::TextButton> loadButton;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BrightsideAudioProcessorEditor)
};
