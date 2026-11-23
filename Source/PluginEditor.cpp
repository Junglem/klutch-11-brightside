#include "PluginEditor.h"

BrightsideAudioProcessorEditor::BrightsideAudioProcessorEditor(BrightsideAudioProcessor& p)
 : AudioProcessorEditor(&p), processor(p)
{
    setSize(900, 650);
    look.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffc49a58));
    look.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff39332b));
    look.setColour(juce::Slider::thumbColourId, juce::Colour(0xfff0d7a5));
    look.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xfff3e7d2));
    look.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff171513));
    look.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff514331));
    const char* ids[] = {"DRIVE","TUBE","TAPE","BODY","PRESENCE","AIR","TONE","DENSITY","MIX","INPUT","OUTPUT"};
    for (size_t i = 0; i < knobs.size(); ++i)
    {
        knobs[i] = std::make_unique<juce::Slider>();
        knobs[i]->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        knobs[i]->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 76, 20);
        knobs[i]->setLookAndFeel(&look);
        addAndMakeVisible(*knobs[i]);
        attachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, ids[i], *knobs[i]);
    }
    quality = std::make_unique<juce::ComboBox>();
    quality->addItemList({"2x", "4x", "8x"}, 1);
    addAndMakeVisible(*quality);
    qualityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, "QUALITY", *quality);
    presets = std::make_unique<juce::ComboBox>();
    presets->addItemList({"VOCAL WARMTH", "TUBE GLOW", "TAPE THICKNESS", "AIR & PRESENCE"}, 1);
    presets->setSelectedId(1);
    addAndMakeVisible(*presets);
    loadButton = std::make_unique<juce::TextButton>("LOAD PRESET");
    addAndMakeVisible(*loadButton);
    loadButton->onClick = [this] { processor.loadPreset(presets->getSelectedId() - 1); };
}
BrightsideAudioProcessorEditor::~BrightsideAudioProcessorEditor()
{
    for (auto& k : knobs) if (k) k->setLookAndFeel(nullptr);
}
void BrightsideAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff10100f));
    g.setColour(juce::Colour(0xffc49a58));
    g.fillRect(0, 0, getWidth(), 5);
    g.setColour(juce::Colour(0xfff0d7a5));
    g.setFont(juce::Font(juce::FontOptions(31.0f, juce::Font::bold)));
    g.drawText("KLUTCH 11", 30, 20, 300, 38, juce::Justification::left);
    g.setFont(juce::Font(juce::FontOptions(22.0f, juce::Font::bold)));
    g.drawText("BRIGHTSIDE", 30, 58, 300, 30, juce::Justification::left);
    g.setColour(juce::Colour(0xffa99b83));
    g.setFont(juce::Font(juce::FontOptions(11.0f)));
    g.drawText("ANALOGUE-INSPIRED VOCAL SATURATION", 32, 91, 420, 20, juce::Justification::left);
    g.setColour(juce::Colour(0xff5a4a34));
    g.drawLine(30.0f, 120.0f, 870.0f, 120.0f, 1.0f);
    g.setColour(juce::Colour(0xffc49a58));
    g.drawText("TONE SHAPING", 30, 600, 150, 18, juce::Justification::left);
    g.setColour(juce::Colour(0xff8c806e));
    g.drawText("WARMTH WITHOUT LOSING CLARITY", 30, 620, 400, 18, juce::Justification::left);
}
void BrightsideAudioProcessorEditor::resized()
{
    const int x0 = 35, y0 = 145, w = 205, h = 145;
    for (int i = 0; i < 11; ++i)
    {
        const int col = i % 4, row = i / 4;
        knobs[(size_t)i]->setBounds(x0 + col * w, y0 + row * h, 150, 130);
    }
    quality->setBounds(700, 48, 150, 28);
    presets->setBounds(590, 570, 190, 28);
    loadButton->setBounds(785, 570, 90, 28);
}
