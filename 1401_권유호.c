#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define WIDTH 120
#define HEIGHT 29

#define PLAYER_Y 25

// https://dimigo.goorm.io/qna/22465 이런 색상은 어때요?
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

#define SAFE_SWPRINTF(buf, size, format, ...) swprintf(buf, format, __VA_ARGS__)

#pragma region 열거형 선언
typedef enum
{
    Heart,
    Car
} NPCType;
#pragma endregion

#pragma region 차선 위치
const int lanes[] = {8, 18, 28, 38, 48, 58};
const int laneOffset = 8; // 차'로'의 크기
int maxLane = 5;          // 차선은 오른쪽이 점점 사라지는 방식으로 갈거임.
#pragma endregion

#pragma region 글자 아스키 아트
const char Grade_labels[][8][12] = {
    {
        "  /$$$$$$  ",
        " /$$__  $$ ",
        "| $$  \\__/ ",
        "|  $$$$$$  ",
        " \\____  $$ ",
        " /$$  \\ $$ ",
        "|  $$$$$$/ ",
        " \\______/ ",
    },
    {
        "  /$$$$$$  ",
        " /$$__  $$ ",
        "| $$  \\ $$ ",
        "| $$$$$$$$ ",
        "| $$__  $$ ",
        "| $$  | $$ ",
        "| $$  | $$ ",
        "|__/  |__/ ",
    },
    {
        " /$$$$$$$  ",
        "| $$__  $$ ",
        "| $$  \\ $$ ",
        "| $$$$$$$  ",
        "| $$__  $$ ",
        "| $$  \\ $$ ",
        "| $$$$$$$/ ",
        "|_______/  ",
    },
    {
        "  /$$$$$$  ",
        " /$$__  $$ ",
        "| $$  \\__/ ",
        "| $$       ",
        "| $$       ",
        "| $$    $$ ",
        "|  $$$$$$/ ",
        " \\______/ ",
    },
    {
        " /$$$$$$$  ",
        "| $$__  $$ ",
        "| $$  \\ $$ ",
        "| $$  | $$ ",
        "| $$  | $$ ",
        "| $$  | $$ ",
        "| $$$$$$$/ ",
        "|_______/  ",
    },
    {
        " /$$$$$$$$",
        "| $$_____/ ",
        "| $$       ",
        "| $$$$$    ",
        "| $$__/    ",
        "| $$       ",
        "| $$       ",
        "|__/       ",
    },

};

const int Grade_colors[] = {
    COLOR_LIGHT_AQUA,
    COLOR_LIGHT_GREEN,
    COLOR_GREEN,
    COLOR_LIGHT_YELLOW,
    COLOR_YELLOW,
    COLOR_RED,
};

const char Number_labels[][8][11] = {
    {
        "  /$$$$$$ ",
        " /$$    $$",
        "| $$    $$",
        "| $$    $$",
        "| $$    $$",
        "| $$    $$",
        "|  $$$$$$/",
        " \\______/ ",
    },
    {
        "   /$$    ",
        "  /$$$$   ",
        " |_  $$   ",
        "   | $$   ",
        "   | $$   ",
        "   | $$   ",
        "  /$$$$$$ ",
        " |______/ ",
    },
    {
        "  /$$$$$$ ",
        " /$$__  $$",
        "|__/  \\ $$",
        "  /$$$$$$/",
        " /$$____/ ",
        "| $$      ",
        "| $$$$$$$$",
        "|________/",
    },
    {
        "  /$$$$$$ ",
        " /$$__  $$",
        "|__/  \\ $$",
        "   /$$$$$/",
        "  |___  $$",
        " /$$  \\ $$",
        "|  $$$$$$/",
        " \\______/ ",
    },
    {
        " /$$   /$$",
        "| $$  | $$",
        "| $$  | $$",
        "| $$$$$$$$",
        "|_____  $$",
        "      | $$",
        "      | $$",
        "      |__/",
    },
    {
        " /$$$$$$$ ",
        "| $$____/ ",
        "| $$      ",
        "| $$$$$$$ ",
        "|_____  $$",
        " /$$  \\ $$",
        "|  $$$$$$/",
        " \\______/ ",
    },
    {
        "  /$$$$$$ ",
        " /$$__  $$",
        "| $$  \\__/",
        "| $$$$$$$ ",
        "| $$__  $$",
        "| $$  \\ $$",
        "|  $$$$$$/",
        " \\______/ ",
    },
    {
        " /$$$$$$$$",
        "|_____ $$/",
        "      /$$/",
        "     /$$/ ",
        "    /$$/  ",
        "   /$$/   ",
        "  /$$/    ",
        " |__/     ",
    },
    {
        "  /$$$$$$ ",
        " /$$__  $$",
        "| $$  \\ $$",
        "|  $$$$$$/",
        " >$$__  $$",
        "| $$  \\ $$",
        "|  $$$$$$/",
        " \\______/ ",
    },
    {
        "  /$$$$$$ ",
        " /$$__  $$",
        "| $$  \\ $$",
        "|  $$$$$$$",
        " \\____  $$",
        " /$$  \\ $$",
        "|  $$$$$$/",
        " \\______/ ",
    },
};
#pragma endregion

