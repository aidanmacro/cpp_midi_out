#include "KeyboardListener.hpp"

#include <stdexcept>
#include <unistd.h>   // read
#include <termios.h>  // tcgetattr, tcsetattr

KeyboardListener::KeyboardListener()
{
    enableRawMode();
}

KeyboardListener::~KeyboardListener()
{
    restoreMode();
}

void KeyboardListener::enableRawMode()
{
    termios oldt{};
    if (tcgetattr(STDIN_FILENO, &oldt) == -1) {
        throw std::runtime_error("tcgetattr failed");
    }

    // Save old settings into our opaque buffer
    static_assert(sizeof(oldTermios_) >= sizeof(termios), "oldTermios_ too small");
    *reinterpret_cast<termios*>(oldTermios_) = oldt;

    termios raw = oldt;

    // Disable canonical mode (line buffering) and echo.
    raw.c_lflag &= static_cast<unsigned int>(~(ICANON | ECHO));

    // Read returns as soon as 1 byte is available.
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        throw std::runtime_error("tcsetattr failed");
    }

    rawEnabled_ = true;
}

void KeyboardListener::restoreMode()
{
    if (!rawEnabled_) return;

    const termios oldt = *reinterpret_cast<const termios*>(oldTermios_);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldt);
    rawEnabled_ = false;
}

char KeyboardListener::readKey()
{
    char c = 0;
    const ssize_t n = ::read(STDIN_FILENO, &c, 1);
    if (n != 1) {
        throw std::runtime_error("read failed");
    }
    return c;
}