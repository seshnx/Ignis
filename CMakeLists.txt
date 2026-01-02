cmake_minimum_required(VERSION 3.22)

project(Ignis VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE setup via FetchContent
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

# Plugin definition
juce_add_plugin(Ignis
    COMPANY_NAME "SeshNx"
    PLUGIN_MANUFACTURER_CODE Sesh
    PLUGIN_CODE Igni
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "SeshNx Ignis"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

# Source files
target_sources(Ignis
    PRIVATE
        # Core
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp

        # DSP
        Source/DSP/SoftClipperCore.cpp
        Source/DSP/AntiAliasingFilter.cpp

        # UI
        Source/UI/IgnisLookAndFeel.cpp
        Source/UI/StereoMeter.cpp
        Source/UI/ClippingIndicator.cpp

        # Utils
        Source/Utils/Parameters.cpp
        Source/Utils/PresetManager.cpp
)

# Include directories
target_include_directories(Ignis
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/DSP
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/UI
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Utils
)

# Binary resources (logo)
juce_add_binary_data(Ignis_BinaryData
    SOURCES
        Resources/company_logo.png
    HEADER_NAME BinaryData.h
)

# Compiler definitions
target_compile_definitions(Ignis
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

# Link JUCE modules
target_link_libraries(Ignis
    PRIVATE
        Ignis_BinaryData
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