#pragma region 구조체 선언
typedef struct _NPC
{
    NPCType type; // NPC의 종류 (Heart, Car)
    int color;    // 당연히 고정
    int x;        // 당연히 고정
    int y;        // 얘는 고정으로 ㄱ. 그냥 내려가는 거 기록하는 게 있을텐데 그걸로 빼기 잘 해서 구하기 ㄱ <= 25년 6월 14일의 나: 고정 안 함 ㅅㄱ
} NPC;

struct GameInfo
{
    int speed;      // 최소 10
    double score;   // 점수
    int heart;      // 체력
    int playerLane; // 플레이어가 있는 차선
    NPC npcs[50];   // NPC 배열
    int npcCount;   // 현재 NPC 개수
} gameInfo = {10, 0.0, 5, 2, {0}, 0};

// 더블버퍼링을 위한 구조체
typedef struct
{
    HANDLE hConsole;
    CHAR_INFO *buffer;
    COORD bufferSize;
    COORD bufferCoord;
    SMALL_RECT writeRegion;
} DoubleBuffer;

DoubleBuffer db;

typedef struct RenderTimerParam
{
    int treeOffset;
} RenderTimerParam;
#pragma endregion

#pragma region 함수 선언
void Initialization(); // 초기화
void GameInitialization(); // 게임 데이터만 초기화
void initDoubleBuffer();
void cleanupDoubleBuffer();
void clearBuffer(wchar_t fillChar, WORD color);
void writeToBuffer(int x, int y, wchar_t ch, WORD color);
void writeCharToBuffer(int x, int y, char ch, WORD color);
void writeStringToBuffer(int x, int y, const char *str, WORD color);
void writeWideStringToBuffer(int x, int y, const wchar_t *str, WORD color);
void flipBuffer();

void PortalPage(); // 포털 페이지
void InGamePage(); // 게임 페이지
void ScorePage();  // 점수 페이지

VOID CALLBACK RenderTimerCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired);         // 렌더링 타이머 콜백
VOID CALLBACK CalculateScoreTimerCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired); // 점수 계산 타이머 콜백
VOID CALLBACK CreateNPCTimerCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired);      // NPC 생성 타이머 콜백
void MoveNPC();                                                                     // NPC 이동
void RenderNPC();                                                                   // NPC 렌더링

void HowToPlayComponent();                             // 게임 방법 컴포넌트
void CarComponent(int x, int y, int color);            // 자동차 컴포넌트
void HeartComponent(int x, int y, int color);          // 하트 컴포넌트
void drawTrees(int x, int y);                          // 나무 그리기
void drawLines(int x, int y, int color, int lineType); // 차선 그리기
void drawForbiddenLines();                             // 이제 막힌 차로 색칠하기

void gotoxy(int x, int y);                // 커서 위치 이동
int getRandom(int max);                   // 랜덤 숫자 생성
void printTitle(int x, int y);            // 타이틀 출력
void printHowToPlay(int x, int y);        // 게임 방법 출력
void printInfo(int x, int y);             // 정보 출력
void printGrade(int x, int y, int color); // 등급 출력
void printScore(int x, int y, int color); // 점수 출력
void calculateScore();                    // 점수 계산
void removeNPCByIndex(int index);         // NPC 제거
NPCType WhatIsThisNPC();                  // NPC 종류 결정
void setMaxLane(int lane);                // 차로 줄이기

void movePlayer(int direction); // 플레이어 이동
void addSpeed(int value);       // 속도 추가
#pragma endregion

#pragma region 메인함수와 초기화
bool iWillRestart = false;
int main()
{
    Initialization();
    initDoubleBuffer();

    PortalPage();
RESTART:
    iWillRestart = false;
    GameInitialization();
    InGamePage();
    ScorePage();

    if (iWillRestart) goto RESTART;
    cleanupDoubleBuffer();

    return 0;
}
float recentScore;
char recentGrade;
bool isWindowsTerminal;
// WindowsTerminal 구분하는거라고 함.
BOOL IsWindowsTerminal() {
    // 1. 환경변수 확인 (가장 확실)
    if (getenv("WT_SESSION") != NULL) {
        return TRUE;
    }

    // 2. 콘솔 모드 확인
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    if (GetConsoleMode(hConsole, &mode)) {
        if (mode & 0x0004) {
            // 3. 콘솔 창 제목 추가 확인
            HWND consoleWindow = GetConsoleWindow();
            if (consoleWindow != NULL) {
                char title[256];
                if (GetWindowTextA(consoleWindow, title, sizeof(title)) > 0) {
                    // 구형 cmd는 보통 "C:\Windows\System32\cmd.exe" 형식
                    if (strstr(title, "cmd.exe") != NULL &&
                        strstr(title, "Windows Terminal") == NULL) {
                        return FALSE;
                    }
                }
                return TRUE;
            }
        }
    }

    return FALSE;
}
void Initialization()
{
    HANDLE hConsole;
    CONSOLE_CURSOR_INFO ConsoleCursor;

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    ConsoleCursor.bVisible = 0;
    ConsoleCursor.dwSize = 1;

    SetConsoleCursorInfo(hConsole, &ConsoleCursor);
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    isWindowsTerminal = IsWindowsTerminal();
}
void GameInitialization() {
    clearBuffer(L' ', COLOR_WHITE);
    gameInfo.heart = 5;
    gameInfo.npcCount = 0;
    memset(gameInfo.npcs, 0, sizeof(gameInfo.npcs));
    gameInfo.playerLane = 2;
    gameInfo.score = 0;
    gameInfo.speed = 0;
    maxLane = 5;
    FILE *file = _wfopen(L"./1401_권유호.txt", L"r, ccs=UTF-8");
    // fscanf(fp1, "점수: %f점 ", &recentScore);
    fwscanf(file, L"점수: %f점\n등급: %c\n", &recentScore, &recentGrade);
    // fscanf(fp1, "등급: %c", &recentGrade);
    fclose(file);
}
#pragma endregion

