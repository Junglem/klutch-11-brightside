cmake_minimum_required(VERSION 3.22)
project(KLUTCH11_BRIGHTSIDE VERSION 1.0.0)

include(FetchContent)
FetchContent_Declare(JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.6
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(KLUTCH11BRIGHTSIDE
  COMPANY_NAME "KLUTCH 11"
  BUNDLE_ID com.klutch11.brightside
  PLUGIN_MANUFACTURER_CODE KL11
  PLUGIN_CODE Brts
  PRODUCT_NAME "KLUTCH 11 BRIGHTSIDE"
  FORMATS VST3
  VST3_CATEGORIES Fx
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE)

juce_generate_juce_header(KLUTCH11BRIGHTSIDE)

target_sources(KLUTCH11BRIGHTSIDE PRIVATE
  Source/PluginProcessor.cpp
  Source/PluginEditor.cpp)

target_compile_features(KLUTCH11BRIGHTSIDE PRIVATE cxx_std_17)
target_compile_definitions(KLUTCH11BRIGHTSIDE PUBLIC
  JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(KLUTCH11BRIGHTSIDE PRIVATE
  juce::juce_audio_utils
  juce::juce_dsp
  PUBLIC
  juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags)
