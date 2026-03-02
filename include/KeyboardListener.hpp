#pragma once

/**
 * @brief Provides blocking single-key input in raw terminal mode (Linux/Unix).
 *
 * This utility class configures the controlling terminal to disable canonical
 * mode (line buffering) and echo, allowing individual key presses to be read
 * immediately without requiring Enter.
 *
 * This is intended for development and testing (e.g. driving MIDI output from
 * a terminal). It is not designed to be cross-platform or used as a general
 * input system.
 *
 * The original terminal settings are restored automatically in the destructor.
 */
class KeyboardListener
{
public:
    /**
     * @brief Construct the listener and enable raw terminal mode.
     *
     * On success, subsequent calls to readKey() will return as soon as a single
     * key press is available.
     */
    KeyboardListener();

    /**
     * @brief Destroy the listener and restore the original terminal settings.
     */
    ~KeyboardListener();

    KeyboardListener(const KeyboardListener&) = delete;
    KeyboardListener& operator=(const KeyboardListener&) = delete;

    /**
     * @brief Block until a single key is pressed (no Enter required).
     *
     * @return The pressed character.
     */
    char readKey();

private:
    void enableRawMode();
    void restoreMode();

    bool rawEnabled_ = false;

    // Opaque storage for previous terminal state.
    // Stored as a byte array to keep termios out of this header.
    unsigned char oldTermios_[64];
};