#pragma region 더블버퍼링 설정
void initDoubleBuffer()
{
    db.hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    db.bufferSize.X = WIDTH;
    db.bufferSize.Y = HEIGHT;
    db.bufferCoord.X = 0;
    db.bufferCoord.Y = 0;
    db.writeRegion.Left = 0;
    db.writeRegion.Top = 0;
    db.writeRegion.Right = WIDTH - 1;
    db.writeRegion.Bottom = HEIGHT - 1;

    db.buffer = (CHAR_INFO *)malloc(WIDTH * HEIGHT * sizeof(CHAR_INFO));

    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(db.hConsole, &cursorInfo);
}

void clearBuffer(wchar_t fillChar, WORD color)
{
    for (int i = 0; i < WIDTH * HEIGHT; i++)
    {
        db.buffer[i].Char.UnicodeChar = fillChar;
        db.buffer[i].Attributes = color;
    }
}

// 캐릭터 하나
void writeToBuffer(int x, int y, wchar_t ch, WORD color)
{
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
    {
        int index = y * WIDTH + x;
        db.buffer[index].Char.UnicodeChar = ch;
        db.buffer[index].Attributes = color;
    }
}

void writeCharToBuffer(int x, int y, char ch, WORD color)
{
    writeToBuffer(x, y, (wchar_t)ch, color);
}

void writeStringToBuffer(int x, int y, const char *str, WORD color)
{
    int len = strlen(str);
    for (int i = 0; i < len && (x + i) < WIDTH; i++)
    {
        writeCharToBuffer(x + i, y, str[i], color);
    }
}

void writeWideStringToBuffer(int x, int y, const wchar_t *str, WORD color)
{
    if (str == NULL)
        return;

    int len = wcslen(str);

    int nujeokX = 0;
    for (int i = 0; i < len && (x + i) < WIDTH; i++)
    {
        if (x + i >= 0 && y >= 0 && y < HEIGHT)
        {
            bool isFullWidth = str[i] >= 0xAC00 && str[i] <= 0xD7AF; // 내가쓰는 전각은 한글뿐이니까 한글 범위에 한정.
            writeToBuffer(x + nujeokX, y, str[i], color);
            if (isWindowsTerminal)
                nujeokX++;
            else
                nujeokX += isFullWidth ? 2 : 1;
        }
    }
}

void flipBuffer()
{
    WriteConsoleOutputW(db.hConsole, db.buffer, db.bufferSize,
                        db.bufferCoord, &db.writeRegion);
}
void cleanupDoubleBuffer()
{
    free(db.buffer);
}
#pragma endregion

#pragma region 페이지
/************/
/** PAGES **/
/************/
void PortalPage()
{
    clearBuffer(' ', COLOR_WHITE);
    printTitle(1, 5);
    writeWideStringToBuffer(32, 16, L"[TIP]: cmd가 아닌 신형 Windows Terminal로 실행하면", COLOR_WHITE);
    writeWideStringToBuffer(39, 17, L"더 빠르고 쾌적한 환경에서 플레이할 수 있습니다.", COLOR_WHITE);
    wchar_t detectedProgram[50];
    SAFE_SWPRINTF(detectedProgram, 50, L"현재 감지된 프로그램은 %s 입니다.", isWindowsTerminal ? L"쾌적한 Windows Terminal" : L"cmd");
    writeWideStringToBuffer(34, 20, detectedProgram, isWindowsTerminal ? COLOR_LIGHT_GREEN : COLOR_RED);
    if (!isWindowsTerminal)
        writeWideStringToBuffer(34, 21, L"플레이는 가능하나, 자주 끊기는 현상이 발견되었습니다.", COLOR_RED);
    writeStringToBuffer(47, 23, "[PRESS 'ENTER' TO START]", COLOR_WHITE);
    flipBuffer();

    while (1)
    {
        int key = _getch();
        if (key == 13) // 엔터키 누르면 InGamePage로 감
        {
            return;
        }
    }
}

