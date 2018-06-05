/* Command line version of Tetris
** Author: MapleDate
*/

#include <Windows.h>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
using namespace std;

// Console Configs
static int CONSOLE_W = 44;
static int CONSOLE_H = 44;
static HANDLE console_screen;
static DWORD chars_written = 0;
static char* buf_display = 0;

// Game Stats
static int game_speed = 500;  // milisecond pause between frames
static int score = 0;

void bufCleanUp() {
    if (buf_display) delete buf_display;
}

void handleErrorExit(string msg, DWORD err) {
    SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE));
    cerr << msg << endl;
    cerr << "Error code :" << err << endl;
    cout << "Enter any key to exit..." << endl;
    cin.ignore();
    cin.get();

    bufCleanUp();
    exit(1);
}

void handleNormalExit() {
    bufCleanUp();
    exit(0);
}

void clearDisplay() {
    for (int i = 0; i < CONSOLE_W * CONSOLE_H; i++) {
        buf_display[i] = ' ';
    }
}

// Initialize display console screen
void initPlayField() {
    // console screen settings
    string window_title = "TetrisCMD - MapleDate";
    SMALL_RECT min_window_size = {0, 0, (short)1, (short)1};
    SMALL_RECT window_size = {0, 0, (short)(CONSOLE_W - 1),
                              (short)(CONSOLE_H - 1)};
    // Init empty display buf
    buf_display = new char[CONSOLE_W * CONSOLE_H];

    // create and set console screen buf
    console_screen = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleTitle(window_title.c_str());
    // resize console size
    if (!SetConsoleWindowInfo(console_screen, TRUE, &min_window_size))
        handleErrorExit("Failed resizing window(1st)", GetLastError());

    if (!SetConsoleScreenBufferSize(console_screen,
                                    {(short)CONSOLE_W, (short)CONSOLE_H}))
        handleErrorExit("Failed resizing screen buffer", GetLastError());

    if (!SetConsoleWindowInfo(console_screen, TRUE, &window_size))
        handleErrorExit("Failed resizing window(2nd)", GetLastError());

    // Disable console window resize, and activate new scrren
    HWND console_window = FindWindow(NULL, window_title.c_str());
    SetWindowPos(console_window, HWND_TOPMOST, 10, 10, 0, 0, SWP_NOSIZE);
    SetWindowLong(console_window, GWL_STYLE,
                  GetWindowLong(console_window, GWL_STYLE) & ~WS_MAXIMIZEBOX &
                      ~WS_SIZEBOX);
    SetConsoleActiveScreenBuffer(console_screen);
}

void handleKeyPress() { return; }

void drawBoundaries() {
    for (int y = 0; y < CONSOLE_H; y++) {
        for (int x = 0; x < CONSOLE_W; x++) {
            if (x == 1 || y == 1 || x == CONSOLE_W - 2 || y == CONSOLE_H - 2) {
                if (x > 0 && y > 1 && x < CONSOLE_W - 1 && y < CONSOLE_H - 1)
                    buf_display[y * CONSOLE_W + x] = '#';
                else if (y == 1)
                    buf_display[y * CONSOLE_W + x] = '~';
            }
        }
    }
}

void updateScore() {
    stringstream msg_score;
    msg_score << "Score: " << score;
    strcpy(&buf_display[1], msg_score.str().c_str());
}

void updateDidplay() {
    WriteConsoleOutputCharacterA(console_screen, buf_display,
                                 CONSOLE_W * CONSOLE_H, COORD({0, 0}),
                                 &chars_written);
}

void startGameLoop() {
    while (1) {
        clearDisplay();
        handleKeyPress();
        if (GetAsyncKeyState(VK_ESCAPE)) {
            // score++;
            // Sleep(1000);
            handleNormalExit();
        }

        updateScore();
        drawBoundaries();
        updateDidplay();
        Sleep(game_speed);
    }
}

int main(int argc, char* argv[]) {
    initPlayField();
    startGameLoop();

    return 0;
}
