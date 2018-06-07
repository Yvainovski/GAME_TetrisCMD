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
#include <math.h>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include <map>
#define MATH_PI 3.141592653
using namespace std;

// Console Configs
static const int CONSOLE_W = 44;
static const int CONSOLE_H = 44;
static HANDLE console_screen;
static DWORD chars_written = 0;
static char* buf_display = 0;
static map<char, vector<string>> tetromino_types;

// Game Stats
enum TetroType { I, O, Z, T, L };
static const int NUM_TETRO_TYPE = 5;
static const int TETRO_W = 10;
static const int TETRO_H = 4;
static int tetro_x = CONSOLE_W / 2 - TETRO_W / 2 ;  // init x pos at the middle
static int tetro_y = 2;  // init y pos on the top
static int cur_tetro_type = 0;
static int cur_tetro_orientation = 0;
static int game_speed = 500;  // milisecond pause between frames
static int score = 0;

void BufCleanUp() {
    if (buf_display) delete[] buf_display;
}

void HandleNormalExit() {
    BufCleanUp();
    exit(0);
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

void ClearDisplay() {
    for (int i = 0; i < CONSOLE_W * CONSOLE_H; i++) {
        buf_display[i] = ' ';
    }
}

// Make all tetroes (╯°□°）╯︵ [_]||
void InitTetrominoes(){
    vector<string> Is(4);
    vector<string> Os(4);
    vector<string> Zs(4);
    vector<string> Ts(4);
    vector<string> Ls(4);
    
    // tmp var for making tetromino blocks
    string tmp = "";

    // I 0 2 
    tmp += "....\xDB\xDB....";
    tmp += "....\xDB\xDB....";  
    tmp += "....\xDB\xDB....";  
    tmp += "....\xDB\xDB....";
    Is[0] = tmp;
    Is[2] = tmp;
    tmp = "";
    
    // I 1 3
    tmp += "..........";
    tmp += ".\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB.";  
    tmp += "..........";
    tmp += "..........";
    Is[1] = tmp;
    Is[3] = tmp;
    tmp = "";

    // O 0 1 2 3
    tmp += "...\xDB\xDB\xDB\xDB...";  
    tmp += "...\xDB\xDB\xDB\xDB..."; 
    tmp += "..........";
    tmp += "..........";
    Os[0] = tmp;
    Os[1] = tmp;
    Os[2] = tmp;
    Os[3] = tmp;
    tmp = "";

    // Z 0 2 
    tmp += "..\xDB\xDB\xDB\xDB....";  
    tmp += "....\xDB\xDB\xDB\xDB.."; 
    tmp += "..........";    
    tmp += "..........";
    Zs[0] = tmp;
    Zs[2] = tmp;
    tmp = "";
    // Z 1 3 
    tmp += ".....\xDB\xDB...";  
    tmp += "...\xDB\xDB\xDB\xDB...";  
    tmp += "...\xDB\xDB....."; 
    tmp += "..........";
    Zs[1] = tmp;
    Zs[3] = tmp;
    tmp = "";

   
    // T0
    tmp += "..\xDB\xDB\xDB\xDB\xDB\xDB..";  
    tmp += "....\xDB\xDB....";
    tmp += "..........";
    tmp += "..........";
    Ts[0] = tmp;
    tmp = "";
    // T1
    tmp += ".....\xDB\xDB...";
    tmp += "...\xDB\xDB\xDB\xDB...";  
    tmp += ".....\xDB\xDB..."; 
    tmp += "..........";
    Ts[1] = tmp;
    tmp = "";
    // T2
    tmp += "....\xDB\xDB....";
    tmp += "..\xDB\xDB\xDB\xDB\xDB\xDB..";  
    tmp += "..........";
    tmp += "..........";
    Ts[2] = tmp;
    tmp = "";
    // T3
    tmp += "...\xDB\xDB.....";
    tmp += "...\xDB\xDB\xDB\xDB...";  
    tmp += "...\xDB\xDB....."; 
    tmp += "..........";
    Ts[3] = tmp;
    tmp = "";

    // L0
    tmp += "..\xDB\xDB\xDB\xDB\xDB\xDB..";
    tmp += "..\xDB\xDB......";
    tmp += "..........";
    tmp += "..........";
    Ls[0] = tmp;
    tmp = "";
    // L1
    tmp += "...\xDB\xDB\xDB\xDB...";
    tmp += ".....\xDB\xDB...";
    tmp += ".....\xDB\xDB...";
    tmp += "..........";
    Ls[1] = tmp;
    tmp = "";
    // L2
    tmp += "......\xDB\xDB..";
    tmp += "..\xDB\xDB\xDB\xDB\xDB\xDB..";
    tmp += "..........";
    tmp += "..........";
    Ls[2] = tmp;
    tmp = ""; 
    // L3
    tmp += "...\xDB\xDB.....";
    tmp += "...\xDB\xDB.....";
    tmp += "...\xDB\xDB\xDB\xDB...";
    tmp += "..........";
    Ls[3] = tmp;
    tmp = "";

    tetromino_types[TetroType::I] = Is;
    tetromino_types[TetroType::O] = Os;
    tetromino_types[TetroType::Z] = Zs;
    tetromino_types[TetroType::T] = Ts;
    tetromino_types[TetroType::L] = Ls;
}



// // Angle: 0, 90, 180, 270
// char RotateTetromino(string const& tetromino, int x, int y, int angle) {
//     int loc;
//     switch(angle){
//         case 0:
//             break;
//         case 90:
//             break;
//     }
    
//     return  tetromino[dy * TETRO_W + dx];
// }

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
    InitTetrominoes();

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
                    buf_display[y * CONSOLE_W + x] = '_';  // upper 196
            }
        }
    }
    buf_display[1 * CONSOLE_W + 1] = '_';  //Upper left 214
    buf_display[1 * CONSOLE_W + (CONSOLE_W - 2)] = '_';  //upper right 183
    buf_display[(CONSOLE_H - 2) * CONSOLE_W + 1] = 200;  //bottom left
    buf_display[(CONSOLE_H - 2) * CONSOLE_W + (CONSOLE_W - 2)] = 188; //bottom right
}

// Draw the falling tetromino
void DrawTetromino() {
    string tetromino = tetromino_types[cur_tetro_type][cur_tetro_orientation];
    // string tetromino = tetromino_types[0][2];
    stringstream t;
    t << "TYPE "<< cur_tetro_type << ", ORI" << cur_tetro_orientation;
    strcpy(&buf_display[50], t.str().c_str());
    for (int y = 0; y < TETRO_H; y++) {
        for (int x = 0; x < TETRO_W; x++) {
            char pixel = tetromino[y * TETRO_W + x];
            // pixel = RotateTetromino(tetromino, x, y, 90);
            if(pixel != '.' || true) {
                buf_display[(tetro_y + y) * CONSOLE_W + (tetro_x + x)] = pixel;
            }
        }
    }
    cur_tetro_orientation++;
    if(cur_tetro_orientation==4){
        cur_tetro_orientation = 0;
        cur_tetro_type++;
    }
    if(cur_tetro_type >= NUM_TETRO_TYPE ){
        cur_tetro_type = 0;
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

    }
}

int main(int argc, char* argv[]) {
    InitPlayField();
    StartGameLoop();

    return 0;
}