void InGamePage()
{
    clearBuffer(' ', COLOR_WHITE);

    HANDLE renderTimer = NULL;
    HANDLE calculateScoreTimer = NULL;
    HANDLE createNPCTimer = NULL;

    bool wPressed = false;
    bool sPressed = false;
/*
    FILE *file = _wfopen(L"./1401_권유호.txt", L"w, ccs=UTF-8");
    if (file)
    {
        fwprintf(file, L"점수: %.3lf점\n등급: %c\n", gameInfo.score, gradeText);
        fclose(file);
        Sleep(250);
        return;
    }
    else
    {
        writeWideStringToBuffer(38, HEIGHT - 4, L"저장 실패! [Enter] 키를 눌러 종료해주세요", COLOR_RED); // 생각해보니까 flipBuffer()를 안 해서 어차피 안 뜨네;; 아쉬운거지~
    }*/

    RenderTimerParam renderTimerParam = {0};
    int renderTimerDueTime = 111 - gameInfo.speed;
    // JS의 setInterval마냥 타이머 씀.
    CreateTimerQueueTimer(&renderTimer, NULL, RenderTimerCallback, &renderTimerParam, 0, renderTimerDueTime, 0);

    CreateTimerQueueTimer(&calculateScoreTimer, NULL, CalculateScoreTimerCallback, NULL, 0, 500, 0);

    CreateTimerQueueTimer(&createNPCTimer, NULL, CreateNPCTimerCallback, NULL, 0, 500, 0);

    // 키입력, 차로 줄이기
    while (1)
    {
        if (gameInfo.heart <= 0)
            break;

        DWORD currentTime = GetTickCount();
        int oldSpeed = gameInfo.speed;

        if (_kbhit())
        {
            int key = _getch();
            switch (key)
            {
            // 테스트 할 때 쓰던 코드
            // case 13:
            //     DeleteTimerQueueTimer(NULL, renderTimer, NULL);
            //     DeleteTimerQueueTimer(NULL, calculateScoreTimer, NULL);
            //     DeleteTimerQueueTimer(NULL, createNPCTimer, NULL);
            //     return;
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
                if (!wPressed) // 꾹 누르기 방지
                {
                    addSpeed(5);
                    wPressed = true;

                    if (oldSpeed != gameInfo.speed)
                    {
                        renderTimerDueTime = 111 - gameInfo.speed;
                        ChangeTimerQueueTimer(NULL, renderTimer, renderTimerDueTime, renderTimerDueTime);
                    }
                }
                break;
            case 'S':
            case 's':
                if (!sPressed)
                {
                    addSpeed(-5);
                    sPressed = true;

                    if (oldSpeed != gameInfo.speed)
                    {
                        renderTimerDueTime = 111 - gameInfo.speed;
                        ChangeTimerQueueTimer(NULL, renderTimer, renderTimerDueTime, renderTimerDueTime);
                    }
                }
                break;
            }
        }

        if (!(GetAsyncKeyState('W') & 0x8000) && !(GetAsyncKeyState('w') & 0x8000))
        {
            wPressed = false;
        }
        if (!(GetAsyncKeyState('S') & 0x8000) && !(GetAsyncKeyState('s') & 0x8000))
        {
            sPressed = false;
        }

        if (gameInfo.score > 5000)
            setMaxLane(2);
        else if (gameInfo.score > 4000)
            setMaxLane(3);
        else if (gameInfo.score > 3000)
            setMaxLane(4);

        // 버퍼 변경
        flipBuffer();

        Sleep(1);
    }
    DeleteTimerQueueTimer(NULL, renderTimer, NULL);
    DeleteTimerQueueTimer(NULL, calculateScoreTimer, NULL);
    DeleteTimerQueueTimer(NULL, createNPCTimer, NULL);
    return;
}

void ScorePage()
{
    // 3000~ S, 2500~ A, 2000~ B, 1500~ C, 1000~ D, 0~ F
    clearBuffer(' ', COLOR_WHITE);
    int score = (int)gameInfo.score;
    int gradeIdx;
    // 등급 Index 계산
    if (score >= 3000)
        gradeIdx = 0; // S
    else if (score >= 2500)
        gradeIdx = 1; // A
    else if (score >= 2000)
        gradeIdx = 2; // B
    else if (score >= 1500)
        gradeIdx = 3; // C
    else if (score >= 1000)
        gradeIdx = 4; // D
    else
        gradeIdx = 5; // F

    // 등급 기록용 텍스트
    char gradeText = "SABCDF"[gradeIdx];
    printGrade(5, 1, COLOR_BRIGHT_WHITE);
    // 등급 출력
    for (int i = 0; i < 8; i++)
    {
        writeStringToBuffer(25, 10 + i, Grade_labels[gradeIdx][i], Grade_colors[gradeIdx]);
    }

    // 선긋기
    for (int i = 0; i < WIDTH; i++)
    {
        writeWideStringToBuffer(i, HEIGHT - 9, L"─", COLOR_GRAY);
        writeWideStringToBuffer(i, HEIGHT - 8, L"─", COLOR_GRAY);
        writeWideStringToBuffer(i, HEIGHT - 7, L"─", COLOR_GRAY);
    }
    for (int i = 0; i <= HEIGHT - 7; i++)
    {
        if (i == HEIGHT - 7)
        {
            writeWideStringToBuffer(59, i, L"┴", COLOR_GRAY);
            writeWideStringToBuffer(60, i, L"┴", COLOR_GRAY);
            writeWideStringToBuffer(61, i, L"┴", COLOR_GRAY);
        }
        else if (i == HEIGHT - 8 || i == HEIGHT - 9)
        {
            writeWideStringToBuffer(59, i, L"┼", COLOR_GRAY);
            writeWideStringToBuffer(60, i, L"┼", COLOR_GRAY);
            writeWideStringToBuffer(61, i, L"┼", COLOR_GRAY);
        }
        else
        {
            writeWideStringToBuffer(59, i, L"│", COLOR_GRAY);
            writeWideStringToBuffer(60, i, L"│", COLOR_GRAY);
            writeWideStringToBuffer(61, i, L"│", COLOR_GRAY);
        }
    }

    // Score
    printScore(67, 1, COLOR_LIGHT_PURPLE);
    char scoreStr[6];
    sprintf(scoreStr, "%05d", score);
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            writeStringToBuffer(65 + i * 11, 10 + j, Number_labels[scoreStr[i] - '0'][j], COLOR_PURPLE);
        }
    }

    writeWideStringToBuffer(38, HEIGHT - 5, L"[S] 키를 눌러 저장 (파일명: 1401_권유호.txt)", COLOR_LIGHT_RED);
    writeWideStringToBuffer(50, HEIGHT - 3, L"[R] 키를 눌러 다시시작", COLOR_YELLOW);
    writeWideStringToBuffer(50, HEIGHT - 1, L"[Enter] 키를 눌러 종료", COLOR_BRIGHT_WHITE);

    flipBuffer();
    while (1)
    {
        int key = _getch();
        switch (key)
        {
        case 13:
            return;
        case 'S':
        case 's':
        {
            FILE *file = _wfopen(L"./1401_권유호.txt", L"w, ccs=UTF-8");
            if (file)
            {
                fwprintf(file, L"점수: %.3lf점\n등급: %c\n", gameInfo.score, gradeText);
                fclose(file);
            }
            else
            {
                writeWideStringToBuffer(38, HEIGHT - 4, L"저장 실패! [Enter] 키를 눌러 종료해주세요", COLOR_RED); // 생각해보니까 flipBuffer()를 안 해서 어차피 안 뜨네;; 아쉬운거지~
            }
            break;
        }
        case 'R':
        case 'r':
            GameInitialization();
            iWillRestart = true;
            return;
        }
    }
}
#pragma endregion

