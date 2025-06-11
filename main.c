#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define WIDTH 120
#define HEIGHT 29

// 생성된 차량을 랜덤적으로 배열에 추가 => 배열에 있는 걸 루프 돌리면서 y 내림.(speed와 함께) => 만약 y가 HEIGHT인가 그 정도라면 pop시키기.
typedef struct _NPC {
    int color; // 당연히 고정
    int x; // 당연히 고정
    int y; // 얘는 고정으로 ㄱ. 그냥 내려가는 거 기록하는 게 있을텐데 그걸로 빼기 잘 해서 구하기 ㄱ
} NPC;

struct GameInfo {
    int meter;
    int speed;
    int heart;
} gameInfo = { 0, 0, 5 };

int lanes[] = { 8, 18, 28, 38, 48, 58 };
const int laneOffset = 8;

// 더블버퍼링을 위한 구조체
typedef struct {
    HANDLE hConsole;
    CHAR_INFO* buffer;
    COORD bufferSize;
    COORD bufferCoord;
    SMALL_RECT writeRegion;
} DoubleBuffer;

DoubleBuffer db;

// 함수 선언
void Initialization();
void initDoubleBuffer();
void cleanupDoubleBuffer();
void clearBuffer(wchar_t fillChar, WORD color);
void writeToBuffer(int x, int y, wchar_t ch, WORD color);
void writeCharToBuffer(int x, int y, char ch, WORD color);
void writeStringToBuffer(int x, int y, const char* str, WORD color);
void writeWideStringToBuffer(int x, int y, const wchar_t* str, WORD color);
void flipBuffer();

void PortalPage();
void InGamePage();

void HowToPlayComponent();
void CarComponent(int x, int y, int color);
void drawTrees(int x, int y);
void drawLanes(int x, int y, bool isYellow);

void printTitle(int x, int y);
void printHowToPlay(int x, int y);

// https://dimigo.goorm.io/qna/22465 이런 색상은 어떄요?
// 색상
#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_AQUA 3
#define COLOR_RED 4
#define COLOR_PURPLE 5
#define COLOR_YELLOW 6
#define COLOR_WHITE 7
#define COLOR_GRAY 8
#define COLOR_LIGHT_BLUE 9
#define COLOR_LIGHT_GREEN 10
#define COLOR_LIGHT_AQUA 11
#define COLOR_LIGHT_RED 12
#define COLOR_LIGHT_PURPLE 13
#define COLOR_LIGHT_YELLOW 14
#define COLOR_BRIGHT_WHITE 15

int main() {
    Initialization();
    initDoubleBuffer();

    PortalPage();
    InGamePage();

    cleanupDoubleBuffer();
    return 0;
}

void Initialization() {
    HANDLE hConsole;
    CONSOLE_CURSOR_INFO ConsoleCursor;

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    ConsoleCursor.bVisible = 0;
    ConsoleCursor.dwSize = 1;

    SetConsoleCursorInfo(hConsole, &ConsoleCursor);

    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    srand(time(NULL));
}

void initDoubleBuffer() {
    db.hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    db.bufferSize.X = WIDTH;
    db.bufferSize.Y = HEIGHT;
    db.bufferCoord.X = 0;
    db.bufferCoord.Y = 0;
    db.writeRegion.Left = 0;
    db.writeRegion.Top = 0;
    db.writeRegion.Right = WIDTH - 1;
    db.writeRegion.Bottom = HEIGHT - 1;

    db.buffer = (CHAR_INFO*)malloc(WIDTH * HEIGHT * sizeof(CHAR_INFO));

    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(db.hConsole, &cursorInfo);
}

void clearBuffer(wchar_t fillChar, WORD color) {
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        db.buffer[i].Char.UnicodeChar = fillChar;
        db.buffer[i].Attributes = color;
    }
}

void writeToBuffer(int x, int y, wchar_t ch, WORD color) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        int index = y * WIDTH + x;
        db.buffer[index].Char.UnicodeChar = ch;
        db.buffer[index].Attributes = color;
    }
}

void writeCharToBuffer(int x, int y, char ch, WORD color) {
    writeToBuffer(x, y, (wchar_t)ch, color);
}

