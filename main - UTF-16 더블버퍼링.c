#include <stdio.h>
#include <windows.h>
#include <conio.h>

#define WIDTH 120
#define HEIGHT 29

// 생성된 차량을 랜덤적으로 배열에 추가 => 배열에 있는 걸 루프 돌리면서 y 내림.(speed와 함께) => 만약 y가 HEIGHT인가 그 정도라면 pop시키기.
// 짜피 speed도 계속 렌더링 해야하니까 How To도 그냥 더블 버퍼링 써야겠다.

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
wchar_t buffer1[HEIGHT][WIDTH + 1];
wchar_t buffer2[HEIGHT][WIDTH + 1];
wchar_t (*current)[WIDTH + 1] = buffer1;
wchar_t (*next)[WIDTH + 1] = buffer2;

void Initialization();

void PortalPage();
void InGamePage();

void HowToPlayComponent();
void CarComponent(int x,  int y, int color);

void gotoxy(int x, int y);
void setColor(int color);
void clearNextBuf();
void renderBuf();
void swapBuf();

void printTitle(int x, int y);
void printHowToPlay(int x, int y);
void drawTrees(int x, int y);

int main() {
    Initialization();
    // PortalPage();
    InGamePage();
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
}

/************/
/** PAGES **/
/************/
void PortalPage() {
    setColor(4);
    printTitle(0, 5);
    setColor(7);
    gotoxy(47, 20);
    printf("[PRESS 'ENTER' TO START]");
    while (1) {
        int key = _getch();
        if (key == 13) {
            system("cls");
            break;
        }
    }
}
void InGamePage() {
    HowToPlayComponent();
    for (int i = 1; i <= 6; i++) {
        CarComponent(8*i + ((i-1)*2), 13, 4);
    }
    int i = 0;
    while (1) {
        clearNextBuf();

        drawTrees(0, i);
        drawTrees(68, i);

        renderBuf();
        swapBuf();
        Sleep(50);

        if (_kbhit()) {
            int key = _getch();
            if (key == 13) {
                system("cls");
                break;
            }
        }
        i++;
    }
}

/********************/
/** COMPONENTS **/
/********************/
void HowToPlayComponent() {
    int xStart = 75;
    int xEnd = 119;

    gotoxy(xStart, 0);
    printf("┌");
    for (int i = xStart+1; i < xEnd; i++) {
        printf("─");
    }
    printf("┐");

    for (int i = 1; i < HEIGHT; i++) {
        gotoxy(xStart, i);
        printf("│");
        gotoxy(xEnd, i);
        printf("│");
    }

    gotoxy(xStart, HEIGHT);
    printf("└");
    for (int i = xStart+1; i < xEnd; i++) {
        printf("─");
    }
    printf("┘");
    printHowToPlay(xStart+4, 1);
    //  6차로 => 5차로 => 4차로 => 3차로?
    gotoxy(xStart+3, 9);
    printf("• A, D키를 이용해 좌, 우로 움직인다."); // 반각 8칸씩이요 ㅎ
}
// (반)8x8
void CarComponent(int x, int y, int color) {
    setColor(color);
    gotoxy(x, y-3);
    printf("  ____  ");
    gotoxy(x, y-2);
    printf(" / ** \\");
    gotoxy(x, y-1);
    printf("/oo  oo\\");
    gotoxy(x, y);
    printf(" 0    0 ");
    setColor(7);
}

/**************/
/** UTILITES **/
/**************/
void gotoxy(int x,int y) {
    COORD pos={ x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
//0 = Black 8 = Gray
//1 = Blue 9 = Light Blue
//2 = Green a = Light Green
//3 = Aqua b = Light Aqua
//4 = Red c = Light Red
//5 = Purple d = Light Purple
//6 = Yellow e = Light Yellow
//7 = White f = Bright White
void setColor(int color) {
    HANDLE  hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}
void clearNextBuf() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            next[y][x] = L' ';
        }
        next[y][WIDTH] = L'\0';
    }
}
void renderBuf() {
    gotoxy(0, 0);
    for (int y = 0; y < HEIGHT; y++) {
        wprintf(L"%ls\n", next[y]);
    }
}
void swapBuf() {
    wchar_t (*temp)[WIDTH + 1] = current;
    current = next;
    next = temp;
}