#pragma region 타이머 콜백
/*************************/
/** TIMER CALLBACKS **/
/************************/
VOID CALLBACK RenderTimerCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
    clearBuffer(L' ', COLOR_WHITE);
    RenderTimerParam *data = (RenderTimerParam *)lpParam;

    // 나무 렌더링
    drawTrees(0, data->treeOffset);
    drawTrees(68, data->treeOffset);

    // 오른쪽 렌더링
    HowToPlayComponent();

    // lane은 총 6개 (값: 5)임.
    // 렌더링할 차선은 0, 1, 2, 3, 4, 5번 차선이 있음.
    // maxLane이 4일 때는 0, 1, 2, 3, 4(중앙선) 차선이 렌더링됨.
    // maxLane이 3일 때는 0, 1, 2, 3(중앙선) 차선이 렌더링됨.
    // maxLane이 2일 때는 0, 1, 2(중앙선) 차선이 렌더링됨.

    // 차선 렌더링
    drawLines(lanes[0] - 2, data->treeOffset, COLOR_YELLOW, 1);         // 왼쪽 중앙선
    drawLines(lanes[0] + laneOffset, data->treeOffset, COLOR_WHITE, 0); // 0번 레인 오른쪽
    drawLines(lanes[1] + laneOffset, data->treeOffset, COLOR_WHITE, 0); // 1번 레인 오른쪽
    // 2번 레인 오른쪽은 안 사라지고 때에 파선, 실선이 됨.
    // 3번 레인 오른쪽은 파선, 실선, 사라짐이 됨.
    // 4번 레인 오른쪽은 파선, 실선, 사라짐이 됨.
    // 5번 레인 오른쪽은 중앙선, 사라짐이 됨.
    if (maxLane > 2)
        drawLines(lanes[2] + laneOffset, data->treeOffset, COLOR_WHITE, 0);
    if (maxLane > 3)
        drawLines(lanes[3] + laneOffset, data->treeOffset, COLOR_WHITE, 0);
    if (maxLane > 4)
        drawLines(lanes[4] + laneOffset, data->treeOffset, COLOR_WHITE, 0);
    // if (maxLane > 4)
    //     drawLines(lanes[5] + laneOffset, data->treeOffset, COLOR_WHITE, 0);

    // 플레이어 렌더링
    CarComponent(lanes[gameInfo.playerLane], PLAYER_Y, COLOR_RED);

    data->treeOffset = (data->treeOffset + 1) % HEIGHT;

    drawForbiddenLines();
    MoveNPC();
    RenderNPC();
}

VOID CALLBACK CalculateScoreTimerCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
    calculateScore();
}

VOID CALLBACK CreateNPCTimerCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
    srand(GetTickCount() * time(NULL) ^ gameInfo.npcCount - 1);
    if (gameInfo.npcCount >= 50)
        return;

    NPCType npcType = WhatIsThisNPC();
    int npcLane = getRandom(maxLane + 1);
    int npcRandom = getRandom(14) + 1;
    int npcColor = npcType == Heart ? COLOR_RED : (npcRandom == COLOR_RED || npcRandom == COLOR_GRAY ? COLOR_LIGHT_GREEN : npcRandom); // 빨간색은 플레이어, 하트용 & gray는 너무 악질임. 하나도 안 보임.
    int npcX = lanes[npcLane];
    int npcY = 0;

    NPC newNPC = {npcType, npcColor, npcX, npcY};
    gameInfo.npcs[gameInfo.npcCount] = newNPC;
    gameInfo.npcCount++;
}

void MoveNPC()
{
    for (int i = 0; i < gameInfo.npcCount; i++)
    {
        gameInfo.npcs[i].y += 1;
    }
}