void writeStringToBuffer(int x, int y, const char* str, WORD color) {
    int len = strlen(str);
    for (int i = 0; i < len && (x + i) < WIDTH; i++) {
        writeCharToBuffer(x + i, y, str[i], color);
    }
}

void writeWideStringToBuffer(int x, int y, const wchar_t* str, WORD color) {
    int len = wcslen(str);
    for (int i = 0; i < len && (x + i) < WIDTH; i++) {
        writeToBuffer(x + i, y, str[i], color);
    }
}

void flipBuffer() {
    WriteConsoleOutput(db.hConsole, db.buffer, db.bufferSize,
                      db.bufferCoord, &db.writeRegion);
}

void cleanupDoubleBuffer() {
    free(db.buffer);
}

/************/
/** PAGES **/
/************/
void PortalPage() {
    clearBuffer(' ', COLOR_WHITE);
    printTitle(1, 5);
    writeStringToBuffer(47, 20, "[PRESS 'ENTER' TO START]", COLOR_WHITE);
    flipBuffer();

    while (1) {
        int key = _getch();
        if (key == 13) {
            break;
        }
    }
}

void InGamePage() {
    int treeOffset = 0;

    while (1) {
        clearBuffer(L' ', COLOR_WHITE);

        drawTrees(0, treeOffset);
        drawTrees(68, treeOffset);

        HowToPlayComponent();

        for (int i = 1; i <= 6; i++) {
            // CarComponent(8*i + ((i-1)*2), 13, COLOR_RED);
            CarComponent(lanes[i-1], 13, COLOR_RED);
        }
        drawLanes(lanes[0] - 2, treeOffset, true);
        drawLanes(lanes[0] + laneOffset, treeOffset, false);
        drawLanes(lanes[1] + laneOffset, treeOffset, false);
        drawLanes(lanes[2] + laneOffset, treeOffset, false);
        drawLanes(lanes[3] + laneOffset, treeOffset, false);
        drawLanes(lanes[4] + laneOffset, treeOffset, false);
        drawLanes(lanes[4] + laneOffset * 2 + 2, treeOffset, true);

        flipBuffer();

        if (_kbhit()) {
            int key = _getch();
            if (key == 13) {
                break;
            }
        }

        treeOffset = (treeOffset + 1) % HEIGHT;
        gameInfo.speed = (gameInfo.speed + 1) % 100;

        Sleep(50);
    }
}

/********************/
/** COMPONENTS **/
/********************/
void HowToPlayComponent() {
    int xStart = 75;
    int xEnd = 119;

    // 테두리 그리기
    writeToBuffer(xStart, 0, L'+', COLOR_GRAY);
    for (int i = xStart+1; i < xEnd; i++) {
        writeToBuffer(i, 0, L'-', COLOR_GRAY);
    }
    writeToBuffer(xEnd, 0, L'+', COLOR_GRAY);

    for (int i = 1; i < HEIGHT-1; i++) {
        writeToBuffer(xStart, i, L'|', COLOR_GRAY);
        writeToBuffer(xEnd, i, L'|', COLOR_GRAY);
    }

    writeToBuffer(xStart, HEIGHT-1, L'+', COLOR_GRAY);
    for (int i = xStart+1; i < xEnd; i++) {
        writeToBuffer(i, HEIGHT-1, L'-', COLOR_GRAY);
    }
    writeToBuffer(xEnd, HEIGHT-1, L'+', COLOR_GRAY);

    printHowToPlay(xStart+4, 2);

    writeStringToBuffer(xStart+3, 9, " - A: Left / D: Right", COLOR_LIGHT_GREEN);
}

// 자동차 컴포넌트
void CarComponent(int x, int y, int color) {
    writeStringToBuffer(x, y-3, "  ____  ", color);
    writeStringToBuffer(x, y-2, " / ** \\", color);
    writeStringToBuffer(x, y-1, "/oo  oo\\", color);
    writeStringToBuffer(x, y,   " 0    0 ", color);
}

