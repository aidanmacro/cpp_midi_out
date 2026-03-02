#include "KeyboardListener.hpp"
#include "MIDIMessageBuilder.hpp"
#include "MidiMessage.hpp"

#include <RtMidi.h>

#include <chrono>
#include <cctype>
#include <iostream>
#include <thread>
#include <vector>

static int keyToNote(char k)
{
    // White keys starting at middle C (60):
    // a s d f g h j k -> C D E F G A B C
    switch (k) {
        case 'a': return 60;
        case 's': return 62;
        case 'd': return 64;
        case 'f': return 65;
        case 'g': return 67;
        case 'h': return 69;
        case 'j': return 71;
        case 'k': return 72;
        default:  return -1;
    }
}

static unsigned int choosePort(RtMidiOut& out)
{
    const unsigned int nPorts = out.getPortCount();
    if (nPorts == 0) {
        throw std::runtime_error("No MIDI output ports found");
    }

    std::cout << "MIDI output ports:\n";
    for (unsigned int i = 0; i < nPorts; ++i) {
        std::cout << "  [" << i << "] " << out.getPortName(i) << "\n";
    }

    // Default: port 0
    std::cout << "Using port 0 (override later if you want)\n";
    return 1;
}

int main()
{
    try {
        RtMidiOut midiOut;
        const unsigned int port = choosePort(midiOut);
        midiOut.openPort(port);

        MIDIMessageBuilder builder;
        KeyboardListener keys;

        const int channel = 1;
        const MidiMessage::Byte velocity = 90;
        const auto noteLen = std::chrono::milliseconds(150);

        std::cout << "\nPress a/s/d/f/g/h/j/k to play notes. Press q to quit.\n";

        while (true) {
            const char c = keys.readKey();
            const char k = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

            if (k == 'q') {
                break;
            }

            const int note = keyToNote(k);
            if (note < 0) {
                continue; // ignore other keys
            }

            const MidiMessage on  = builder.noteOn(channel, static_cast<MidiMessage::Byte>(note), velocity);
            const MidiMessage off = builder.noteOff(channel, static_cast<MidiMessage::Byte>(note));

            // RtMidi expects std::vector<unsigned char>
            midiOut.sendMessage(&on.bytes());
            std::this_thread::sleep_for(noteLen);
            midiOut.sendMessage(&off.bytes());
        }

        // Panic on exit: All Notes Off + All Sound Off on channel 1
        {
            const MidiMessage allNotesOff{ static_cast<MidiMessage::Byte>(0xB0), 0x7B, 0x00 };
            const MidiMessage allSoundOff{ static_cast<MidiMessage::Byte>(0xB0), 0x78, 0x00 };
            midiOut.sendMessage(&allNotesOff.bytes());
            midiOut.sendMessage(&allSoundOff.bytes());
        }

        std::cout << "\nBye.\n";
        return 0;
    }
    catch (const RtMidiError& e) {
        std::cerr << "RtMidi error: " << e.getMessage() << "\n";
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}