void RenderNPC()
{
    for (int i = gameInfo.npcCount - 1; i >= 0; i--)
    {
        NPC *npc = &(gameInfo.npcs[i]);
        bool toRemove = false;

        // 충돌 체크
        if ((PLAYER_Y - 3 <= npc->y) && (npc->y <= PLAYER_Y + 3) && (npc->x == lanes[gameInfo.playerLane]))
        {
            if (npc->type == Heart)
            {
                if (gameInfo.heart < 5)
                { // 하트 오버플로우..? 방지
                    gameInfo.heart++;
                }
            }
            else
            {
                gameInfo.heart--;
            }
            toRemove = true;
        }
        // 화면 밖으로 나감(아래 || 오른쪽)
        else if (npc->y >= HEIGHT + 4 || npc->x > lanes[maxLane])
        {
            toRemove = true;
        }

        if (toRemove)
        {
            removeNPCByIndex(i);
        }
        else
        {
            if (npc->type == Car)
                CarComponent(npc->x, npc->y, npc->color);
            else if (npc->type == Heart)
                HeartComponent(npc->x, npc->y, npc->color);
        }
    }
}
#pragma endregion

#pragma region 컴포넌트
/********************/
/** COMPONENTS **/
/********************/
void HowToPlayComponent()
{
    int xStart = 75;
    int xEnd = 119;

    printHowToPlay(xStart + 4, 2);

    writeWideStringToBuffer(xStart + 3, 9, L"• A, D키를 이용해 좌/우로 움직인다.", COLOR_LIGHT_GREEN);
    writeWideStringToBuffer(xStart + 3, 10, L"• W, S키를 이용해 가/감속 한다.", COLOR_LIGHT_GREEN);
    writeWideStringToBuffer(xStart + 3, 11, L"• 점수는 속도에 따라 지수함수 모양으로", COLOR_LIGHT_GREEN);
    writeWideStringToBuffer(xStart + 3, 12, L"  증가하고, 개발자가 NPC생성 주기를", COLOR_LIGHT_GREEN);
    writeWideStringToBuffer(xStart + 3, 13, L"  500ms로 하드코딩했기 때문에 80km/h로", COLOR_LIGHT_GREEN);
    writeWideStringToBuffer(xStart + 3, 14, L"  설정하면 쉬운 게임을 즐길 수 있습니다.", COLOR_LIGHT_GREEN);

    printInfo(xStart + 11, 16);

    wchar_t recentWstr[50];
    SAFE_SWPRINTF(recentWstr, 50, L"= 최근 점수: %.0f점 | 최근 등급: %c =", recentScore, recentGrade);
    writeWideStringToBuffer(xStart + 3, 23, recentWstr, COLOR_RED);

    wchar_t scoreWstr[50];
    SAFE_SWPRINTF(scoreWstr, 50, L"• 점수: %d", (int)gameInfo.score);
    writeWideStringToBuffer(xStart + 3, 24, scoreWstr, COLOR_LIGHT_YELLOW);

    wchar_t speedWstr[50];
    SAFE_SWPRINTF(speedWstr, 50, L"• 속도: %dkm/h", gameInfo.speed);
    writeWideStringToBuffer(xStart + 3, 25, speedWstr, COLOR_LIGHT_YELLOW);

    wchar_t hearts[] = L"♡ ♡ ♡ ♡ ♡  ";
    writeWideStringToBuffer(xStart + 3, 26, L"• 하트: ", COLOR_LIGHT_YELLOW);
    for (int i = 0; i < gameInfo.heart * 2; i += 2)
    {
        hearts[i] = L'❤';
    }
    writeWideStringToBuffer(xStart + 9, 26, hearts, COLOR_RED);

        // 테두리 그리기
    writeToBuffer(xStart, 0, L'┌', COLOR_GRAY);
    for (int i = xStart + 1; i < xEnd; i++)
    {
        writeToBuffer(i, 0, L'─', COLOR_GRAY);
    }
    writeToBuffer(xEnd, 0, L'┐', COLOR_GRAY);

    for (int i = 1; i < HEIGHT - 1; i++)
    {
        writeToBuffer(xStart, i, L'│', COLOR_GRAY);
        writeToBuffer(xEnd, i, L'│', COLOR_GRAY);
    }

    writeToBuffer(xStart, HEIGHT - 1, L'└', COLOR_GRAY);
    for (int i = xStart + 1; i < xEnd; i++)
    {
        writeToBuffer(i, HEIGHT - 1, L'─', COLOR_GRAY);
    }
    writeToBuffer(xEnd, HEIGHT - 1, L'┘', COLOR_GRAY);
}

// 자동차 컴포넌트
void CarComponent(int x, int y, int color)
{
    writeStringToBuffer(x, y - 3, "  ____  ", color);
    writeStringToBuffer(x, y - 2, " / ** \\", color);
    writeStringToBuffer(x, y - 1, "/oo  oo\\", color);
    writeStringToBuffer(x, y, " 0    0 ", color);
}

void HeartComponent(int x, int y, int color)
{
    writeStringToBuffer(x, y - 3, " **  ** ", color);
    writeStringToBuffer(x, y - 2, "********", color);
    writeStringToBuffer(x, y - 1, " ****** ", color);
    writeStringToBuffer(x, y, "  ****  ", color);
}

