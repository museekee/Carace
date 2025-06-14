#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define WIDTH 120
#define HEIGHT 29

// 제한 속도 준수 점수 추가
// 체력 아이템도 떨어지기
// 유저 선택적으로 가속하게 하자. 점수는 속도에 따라 하는거로 하고 <= (x*sqrt(x))/10 ㄱ.
// 생성된 차량을 랜덤적으로 배열에 추가 => 배열에 있는 걸 루프 돌리면서 y 내림.(speed와 함께) => 만약 y가 HEIGHT인가 그 정도라면 pop시키기.
#pragma region 구조체 선언
typedef struct _NPC {
    int color; // 당연히 고정
    int x; // 당연히 고정
    int y; // 얘는 고정으로 ㄱ. 그냥 내려가는 거 기록하는 게 있을텐데 그걸로 빼기 잘 해서 구하기 ㄱ
} NPC;

struct GameInfo {
    int meter;
    int speed; // 최소 10
    double score; // 점수
    int heart;
    int playerLane;
} gameInfo = { 0, 10, 0.0, 3, 2 };

const int lanes[] = { 8, 18, 28, 38, 48, 58 };
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

typedef struct RenderTimerParam {
    int treeOffset;
}  RenderTimerParam;
#pragma endregion

#pragma region 함수 선언
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

void RenderTimerCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired);
void CalculateScoreTimerCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired);

void HowToPlayComponent();
void CarComponent(int x, int y, int color);
void drawTrees(int x, int y);
void drawLanes(int x, int y, bool isYellow);

void printTitle(int x, int y);
void printHowToPlay(int x, int y);
void printInfo(int x, int y);
void calculateScore();

void movePlayer(int diection);
void addSpeed(int value);
#pragma endregion

// https://dimigo.goorm.io/qna/22465 이런 색상은 어떄요?
#pragma region 색상
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
#pragma endregion

#pragma region 메인함수와 초기화
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

    // UTF-8 코드페이지 설정
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    srand(time(NULL));
}
#pragma endregion

#pragma region 더블버퍼링 설정
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

// 캐릭터 하나
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
    if (str == NULL) return;
    
    int len = wcslen(str);
    for (int i = 0; i < len && (x + i) < WIDTH; i++) {
        if (x + i >= 0 && y >= 0 && y < HEIGHT) {
            writeToBuffer(x + i, y, str[i], color);
        }
    }
}

void flipBuffer() {
    WriteConsoleOutputW(db.hConsole, db.buffer, db.bufferSize,
                       db.bufferCoord, &db.writeRegion);
}
void cleanupDoubleBuffer() {
    free(db.buffer);
}
#pragma endregion

#pragma region 페이지
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
    HANDLE renderTimer = NULL;
    HANDLE calculateScoreTimer = NULL;
    
    // 키 상태 추적 변수들 (W/S만 필요)
    bool wPressed = false;
    bool sPressed = false;

    RenderTimerParam renderTimerParam = { 0 };
    int renderTimerDueTime = 111 - gameInfo.speed;
    CreateTimerQueueTimer(&renderTimer, NULL, RenderTimerCallback, &renderTimerParam, 0, renderTimerDueTime, 0);

    CreateTimerQueueTimer(&calculateScoreTimer, NULL, CalculateScoreTimerCallback, NULL, 0, 500, 0);

    while (1) {
        DWORD currentTime = GetTickCount();
        int oldSpeed = gameInfo.speed;

        if (_kbhit()) {
            int key = _getch();
            switch (key) {
            case 13:
                DeleteTimerQueueTimer(NULL, renderTimer, NULL);
                DeleteTimerQueueTimer(NULL, calculateScoreTimer, NULL);
                return;
            case 'A':
            case 'a':
                movePlayer(0);
                break;
            case 'D':
            case 'd':
                movePlayer(1);
                break;
            case 'W':
            case 'w':
                if (!wPressed) {
                    addSpeed(5);
                    wPressed = true;
                    
                    if (oldSpeed != gameInfo.speed) {
                        renderTimerDueTime = 111 - gameInfo.speed;
                        ChangeTimerQueueTimer(NULL, renderTimer, renderTimerDueTime, renderTimerDueTime);
                    }
                }
                break;
            case 'S':
            case 's':
                if (!sPressed) {
                    addSpeed(-5);
                    sPressed = true;
                    
                    if (oldSpeed != gameInfo.speed) {
                        renderTimerDueTime = 111 - gameInfo.speed;
                        ChangeTimerQueueTimer(NULL, renderTimer, renderTimerDueTime, renderTimerDueTime);
                    }
                }
                break;
            }
        }

        if (!(GetAsyncKeyState('W') & 0x8000) && !(GetAsyncKeyState('w') & 0x8000)) {
            wPressed = false;
        }
        if (!(GetAsyncKeyState('S') & 0x8000) && !(GetAsyncKeyState('s') & 0x8000)) {
            sPressed = false;
        }

        // 버퍼 변경
        flipBuffer();
        
        Sleep(1);
    }
}
#pragma endregion

