#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>

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
void clearBuffer(char fillChar, WORD color);
void writeToBuffer(int x, int y, char ch, WORD color);
void writeStringToBuffer(int x, int y, const char* str, WORD color);
void writeWideStringToBuffer(int x, int y, const wchar_t* str, WORD color);
void flipBuffer();

void PortalPage();
void InGamePage();

void HowToPlayComponent();
void CarComponent(int x, int y, int color);
void drawTrees(int x, int y);

void printTitle(int x, int y);
void printHowToPlay(int x, int y);

// 색상 상수 정의
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
    
    // PortalPage();
    InGamePage();
    
    cleanupDoubleBuffer();
    return 0;
}

void Initialization() {
    // 커서 숨기기
    HANDLE hConsole;
    CONSOLE_CURSOR_INFO ConsoleCursor;

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    ConsoleCursor.bVisible = 0;
    ConsoleCursor.dwSize = 1;

    SetConsoleCursorInfo(hConsole, &ConsoleCursor);

    // 인코딩 변경
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    
    // 랜덤 시드 초기화
    srand((unsigned int)time(NULL));
}

// 더블버퍼 초기화
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
    
    // 버퍼 메모리 할당
    db.buffer = (CHAR_INFO*)malloc(WIDTH * HEIGHT * sizeof(CHAR_INFO));
    
    // 콘솔 설정
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(db.hConsole, &cursorInfo);
}

// 버퍼 클리어
void clearBuffer(char fillChar, WORD color) {
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        db.buffer[i].Char.AsciiChar = fillChar;
        db.buffer[i].Attributes = color;
    }
}

// 버퍼에 문자 쓰기
void writeToBuffer(int x, int y, char ch, WORD color) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        int index = y * WIDTH + x;
        db.buffer[index].Char.AsciiChar = ch;
        db.buffer[index].Attributes = color;
    }
}

// 버퍼에 문자열 쓰기
void writeStringToBuffer(int x, int y, const char* str, WORD color) {
    int len = strlen(str);
    for (int i = 0; i < len && (x + i) < WIDTH; i++) {
        writeToBuffer(x + i, y, str[i], color);
    }
}

// 와이드 문자열을 버퍼에 쓰기 (간단한 ASCII 변환)
void writeWideStringToBuffer(int x, int y, const wchar_t* str, WORD color) {
    int len = wcslen(str);
    for (int i = 0; i < len && (x + i) < WIDTH; i++) {
        char ch = (char)str[i]; // 간단한 변환
        writeToBuffer(x + i, y, ch, color);
    }
}

// 화면에 버퍼 출력
void flipBuffer() {
    WriteConsoleOutput(db.hConsole, db.buffer, db.bufferSize, 
                      db.bufferCoord, &db.writeRegion);
}

// 더블버퍼 정리
void cleanupDoubleBuffer() {
    free(db.buffer);
}

/************/
/** PAGES **/
/************/
void PortalPage() {
    clearBuffer(' ', COLOR_WHITE);
    printTitle(0, 5);
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
        // 버퍼 클리어
        clearBuffer(' ', COLOR_WHITE);

        // 나무 그리기 (스크롤 효과)
        drawTrees(0, treeOffset);
        drawTrees(68, treeOffset);
        
        // UI 컴포넌트들
        HowToPlayComponent();
        
        // 플레이어 차량들 그리기 (예시)
        for (int i = 1; i <= 6; i++) {
            CarComponent(8*i + ((i-1)*2), 13, COLOR_RED);
        }
        
        // 게임 정보 표시
        char gameInfoStr[100];
        sprintf(gameInfoStr, "Meter: %d | Speed: %d | Heart: %d", 
                gameInfo.meter, gameInfo.speed, gameInfo.heart);
        writeStringToBuffer(2, 1, gameInfoStr, COLOR_LIGHT_YELLOW);
        
        // 화면 업데이트
        flipBuffer();
        
        // 키 입력 처리
        if (_kbhit()) {
            int key = _getch();
            if (key == 13) { // Enter로 종료
                break;
            } else if (key == 'a' || key == 'A') {
                // 왼쪽 이동 로직
                gameInfo.meter++;
            } else if (key == 'd' || key == 'D') {
                // 오른쪽 이동 로직
                gameInfo.meter++;
            }
        }
        
        // 게임 속도 및 애니메이션 업데이트
        treeOffset = (treeOffset + 1) % HEIGHT;
        gameInfo.speed = (gameInfo.speed + 1) % 100;
        
        Sleep(100); // 프레임 조절
    }
}