// 나무 그리기
void drawTrees(int x, int y)
{
    for (int i = 0; i < HEIGHT + 5; i += 5)
    {
        int treeY = (i + y) % HEIGHT;
        writeStringToBuffer(x, treeY, "  /\\  ", COLOR_GREEN);
        writeStringToBuffer(x, (treeY + 1) % HEIGHT, " /**\\ ", COLOR_GREEN);
        writeStringToBuffer(x, (treeY + 2) % HEIGHT, "/****\\", COLOR_GREEN);
        writeStringToBuffer(x, (treeY + 3) % HEIGHT, "  ||  ", COLOR_YELLOW);
        writeStringToBuffer(x, (treeY + 4) % HEIGHT, "    ", COLOR_YELLOW);
    }
}

// 0: 파선, 1: 실선
void drawLines(int x, int y, int color, int lineType)
{
    int actualY = lineType == 1 ? 0 : y;

    for (int i = 0; i < HEIGHT + 5; i += 5)
    {
        int laneY = (i + actualY) % HEIGHT;
        writeWideStringToBuffer(x, laneY, lineType ? L"││" : L"┌┐", color);
        writeWideStringToBuffer(x, (laneY + 1) % HEIGHT, L"││", color);
        writeWideStringToBuffer(x, (laneY + 2) % HEIGHT, L"││", color);
        writeWideStringToBuffer(x, (laneY + 3) % HEIGHT, lineType ? L"││" : L"└┘", color);
        writeWideStringToBuffer(x, (laneY + 4) % HEIGHT, lineType ? L"││" : L"  ", color);
    }
}

void drawForbiddenLines()
{
    drawLines(lanes[maxLane] + laneOffset, 0, COLOR_YELLOW, 1); // 중앙선
    for (int lane = 5; lane > maxLane; lane--)
    {
        for (int i = 0; i < HEIGHT; i++)
        {
            // 막힌 차로에 / 그리기
            for (int w = 0; w < 10; w++)
                writeWideStringToBuffer(lanes[lane] + w, i, L"/", COLOR_YELLOW);
        }
    }
}
#pragma endregion

#pragma region 함수 정의
/****************/
/** FUNCTIONS **/
/****************/
void gotoxy(int x, int y)
{
    COORD Pos = {x, y};
    SetConsoleCursorPosition(db.hConsole, Pos);
}

int getRandom(int max)
{
    return rand() % max; // 0 ~ max-1
}

// https://patorjk.com/software/taag/#p=display&f=Big%20Money-ne
void printTitle(int x, int y)
{
    writeStringToBuffer(x, y, "  /$$$$$$  /$$$$$$$         /$$$$$$   /$$$$$$  /$$$$$$$        /$$$$$$$   /$$$$$$   /$$$$$$  /$$$$$$ /$$   /$$  /$$$$$$", COLOR_RED);
    writeStringToBuffer(x, y + 1, " /$$__  $$| $$__  $$       /$$__  $$ /$$__  $$| $$__  $$      | $$__  $$ /$$__  $$ /$$__  $$|_  $$_/| $$$ | $$ /$$__  $$", COLOR_RED);
    writeStringToBuffer(x, y + 2, "|__/  \\ $$| $$  \\ $$      | $$  \\__/| $$  \\ $$| $$  \\ $$      | $$  \\ $$| $$  \\ $$| $$  \\__/  | $$  | $$$$| $$| $$  \\__/", COLOR_RED);
    writeStringToBuffer(x, y + 3, "  /$$$$$$/| $$  | $$      | $$      | $$$$$$$$| $$$$$$$/      | $$$$$$$/| $$$$$$$$| $$        | $$  | $$ $$ $$| $$ /$$$$", COLOR_RED);
    writeStringToBuffer(x, y + 4, " /$$____/ | $$  | $$      | $$      | $$__  $$| $$__  $$      | $$__  $$| $$__  $$| $$        | $$  | $$  $$$$| $$|_  $$", COLOR_RED);
    writeStringToBuffer(x, y + 5, "| $$      | $$  | $$      | $$    $$| $$  | $$| $$  \\ $$      | $$  \\ $$| $$  | $$| $$    $$  | $$  | $$\\  $$$| $$  \\ $$", COLOR_RED);
    writeStringToBuffer(x, y + 6, "| $$$$$$$$| $$$$$$$/      |  $$$$$$/| $$  | $$| $$  | $$      | $$  | $$| $$  | $$|  $$$$$$/ /$$$$$$| $$ \\  $$|  $$$$$$/", COLOR_RED);
    writeStringToBuffer(x, y + 7, "|________/|_______/        \\______/ |__/  |__/|__/  |__/      |__/  |__/|__/  |__/ \\______/ |______/|__/  \\__/ \\______/ ", COLOR_RED);
}

// https://patorjk.com/software/taag/#p=display&f=Big
void printHowToPlay(int x, int y)
{
    writeStringToBuffer(x, y, "  _    _                 _______", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y + 1, " | |  | |               |__   __|", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y + 2, " | |__| | _____      __    | | ___", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y + 3, " |  __  |/ _ \\ \\ /\\ / /    | |/ _ \\", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y + 4, " | |  | | (_) \\ V  V /     | | (_) |", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y + 5, " |_|  |_|\\___/ \\_/\\_/      |_|\\___/", COLOR_LIGHT_BLUE);
}

