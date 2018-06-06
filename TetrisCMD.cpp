/* Command line version of Tetris
** Author: MapleDate
*/

//  ████████╗███████╗████████╗██████╗ ██╗███████╗                                   
//  ╚══██╔══╝██╔════╝╚══██╔══╝██╔══██╗██║██╔════╝                                   
//     ██║   █████╗     ██║   ██████╔╝██║███████╗                                   
//     ██║   ██╔══╝     ██║   ██╔══██╗██║╚════██║                                   
//     ██║   ███████╗   ██║   ██║  ██║██║███████║                                   
//     ╚═╝   ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝╚══════╝                                   
//                                                                                  
//  ██╗   ██╗ ██╗    ██████╗                                                        
//  ██║   ██║███║   ██╔═████╗                                                       
//  ██║   ██║╚██║   ██║██╔██║                                                       
//  ╚██╗ ██╔╝ ██║   ████╔╝██║                                                       
//   ╚████╔╝  ██║██╗╚██████╔╝                                                       
//    ╚═══╝   ╚═╝╚═╝ ╚═════╝                              
//                                                                                  
//  ███╗   ███╗ █████╗ ██████╗ ██╗     ███████╗██████╗  █████╗ ████████╗███████╗    
//  ████╗ ████║██╔══██╗██╔══██╗██║     ██╔════╝██╔══██╗██╔══██╗╚══██╔══╝██╔════╝    
//  ██╔████╔██║███████║██████╔╝██║     █████╗  ██║  ██║███████║   ██║   █████╗      
//  ██║╚██╔╝██║██╔══██║██╔═══╝ ██║     ██╔══╝  ██║  ██║██╔══██║   ██║   ██╔══╝      
//  ██║ ╚═╝ ██║██║  ██║██║     ███████╗███████╗██████╔╝██║  ██║   ██║   ███████╗  
//

/*
 _____          _            _       
|_   _|        | |          (_)      
  | |     ___  | |_   _ __   _   ___ 
  | |    / _ \ | __| | ___| | | / __|
  | |   |  __/ | |_  | |    | | \__ \
  \_/    \___|  \__| |_|    |_| |___/
                                 _ 
                        \  / /| / \
                         \/   |o\_/
                                        (╯°□°）╯︵ [_]||
                                        з=(◕_◕)=ε
                __        
|\/| _  _ | _  |  \ _ |_ _
|  |(_||_)|(-  |__/(_||_(-
       |   

            
  */

#include <Windows.h>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
using namespace std;

// Console Configs
static const int CONSOLE_W = 44;
static const int CONSOLE_H = 44;
static HANDLE console_screen;
static DWORD chars_written = 0;
static char* buf_display = 0;
static vector<string> tetrominoes;

// Game Stats
static const int TETRO_W = 10;
static const int TETRO_H = 4;
static int tetro_x = CONSOLE_W / 2 - TETRO_W / 2 ;  // init x pos at the middle
static int tetro_y = 2;  // init y pos on the top
static int game_speed = 500;  // milisecond pause between frames
static int score = 0;

void BufCleanUp() {
    if (buf_display) delete[] buf_display;
}

void HandleErrorExit(string msg, DWORD err) {
    SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE));
    cerr << msg << endl;
    cerr << "Error code :" << err << endl;
    cout << "Enter any key to exit..." << endl;
    cin.ignore();
    cin.get();

    BufCleanUp();
    exit(1);
}

void HandleNormalExit() {
    BufCleanUp();
    exit(0);
}

void ClearDisplay() {
    for (int i = 0; i < CONSOLE_W * CONSOLE_H; i++) {
        buf_display[i] = ' ';
    }
}

void InitTetrominos(){
    string I = "";
    string O = "";
    string Z = "";
    string T = "";
    string L = "";

    I += "....\xDB\xDB....";
    I += "....\xDB\xDB....";  
    I += "....\xDB\xDB....";  
    I += "....\xDB\xDB....";
   

    tetrominoes.push_back(I);
    tetrominoes.push_back(O);
    tetrominoes.push_back(Z);
    tetrominoes.push_back(T);
    tetrominoes.push_back(L);
}