/****************/
/** FUNCTION **/
/****************/
// https://patorjk.com/software/taag/#p=display&f=Big%20Money-ne
void printTitle(int x, int y) {
    gotoxy(x, y);
    printf("  /$$$$$$  /$$$$$$$         /$$$$$$   /$$$$$$  /$$$$$$$        /$$$$$$$   /$$$$$$   /$$$$$$  /$$$$$$ /$$   /$$  /$$$$$$ \n");
    gotoxy(x, y+1);
    printf(" /$$__  $$| $$__  $$       /$$__  $$ /$$__  $$| $$__  $$      | $$__  $$ /$$__  $$ /$$__  $$|_  $$_/| $$$ | $$ /$$__  $$\n");
    gotoxy(x, y+2);
    printf("|__/  \\ $$| $$  \\ $$      | $$  \\__/| $$  \\ $$| $$  \\ $$      | $$  \\ $$| $$  \\ $$| $$  \\__/  | $$  | $$$$| $$| $$  \\__/\n");
    gotoxy(x, y+3);
    printf("  /$$$$$$/| $$  | $$      | $$      | $$$$$$$$| $$$$$$$/      | $$$$$$$/| $$$$$$$$| $$        | $$  | $$ $$ $$| $$ /$$$$\n");
    gotoxy(x, y+4);
    printf(" /$$____/ | $$  | $$      | $$      | $$__  $$| $$__  $$      | $$__  $$| $$__  $$| $$        | $$  | $$  $$$$| $$|_  $$\n");
    gotoxy(x, y+5);
    printf("| $$      | $$  | $$      | $$    $$| $$  | $$| $$  \\ $$      | $$  \\ $$| $$  | $$| $$    $$  | $$  | $$\\  $$$| $$  \\ $$\n");
    gotoxy(x, y+6);
    printf("| $$$$$$$$| $$$$$$$/      |  $$$$$$/| $$  | $$| $$  | $$      | $$  | $$| $$  | $$|  $$$$$$/ /$$$$$$| $$ \\  $$|  $$$$$$/\n");
    gotoxy(x, y+7);
    printf("|________/|_______/        \\______/ |__/  |__/|__/  |__/      |__/  |__/|__/  |__/ \\______/ |______/|__/  \\__/ \\______/ \n");
}
void printHowToPlay(int x, int y) {
    gotoxy(x, y);
    printf("  _    _                 _______    ");
    gotoxy(x, y+1);
    printf(" | |  | |               |__   __|   ");
    gotoxy(x, y+2);
    printf(" | |__| | _____      __    | | ___  ");
    gotoxy(x, y+3);
    printf(" |  __  |/ _ \\ \\ /\\ / /    | |/ _ \\ ");
    gotoxy(x, y+4);
    printf(" | |  | | (_) \\ V  V /     | | (_) |");
    gotoxy(x, y+5);
    printf(" |_|  |_|\\___/ \\_/\\_/      |_|\\___/ ");

}
void drawTrees(int x, int y) {
    for (int i = 0; i < HEIGHT; i+=5) {
        int treeY = (i + y) % HEIGHT;

        next[treeY][x] = L'　';
        next[treeY][x+1] = L'🟩';
        next[treeY][x+2] = L'　';

        next[treeY+1][x] = L'🟩';
        next[treeY+1][x+1] = L'🟩';
        next[treeY+1][x+2] = L'🟩';

        next[treeY+2][x] = L'　';
        next[treeY+2][x+1] = L'🟫';
        next[treeY+2][x+2] = L'　';

        next[treeY+3][x] = L'　';
        next[treeY+3][x+1] = L'🟫';
        next[treeY+3][x+2] = L'　';

        next[treeY+4][x] = L'　';
        next[treeY+4][x+1] = L'　';
        next[treeY+4][x+2] = L'　';
    }
}