void printInfo(int x, int y)
{
    writeStringToBuffer(x, y, "  _____        __      ", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y + 1, " |_   _|      / _|     ", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y + 2, "   | |  _ __ | |_ ___  ", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y + 3, "   | | | '_ \\|  _/ _ \\ ", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y + 4, "  _| |_| | | | || (_) |", COLOR_LIGHT_BLUE);
    writeStringToBuffer(x, y + 5, " |_____|_| |_|_| \\___/ ", COLOR_LIGHT_BLUE);
}

void printGrade(int x, int y, int color)
{
    writeStringToBuffer(x, y + 0, "  /$$$$$$   /$$$$$$   /$$$$$$  /$$$$$$$  /$$$$$$$$", color);
    writeStringToBuffer(x, y + 1, " /$$__  $$| $$__  $$ /$$__  $$| $$__  $$| $$_____/", color);
    writeStringToBuffer(x, y + 2, "| $$  \\__/| $$  \\ $$| $$  \\ $$| $$  \\ $$| $$      ", color);
    writeStringToBuffer(x, y + 3, "| $$ /$$$$| $$$$$$$/| $$$$$$$$| $$  | $$| $$$$$   ", color);
    writeStringToBuffer(x, y + 4, "| $$|_  $$| $$__  $$| $$__  $$| $$  | $$| $$__/   ", color);
    writeStringToBuffer(x, y + 5, "| $$  \\ $$| $$  \\ $$| $$  | $$| $$  | $$| $$      ", color);
    writeStringToBuffer(x, y + 6, "|  $$$$$$/| $$  | $$| $$  | $$| $$$$$$$/| $$$$$$$$", color);
    writeStringToBuffer(x, y + 7, " \\______/ |__/  |__/|__/  |__/|_______/ |________/", color);
}

void printScore(int x, int y, int color)
{
    writeStringToBuffer(x, y + 0, "  /$$$$$$   /$$$$$$   /$$$$$$  /$$$$$$$  /$$$$$$$$", color);
    writeStringToBuffer(x, y + 1, " /$$__  $$ /$$__  $$ /$$__  $$| $$__  $$| $$_____/", color);
    writeStringToBuffer(x, y + 2, "| $$  \\__/| $$  \\__/| $$  \\ $$| $$  \\ $$| $$      ", color);
    writeStringToBuffer(x, y + 3, "|  $$$$$$ | $$      | $$  | $$| $$$$$$$/| $$$$$   ", color);
    writeStringToBuffer(x, y + 4, " \\____  $$| $$      | $$  | $$| $$__  $$| $$__/   ", color);
    writeStringToBuffer(x, y + 5, " /$$  \\ $$| $$    $$| $$  | $$| $$  \\ $$| $$      ", color);
    writeStringToBuffer(x, y + 6, "|  $$$$$$/|  $$$$$$/|  $$$$$$/| $$  | $$| $$$$$$$$", color);
    writeStringToBuffer(x, y + 7, " \\______/  \\______/  \\______/ |__/  |__/|________/", color);
}

void calculateScore()
{
    double y_values[] = {// { 10 <= x <= 110 }의 y값. y=1.45\cdot e^{\left(0.032\cdot x\right)}\cdot2\ \left\{10\ \le\ x\ \le\ 110\ \right\} (desmos임)
                         3.863145, 4.556196, 5.143086, 5.812064, 6.574221,
                         7.442174, 8.430248, 9.554559, 10.833014, 12.285258,
                         13.932561, 15.798666, 17.909581, 20.293312, 22.980529,
                         26.004098, 29.399672, 33.205321, 37.461183, 42.209138,
                         47.493501};
    gameInfo.score += y_values[(gameInfo.speed - 10) / 5];
}

void removeNPCByIndex(int index)
{
    if (index < 0 || index >= gameInfo.npcCount)
        return;

    // 마지막 요소를 현재 위치로 이동
    gameInfo.npcs[index] = gameInfo.npcs[gameInfo.npcCount - 1];
    gameInfo.npcCount--;
}

NPCType WhatIsThisNPC()
{
    srand(GetTickCount() * time(NULL) ^ gameInfo.npcCount);
    int randomValue = getRandom(101);
    if (randomValue < 5)
    {
        return Heart;
    }
    else
    {
        return Car;
    }
}

void setMaxLane(int lane)
{
    if (maxLane < 2)
        return; // 3차로면 개빡셈;
    maxLane = lane;
    if (gameInfo.playerLane > maxLane)
        gameInfo.playerLane = maxLane; // 레인 줄이면 강제로 왼쪽으로 이동해야지
}
#pragma endregion

#pragma region 컨트롤러
/********************/
/** CONTROLLERS **/
/********************/

void movePlayer(int direction)
{
    if (direction == 0 && gameInfo.playerLane > 0)
        gameInfo.playerLane--;
    else if (direction == 1 && gameInfo.playerLane < maxLane)
        gameInfo.playerLane++;
}
void addSpeed(int value)
{
    int *speed = &(gameInfo.speed);

    if (value < 0)
    {
        // 감속: 최소 속도 10까지
        int newSpeed = *speed + value;
        if (newSpeed >= 10)
        {
            *speed = newSpeed;
        }
        else
        {
            *speed = 10;
        }
    }
    else if (value > 0)
    {
        // 가속: 최대 속도 110까지
        int newSpeed = *speed + value;
        if (newSpeed <= 110)
        {
            *speed = newSpeed;
        }
        else
        {
            *speed = 110;
        }
    }
}
#pragma endregion