// Initialize display console screen
void InitPlayField() {
    // console screen settings
    string window_title = "TetrisCMD - MapleDate";
    SMALL_RECT min_window_size = {0, 0, (short)1, (short)1};
    SMALL_RECT window_size = {0, 0, (short)(CONSOLE_W - 1),
                              (short)(CONSOLE_H - 1)};
    // Init empty display buf
    buf_display = new char[CONSOLE_W * CONSOLE_H];
    // Init all Tetronimoes 
    InitTetrominos();

    // create console screen buf and set window name
    console_screen = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleTitle(window_title.c_str());
    // resize console size
    if (!SetConsoleWindowInfo(console_screen, TRUE, &min_window_size))
        HandleErrorExit("Failed resizing window(1st)", GetLastError());
    if (!SetConsoleScreenBufferSize(console_screen,
                                    {(short)CONSOLE_W, (short)CONSOLE_H}))
        HandleErrorExit("Failed resizing screen buffer", GetLastError());
    if (!SetConsoleWindowInfo(console_screen, TRUE, &window_size))
        HandleErrorExit("Failed resizing window(2nd)", GetLastError());

    // Disable console window resize
    HWND console_window = FindWindow(NULL, window_title.c_str());
    SetWindowPos(console_window, HWND_TOPMOST, 10, 10, 0, 0, SWP_NOSIZE);
    SetWindowLong(console_window, GWL_STYLE,
                  GetWindowLong(console_window, GWL_STYLE) & ~WS_MAXIMIZEBOX &
                      ~WS_SIZEBOX);
    // Hide cursor

    // Activate new scrren
    SetConsoleActiveScreenBuffer(console_screen);
}

void DrawBoundaries() {
    for (int y = 0; y < CONSOLE_H; y++) {
        for (int x = 0; x < CONSOLE_W; x++) {
            if (x > 0 && x < CONSOLE_W - 1 && y > 0 && y < CONSOLE_H - 1) {
                if (x == 1 || x == CONSOLE_W - 2)
                    buf_display[y * CONSOLE_W + x] = 186;  // left,right
                if (y == CONSOLE_H - 2)
                    buf_display[y * CONSOLE_W + x] = 205;  // bottom
                if (y == 1) 
                    buf_display[y * CONSOLE_W + x] = 196;  // upper
            }
        }
    }
    buf_display[1 * CONSOLE_W + 1] = 214;  //Upper left
    buf_display[1 * CONSOLE_W + (CONSOLE_W - 2)] = 183;  //upper right
    buf_display[(CONSOLE_H - 2) * CONSOLE_W + 1] = 200;  //bottom left
    buf_display[(CONSOLE_H - 2) * CONSOLE_W + (CONSOLE_W - 2)] = 188; //bottom right
}

// Draw the falling tetromino
void DrawTetromino() {
    string tetromino = tetrominoes[0];
    for (int y = 0; y < TETRO_H; y++) {
        for (int x = 0; x < TETRO_W; x++) {
            char pixel = tetromino[y * TETRO_W + x];
            if(pixel != '.' || true) {
                buf_display[(tetro_y + y) * CONSOLE_W + (tetro_x + x)] = pixel;
            }
        }
    }
}

void UpdateScore() {
    stringstream msg_score;
    msg_score << "Score: " << score;
    strcpy(&buf_display[2], msg_score.str().c_str());
}

void RenderDidplay() {
    WriteConsoleOutputCharacterA(console_screen, buf_display,
                                 CONSOLE_W * CONSOLE_H, COORD({0, 0}),
                                 &chars_written);
}

void HandleKeyPress() {
    if (GetAsyncKeyState(VK_ESCAPE)) {
        
        // score++;
        // Sleep(1000);
        HandleNormalExit();
    }
}

void StartGameLoop() {
    while (1) {
        ClearDisplay();
        HandleKeyPress();
        UpdateScore();
        DrawBoundaries();
        DrawTetromino();

        RenderDidplay();
        Sleep(game_speed);

        // tetro_y++;
    }
}

int main(int argc, char* argv[]) {
    InitPlayField();
    StartGameLoop();

    return 0;
}
