cmake_minimum_required(VERSION 3.16)
project(midi_key_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(rtmidi STATIC
    lib/rtmidi/RtMidi.cpp
)
target_include_directories(rtmidi PUBLIC
    lib/rtmidi
)

target_compile_definitions(rtmidi PRIVATE __LINUX_ALSA__)

# Linux ALSA backend for RtMidi
find_library(ALSA_LIB asound)
if (NOT ALSA_LIB)
    message(FATAL_ERROR "ALSA library (asound) not found")
endif()
target_link_libraries(rtmidi PUBLIC ${ALSA_LIB})

add_executable(midi_key_test
    src/main.cpp
    src/KeyboardListener.cpp
    src/MidiMessage.cpp
    src/MIDIMessageBuilder.cpp
)
target_include_directories(midi_key_test PRIVATE include)
target_link_libraries(midi_key_test PRIVATE rtmidi)