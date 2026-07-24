cmake_minimum_required(VERSION 3.24)

project(ydna_stereodrive VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.12
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

FetchContent_Declare(
    signalsmith_stretch
    GIT_REPOSITORY https://github.com/Signalsmith-Audio/signalsmith-stretch.git
    GIT_TAG main
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(signalsmith_stretch)

set(YDNA_PLUGIN_FORMATS VST3 Standalone)
if(APPLE)
    list(APPEND YDNA_PLUGIN_FORMATS AU)
endif()

juce_add_plugin(ydna_stereodrive
    COMPANY_NAME "ydna"
    PRODUCT_NAME "stereodrive"
    PLUGIN_MANUFACTURER_CODE Ydna
    PLUGIN_CODE Ysd1
    FORMATS ${YDNA_PLUGIN_FORMATS}
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    VST3_CATEGORIES Fx Distortion
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(ydna_stereodrive
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/Parameters.cpp
        Source/dsp/Crossover.cpp
        Source/dsp/BandCompressor.cpp
        Source/dsp/OverdriveStage.cpp
        Source/dsp/Voicing.cpp
        Source/dsp/PitchShifter.cpp
        Source/dsp/CabinetSim.cpp
        Source/gui/LookAndFeel.cpp
        Source/gui/BandPanel.cpp
        Source/gui/SidePanel.cpp
        Source/gui/CabPanel.cpp
        Source/gui/GatePanel.cpp
        Source/gui/ChorusPanel.cpp
)

target_include_directories(ydna_stereodrive
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_compile_definitions(ydna_stereodrive
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(ydna_stereodrive
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        signalsmith-stretch
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