/********************/
/** COMPONENTS **/
/********************/
void HowToPlayComponent() {
    int xStart = 75;
    int xEnd = 119;

    // 테두리 그리기
    writeToBuffer(xStart, 0, '+', COLOR_WHITE);
    for (int i = xStart+1; i < xEnd; i++) {
        writeToBuffer(i, 0, '-', COLOR_WHITE);
    }
    writeToBuffer(xEnd, 0, '+', COLOR_WHITE);

    for (int i = 1; i < HEIGHT-1; i++) {
        writeToBuffer(xStart, i, '|', COLOR_WHITE);
        writeToBuffer(xEnd, i, '|', COLOR_WHITE);
    }

    writeToBuffer(xStart, HEIGHT-1, '+', COLOR_WHITE);
    for (int i = xStart+1; i < xEnd; i++) {
        writeToBuffer(i, HEIGHT-1, '-', COLOR_WHITE);
    }
    writeToBuffer(xEnd, HEIGHT-1, '+', COLOR_WHITE);
    
    // How To 제목
    printHowToPlay(xStart+4, 2);
    
    // 설명 텍스트
    writeStringToBuffer(xStart+3, 9, "A, D key: Left, Right", COLOR_LIGHT_GREEN);
    writeStringToBuffer(xStart+3, 10, "Enter: Exit game", COLOR_LIGHT_GREEN);
    writeStringToBuffer(xStart+3, 11, "Avoid other cars!", COLOR_LIGHT_RED);
}

// 자동차 컴포넌트 (더블버퍼링 버전)
void CarComponent(int x, int y, int color) {
    // 차량 모양을 버퍼에 직접 그리기
    writeStringToBuffer(x, y-3, "  ____  ", color);
    writeStringToBuffer(x, y-2, " / ** \\", color);
    writeStringToBuffer(x, y-1, "/oo  oo\\", color);
    writeStringToBuffer(x, y,   " 0    0 ", color);
}

// 나무 그리기 (더블버퍼링 버전)
void drawTrees(int x, int y) {
    for (int i = 0; i < HEIGHT + 5; i += 5) {
        int treeY = (i + y) % HEIGHT;
        if (treeY >= 0 && treeY < HEIGHT - 4) {
            // 나무 그리기
            writeStringToBuffer(x, treeY,     "  /\\  ", COLOR_GREEN);
            writeStringToBuffer(x, treeY+1,   " /**\\ ", COLOR_GREEN);
            writeStringToBuffer(x, treeY+2,   "/****\\", COLOR_GREEN);
            writeStringToBuffer(x, treeY+3,   "  ||  ", COLOR_YELLOW);
        }
    }
}

/****************/
/** FUNCTIONS **/
/****************/
void printTitle(int x, int y) {
    // 타이틀을 더블버퍼에 그리기
    writeStringToBuffer(x, y,   "  /$$$$$$  /$$$$$$$         /$$$$$$   /$$$$$$  /$$$$$$$", COLOR_RED);
    writeStringToBuffer(x, y+1, " /$$__  $$| $$__  $$       /$$__  $$ /$$__  $$| $$__  $$", COLOR_RED);
    writeStringToBuffer(x, y+2, "|__/  \\ $$| $$  \\ $$      | $$  \\__/| $$  \\ $$| $$  \\ $$", COLOR_RED);
    writeStringToBuffer(x, y+3, "  /$$$$$$/| $$  | $$      | $$      | $$$$$$$$| $$$$$$$", COLOR_RED);
    writeStringToBuffer(x, y+4, " /$$____/ | $$  | $$      | $$      | $$__  $$| $$__  $$", COLOR_RED);
    writeStringToBuffer(x, y+5, "| $$      | $$  | $$      | $$    $$| $$  | $$| $$  \\ $$", COLOR_RED);
    writeStringToBuffer(x, y+6, "| $$$$$$$$| $$$$$$$/      |  $$$$$$/| $$  | $$| $$  | $$", COLOR_RED);
    writeStringToBuffer(x, y+7, "|________/|_______/        \\______/ |__/  |__/|__/  |__/", COLOR_RED);
}

void printHowToPlay(int x, int y) {
    writeStringToBuffer(x, y,   "  _    _                 _______", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+1, " | |  | |               |__   __|", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+2, " | |__| | _____      __    | | ___", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+3, " |  __  |/ _ \\ \\ /\\ / /    | |/ _ \\", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+4, " | |  | | (_) \\ V  V /     | | (_) |", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y+5, " |_|  |_|\\___/ \\_/\\_/      |_|\\___/", COLOR_LIGHT_BLUE);
}