#pragma region 타이머 콜백
/*************************/
/** TIMER CALLBACKS **/
/************************/
void RenderTimerCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired) {
    RenderTimerParam* data = (RenderTimerParam*)lpParam;

    clearBuffer(L' ', COLOR_WHITE);

    // 나무 렌더링
    drawTrees(0, data->treeOffset);
    drawTrees(68, data->treeOffset);

    // 오른쪽 렌더링
    HowToPlayComponent();

    // 차선 렌더링
    drawLanes(lanes[0] - 2, data->treeOffset, true);
    drawLanes(lanes[0] + laneOffset, data->treeOffset, false);
    drawLanes(lanes[1] + laneOffset, data->treeOffset, false);
    drawLanes(lanes[2] + laneOffset, data->treeOffset, false);
    drawLanes(lanes[3] + laneOffset, data->treeOffset, false);
    drawLanes(lanes[4] + laneOffset, data->treeOffset, false);
    drawLanes(lanes[4] + laneOffset * 2 + 2, data->treeOffset, true);

    // 플레이어 렌더링
    CarComponent(lanes[gameInfo.playerLane], 25, COLOR_RED);

    data->treeOffset = (data->treeOffset + 1) % HEIGHT;
}

void CalculateScoreTimerCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired) {
    calculateScore();
}
#pragma endregion

#pragma region 컴포넌트
/********************/
/** COMPONENTS **/
/********************/
void HowToPlayComponent() {
    int xStart = 75;
    int xEnd = 119;

    // 테두리 그리기
    writeToBuffer(xStart, 0, L'┌', COLOR_GRAY);
    for (int i = xStart+1; i < xEnd; i++) {
        writeToBuffer(i, 0, L'─', COLOR_GRAY);
    }
    writeToBuffer(xEnd, 0, L'┐', COLOR_GRAY);

    for (int i = 1; i < HEIGHT-1; i++) {
        writeToBuffer(xStart, i, L'│', COLOR_GRAY);
        writeToBuffer(xEnd, i, L'│', COLOR_GRAY);
    }

    writeToBuffer(xStart, HEIGHT-1, L'└', COLOR_GRAY);
    for (int i = xStart+1; i < xEnd; i++) {
        writeToBuffer(i, HEIGHT-1, L'─', COLOR_GRAY);
    }
    writeToBuffer(xEnd, HEIGHT-1, L'┘', COLOR_GRAY);

    printHowToPlay(xStart+4, 2);

    writeWideStringToBuffer(xStart+3, 9, L"• A, D키를 이용해 좌/우로 움직인다.", COLOR_LIGHT_GREEN);
    writeWideStringToBuffer(xStart+3, 10, L"• W, S키를 이용해 가/감속 한다.", COLOR_LIGHT_GREEN);
    
    printInfo(xStart+11, 12);

    wchar_t scoreWstr[50];
    swprintf(scoreWstr, 50, L"• 점수: %d",
            (int)gameInfo.score);
    writeWideStringToBuffer(xStart+3, 19, scoreWstr, COLOR_LIGHT_YELLOW);

    wchar_t speedWstr[50];
    swprintf(speedWstr, 50, L"• 속도: %dkm/h",
            gameInfo.speed);
    writeWideStringToBuffer(xStart+3, 20, speedWstr, COLOR_LIGHT_YELLOW);

    wchar_t hearts[] = L"♡ ♡ ♡ ♡ ♡  ";
    writeWideStringToBuffer(xStart+3, 21, L"• 하트: ", COLOR_LIGHT_YELLOW);
    for (int i = 0; i < gameInfo.heart*2; i+=2) {
        hearts[i] = L'❤';
    }
    writeWideStringToBuffer(xStart+9, 21, hearts, COLOR_RED);
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
        writeWideStringToBuffer(x, laneY, isYellow ? L"││" : L"┌┐", color);
        writeWideStringToBuffer(x, (laneY + 1) % HEIGHT, L"││", color);
        writeWideStringToBuffer(x, (laneY + 2) % HEIGHT, L"││", color);
        writeWideStringToBuffer(x, (laneY + 3) % HEIGHT, isYellow ? L"││" : L"└┘", color);
        writeWideStringToBuffer(x, (laneY + 4) % HEIGHT, isYellow ? L"││" : L"  ", color);
    }
}
#pragma endregion