// 나무 그리기
void drawTrees(int x, int y) {
    for (int i = 0; i < HEIGHT + 5; i += 5) {
        int treeY = (i + y) % HEIGHT;
        writeStringToBuffer(x, treeY,     "  /\\  ", COLOR_GREEN);
        writeStringToBuffer(x, (treeY + 1) % HEIGHT,   " /**\\ ", COLOR_GREEN);
        writeStringToBuffer(x, (treeY + 2) % HEIGHT,   "/****\\", COLOR_GREEN);
        writeStringToBuffer(x, (treeY + 3) % HEIGHT,   "  ||  ", COLOR_YELLOW);
        writeStringToBuffer(x, (treeY + 4) % HEIGHT,   "    ", COLOR_YELLOW);
    }
}
void drawLanes(int x, int y, bool isYellow) {
    for (int i = 0; i < HEIGHT + 5; i += 5) {
        int laneY = (i + y) % HEIGHT;
        int color = isYellow ? COLOR_YELLOW : COLOR_BRIGHT_WHITE;
        writeStringToBuffer(x, laneY,     "||", color);
        writeStringToBuffer(x, (laneY + 1) % HEIGHT,   "||", color);
        writeStringToBuffer(x, (laneY + 2) % HEIGHT,   "||", color);
        writeStringToBuffer(x, (laneY + 3) % HEIGHT,   "||", color);
        writeStringToBuffer(x, (laneY + 4) % HEIGHT,   isYellow ? "||" : "  ", color);
    }
}

/****************/
/** FUNCTIONS **/
/****************/
void printTitle(int x, int y) {
    writeStringToBuffer(x, y,   "  /$$$$$$  /$$$$$$$         /$$$$$$   /$$$$$$  /$$$$$$$        /$$$$$$$   /$$$$$$   /$$$$$$  /$$$$$$ /$$   /$$  /$$$$$$", COLOR_RED);
    writeStringToBuffer(x, y+1, " /$$__  $$| $$__  $$       /$$__  $$ /$$__  $$| $$__  $$      | $$__  $$ /$$__  $$ /$$__  $$|_  $$_/| $$$ | $$ /$$__  $$", COLOR_RED);
    writeStringToBuffer(x, y+2, "|__/  \\ $$| $$  \\ $$      | $$  \\__/| $$  \\ $$| $$  \\ $$      | $$  \\ $$| $$  \\ $$| $$  \\__/  | $$  | $$$$| $$| $$  \\__/", COLOR_RED);
    writeStringToBuffer(x, y+3, "  /$$$$$$/| $$  | $$      | $$      | $$$$$$$$| $$$$$$$/      | $$$$$$$/| $$$$$$$$| $$        | $$  | $$ $$ $$| $$ /$$$$", COLOR_RED);
    writeStringToBuffer(x, y+4, " /$$____/ | $$  | $$      | $$      | $$__  $$| $$__  $$      | $$__  $$| $$__  $$| $$        | $$  | $$  $$$$| $$|_  $$", COLOR_RED);
    writeStringToBuffer(x, y+5, "| $$      | $$  | $$      | $$    $$| $$  | $$| $$  \\ $$      | $$  \\ $$| $$  | $$| $$    $$  | $$  | $$\\  $$$| $$  \\ $$", COLOR_RED);
    writeStringToBuffer(x, y+6, "| $$$$$$$$| $$$$$$$/      |  $$$$$$/| $$  | $$| $$  | $$      | $$  | $$| $$  | $$|  $$$$$$/ /$$$$$$| $$ \\  $$|  $$$$$$/", COLOR_RED);
    writeStringToBuffer(x, y+7, "|________/|_______/        \\______/ |__/  |__/|__/  |__/      |__/  |__/|__/  |__/ \\______/ |______/|__/  \\__/ \\______/ ", COLOR_RED);
}

void printHowToPlay(int x, int y) {
    writeStringToBuffer(x, y,   "  _    _                 _______", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+1, " | |  | |               |__   __|", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+2, " | |__| | _____      __    | | ___", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+3, " |  __  |/ _ \\ \\ /\\ / /    | |/ _ \\", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+4, " | |  | | (_) \\ V  V /     | | (_) |", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+5, " |_|  |_|\\___/ \\_/\\_/      |_|\\___/", COLOR_LIGHT_BLUE);
}