#pragma region 함수 정의
/****************/
/** FUNCTIONS **/
/****************/
// https://patorjk.com/software/taag/#p=display&f=Big%20Money-ne
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

// https://patorjk.com/software/taag/#p=display&f=Big
void printHowToPlay(int x, int y) {
    writeStringToBuffer(x, y,   "  _    _                 _______", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+1, " | |  | |               |__   __|", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+2, " | |__| | _____      __    | | ___", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+3, " |  __  |/ _ \\ \\ /\\ / /    | |/ _ \\", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+4, " | |  | | (_) \\ V  V /     | | (_) |", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+5, " |_|  |_|\\___/ \\_/\\_/      |_|\\___/", COLOR_LIGHT_BLUE);
}

void printInfo(int x, int y) {
    writeStringToBuffer(x, y,   "  _____        __      ", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+1, " |_   _|      / _|     ", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+2, "   | |  _ __ | |_ ___  ", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+3, "   | | | '_ \\|  _/ _ \\ ", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+4, "  _| |_| | | | || (_) |", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+5, " |_____|_| |_|_| \\___/ ", COLOR_LIGHT_BLUE);
}

void calculateScore() {
    double y_values[] = { // { 10 <= x <= 110 }의 y값. y=1.45\cdot e^{\left(0.032\cdot x\right)}\cdot2\ \left\{10\ \le\ x\ \le\ 110\ \right\} (desmos임)
        3.863145, 4.556196, 5.143086, 5.812064, 6.574221,
        7.442174, 8.430248, 9.554559, 10.833014, 12.285258,
        13.932561, 15.798666, 17.909581, 20.293312, 22.980529,
        26.004098, 29.399672, 33.205321, 37.461183, 42.209138,
        47.493501
    };
    gameInfo.score += y_values[(gameInfo.speed - 10) / 5];
}
#pragma endregion

#pragma region 컨트롤러
/********************/
/** CONTROLLERS **/
/********************/

// { direction } 0: Left, 1: Right
void movePlayer(int direction) {
    if (direction == 0 && gameInfo.playerLane > 0)
        gameInfo.playerLane--;
    else if (direction == 1 && gameInfo.playerLane < 5)
        gameInfo.playerLane++;
}
void addSpeed(int value) {
    int* speed = &(gameInfo.speed);
    
    if (value < 0) {
        // 감속: 최소 속도 10까지
        int newSpeed = *speed + value;
        if (newSpeed >= 10) {
            *speed = newSpeed;
        } else {
            *speed = 10;
        }
    } else if (value > 0) {
        // 가속: 최대 속도 110까지
        int newSpeed = *speed + value;
        if (newSpeed <= 110) {
            *speed = newSpeed;
        } else {
            *speed = 110;
        }
    }
}
#pragma endregion
