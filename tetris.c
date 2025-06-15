//
// Created by 박우현 on 25. 6. 1.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
    #include "termios.h"
    #include <windows.h>
    #include <conio.h>
    #define CLEAR_SCREEN_CMD "cls"
#else
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/ioctl.h>
// #include <sys/types.h>
// 굳이 필요한가.
#define CLEAR_SCREEN_CMD "clear"
#endif

/* 타이머  */
#define CCHAR 0
#ifdef CTIME
#undef CTIME
#endif
#define CTIME 1

/* 왼쪽, 오른쪽, 아래, 회전  */
#define LEFT 0
#define RIGHT 1
#define DOWN 2
#define ROTATE 3

/* 블록 모양 */
#define I_BLOCK 0
#define	T_BLOCK 1
#define S_BLOCK 2
#define Z_BLOCK 3

#define L_BLOCK 4
#define J_BLOCK 5
#define O_BLOCK 6

/* 게임 시작, 게임 종료 */
#define GAME_START 0
#define GAME_END 1

#define MAX_NAME_LEN 30

/*
 * 블록 모양(I, T, S, Z, L, J, O)
 * 4*4 배열의 2차원 배열
 * 모든 블록의 모양을 표시
 *
 * 블록의 모양을 표시
 * 왼쪽, 오른쪽, 아래, 회전
 * 4*4 배열의 2차원 배열
 * 모든 블록의 모양을 표시
 *
 * 4*4*4 배열의 3차원 배열
 */

char null_block[4][4][4] =
{
    {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {}, {}, {}
};

char i_block[4][4][4] =
{
    {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1}},
    {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}},
    {{1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}}
};

char t_block[4][4][4] =
{
    {{1, 0, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}}
};


char s_block[4][4][4] =
{
    {{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}}

};

char z_block[4][4][4] =
{
    {{0, 1, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}
};

char l_block[4][4][4] =
{
    {{1, 0, 0, 0}, {1, 0, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}}

};

char j_block[4][4][4] =
{
    {{0, 1, 0, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
    {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}
};


char o_block[4][4][4] =
{
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}

};


/* 테트리스 판을 2차원 배열로 표현
 * 2차원 배열의 2차원 배열
 * 모든 블록의 모양을 표시
 *
 * 20*8 배열
 * 모든 블록의 모양을 표시
 * 모든 블록의 모양을 표시*/

char tetris_table[21][10];

/* 게임 종료 땨마다
 * 이름과 득점수와
 * 날짜와 시간과 순위를 저장
 * */
static struct result
{
    char name[MAX_NAME_LEN];
    long point;
    int year;
    int month;
    int day;
    int hour;
    int min;
    int rank;
} temp_result;

int block_number = 0; /*블록 번호*/
int next_block_number = 0; /*다음 블록 번호 */
int block_state = 0; /*블록 상태, 왼쪽, 오른쪽, 아래, 회전  */
int x = 3, y = 0; /*블록의 위치*/
int game = GAME_END; /*게임 시작, 게임 종료*/
long best_point = 0; /* 최고 점수*/
long point = 0; /* 현재 점수*/

/* 터미널 입출력 제어를 위한 원래 터미널 설정 저장 */
struct termios orig_termios;
int hold_block_number = 7; // hold된 블록 번호
int hold_used_in_turn = 0; // 한 턴에 한 번만 hold 허용

#ifdef _WIN32
/* Windows 환경에서는 타이머 관련 변수가 필요 없으므로 선언하지 않음. */
#else
/* POSIX(macOS/Linux) 전용 타이머용 변수 */
struct itimerval timer;
void alarm_handler(int signum);
void init_timer(void);
void stop_timer(void);

#endif
/* 블록 모양 포인터 배열로 묶어 두기 */
char (*blocks[8])[4][4][4] = {
    &i_block, &t_block, &s_block, &z_block, &l_block, &j_block, &o_block, &null_block
};


int display_menu(void); /* 메뉴 표시*/

int game_start(void); // 게임 시작
void search_result(void); // Search history
void record_output(void); //  Record Output

void enable_raw_mode(void); // 터미널 입력 모드 non-canonical
void disable_raw_mode(void); // 터미널 입력 모드 canonical

void init_table(void); // 게임보드 초기화
void draw_table(void); // 게임보드 그리기
void clear_screen(void); // 화면 지우기
void set_random_block(void); // 블럭 랜던 설정
void place_block(void); // 블럭 놓기
void remove_block(void); // 블럭 지우기
int is_collision(int newY, int newX, int newState); // 충돌 판정
void move_left(void); // 블럭 이동
void move_right(void);
void move_down(void);
void rotate_block(void); // 회전
void clear_lines(void); // 줄 지우기
void lock_block(void); // 블럭 놓기
int get_key(void);
void process_key(int key);


void save_result(void);

void press_any_key(void);

void load_best_point(void);
void sort_record(void);
int load_results_count(void);
struct result* load_results(void);

char get_next_block_char(void);
int compute_ghost_y(void);
void hold_block(void);

void get_terminal_size(int* rows, int* cols);
int is_terminal_size_sufficient(void);


int main(void)
{
    while (!is_terminal_size_sufficient())
    {
        clear_screen();
    }

#ifdef _WIN32
        // Windows 콘솔을 UTF-8(65001)로 설정 (복구 코드 제거)
    SetConsoleOutputCP(CP_UTF8);
#endif
    int menu = 1;
    load_best_point();
    while (menu)
    {
        menu = display_menu();

        if (menu == 1)
        {
            game = GAME_START;
            menu = game_start();
        }
        else if (menu == 2)
        {
            search_result();
        }
        else if (menu == 3)
        {
            record_output();
        }
        else if (menu == 4)
        {
            exit(0);
        }
    }

    return 0;
}

void clear_screen(void)
{
    system(CLEAR_SCREEN_CMD);
}

int game_start(void)
{
    // 1) 터미널 설정
    enable_raw_mode();
    // 화면 완전히 지우기 (ANSI 코드)
    clear_screen();
    printf("\033[2J");
    printf("\033[H");

    // 2) 테이블 초기화
    init_table();
    srand((unsigned int)time(NULL));
    next_block_number = rand() % 7;
    set_random_block();
    place_block();

    // 3) 스크린 그리기
    draw_table();

#ifndef _WIN32
    // 4) 타이머 설정
    init_timer();
    // 5) 메인 루프
    while (game == GAME_START)
    {
        int key = get_key();
        if (key != -1)
        {
            process_key(key);
            draw_table();
        }
        // 짧은 usleep으로 CPU 사용률 낮추기
        usleep(50 * 1000); // 50ms
    }
#else
    // Windows용 타이머: 마지막으로 블록을 내린 시점(ms 단위)
    DWORD lastFall = GetTickCount();
    const DWORD fallInterval = 500;  // 500ms마다 자동 낙하

    while (game == GAME_START)
    {
        // 1. 키 입력 처리
        int key = get_key();
        if (key != -1)
        {
            process_key(key);
            draw_table();
        }

        // 2. 자동 낙하 타이밍 체크
        DWORD now = GetTickCount();
        if (now - lastFall >= fallInterval)
        {
            // fallInterval(예: 500ms) 경과했으면 블록 한 칸 아래로
            move_down();
            draw_table();
            lastFall = now;
        }

        // 3. CPU 사용률 절감
        Sleep(20);  // 20ms 대기 (필요에 따라 조절)
    }


#endif


#ifndef _WIN32
    // 6) 게임 종료 처리
    stop_timer();
#endif

    disable_raw_mode();

    // 게임 오버 메시지
    clear_screen();
    printf("\033[H\t\t\t");
    printf("\n\t\t\t");
    printf("\n\t\t\t============================");
    printf("\n\t\t\t\tGAME OVER\t");
    printf("\n\t\t\t============================");
    printf("\n\t\t\t\tYour score: %ld\n", point);

    if (point > best_point)
    {
        printf("\n\t\t\tBest Record!\n");
        best_point = point;
        // best_point 저장 로직 삽입 가능 (파일 I/O 등)
    }
    save_result();
    point = 0; // 초기화
    press_any_key();
    // 메뉴로 돌아가려면 1 또는 0 반환
    return 1;
}

/* 원래 터미널 속성 저장 */
void enable_raw_mode(void)
{
    tcgetattr(STDIN_FILENO, &orig_termios);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON); // 에코 끄고, 버퍼링 끄기
    raw.c_cc[VMIN] = 0; // 최소 0자 읽기
    raw.c_cc[VTIME] = 1; // 0.1초 대기

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

/* 원래 터미널 속성으로 복원 */
void disable_raw_mode(void)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void draw_table(void)
{
    int i, j;
    // 1) 고스트 y 계산
    int ghost_y = compute_ghost_y();

    // 2) 커서 맨 위로 이동
    printf("\033[H");
    // 점수 등 정보 출력
    printf("\t\t\tBest Score: %ld Score: %ld\n", best_point, point);
    printf("\t\t\tNext Block | Hold block\n");
    char (*next_shape)[4] = (*blocks[next_block_number])[0];
    char (*hold_shape)[4] = (*blocks[hold_block_number])[0];


    for (int ni = 0; ni < 4; ni++)
    {
        printf("\t\t\t");
        for (int nj = 0; nj < 4; nj++)
        {
            if (next_shape[ni][nj])
            {
                printf("🟨");
            }
            else
            {
                printf("  ");
            }
        }
        printf("🔲🔲");
        for (int hj = 0; hj < 4; hj++)
        {
            if (hold_shape[ni][hj])
            {
                printf("🟩");
            }
            else
            {
                printf("  ");
            }
        }
        printf("\n");
    }


    printf("\t\t\t");
    for (j = 0; j < 10; j++)
    {
        printf("🔲");
    }
    printf("\n\t\t\t");
    for (i = 0; i < 21; i++)
    {
        for (j = 0; j < 10; j++)
        {
            //  먼저 고정 블록(3)과 벽/바닥(1)은 그대로 출력
            if (tetris_table[i][j] == 1)
            {
                printf("🔲");
            }
            else if (tetris_table[i][j] == 3)
            {
                printf("⬜");
            }
            else
            {
                // 빈 칸일 때, 고스트와 겹치는지 확인
                int printed = 0;

                // 고스트 블록은 블록 모양 참조
                char (*shape)[4] = (*blocks[block_number])[block_state];
                // 블록의 4×4 셀 중, i와 j가 고스트 위치의 블록 셀인지 확인
                // 고스트가 y = ghost_y이고, x = x 이므로
                for (int bi = 0; bi < 4 && !printed; bi++)
                {
                    for (int bj = 0; bj < 4; bj++)
                    {
                        if (shape[bi][bj])
                        {
                            int gi = ghost_y + bi;
                            int gj = x + bj;
                            if (gi == i && gj == j)
                            {
                                // (i, j)가 고스트가 찍힐 자리면
                                printf("🟪");
                                printed = 1;
                                break;
                            }
                        }
                    }
                }
                if (printed)
                {
                    continue;
                }

                // 현재 낙하 중인 블록
                if (tetris_table[i][j] == 2)
                {
                    printf("🔳");
                }
                // 빈칸
                else
                {
                    printf("  ");
                }
            }
        }
        printf("\n\t\t\t");
    }
    printf("A: Drop, S: Hold, P: Game Stop\n\t\t\t");
    printf("I: Rotate, K: Down\n\t\t\t");
    printf("J: Move Left, L: Move Right\n");
}

void init_table(void)
{
    int i, j;
    // 모든 칸을 0으로 초기화
    for (i = 0; i < 21; i++)
    {
        for (j = 0; j < 10; j++)
        {
            tetris_table[i][j] = 0;
        }
    }
    // 왼쪽/오른쪽 벽(1) 설정
    for (i = 0; i < 21; i++)
    {
        tetris_table[i][0] = 1; // 왼쪽 벽
        tetris_table[i][9] = 1; // 오른쪽 벽
    }
    // 바닥(1) 설정
    for (j = 0; j < 10; j++)
    {
        tetris_table[20][j] = 1;
    }
}

void set_random_block(void)
{
    block_number = next_block_number;
    next_block_number = rand() % 7;
    block_state = 0;
    x = 3;
    y = 0;
}

// 현재 블록을 테트리스판에 표시
void place_block(void)
{
    int i, j;
    char (*shape)[4] = (*blocks[block_number])[block_state];
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (shape[i][j])
            {
                tetris_table[y + i][x + j] = 2;
            }
        }
    }
}

// 현재 블록을 테트리스판에서 지우기
void remove_block(void)
{
    int i, j;
    char (*shape)[4] = (*blocks[block_number])[block_state];
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (shape[i][j])
            {
                tetris_table[y + i][x + j] = 0;
            }
        }
    }
}

int is_collision(int newY, int newX, int newState)
{
    int i, j;
    char (*shape)[4] = (*blocks[block_number])[newState];
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (shape[i][j])
            {
                int ty = newY + i;
                int tx = newX + j;
                // 테이블 경계 벗어나면 충돌
                if (ty < 0 || ty >= 21 || tx < 0 || tx >= 10) return 1;
                // 벽(1) 또는 고정 블록(3)에 닿으면 충돌
                if (tetris_table[ty][tx] == 1 || tetris_table[ty][tx] == 3) return 1;
            }
        }
    }
    return 0; // 충돌 없음
}

void move_left(void)
{
    remove_block();
    if (!is_collision(y, x - 1, block_state))
    {
        x = x - 1;
    }
    place_block();
}

void move_right(void)
{
    remove_block();
    if (!is_collision(y, x + 1, block_state))
    {
        x = x + 1;
    }
    place_block();
}

void move_down(void)
{
    remove_block();
    if (!is_collision(y + 1, x, block_state))
    {
        y = y + 1;
        place_block();
    }
    else
    {
        // 더 아래로 못 가면 고정
        place_block(); // 다시 표시
        lock_block();
    }
}

void rotate_block(void)
{
    int newState = (block_state + 1) % 4;
    remove_block();
    if (!is_collision(y, x, newState))
    {
        block_state = newState;
    }
    place_block();
}

/* 블록이 바닥이나 다른 블록 위에 닿으면 고정시키는 함수 */
void lock_block(void)
{
    int i, j;
    char (*shape)[4] = (*blocks[block_number])[block_state];
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (shape[i][j])
            {
                tetris_table[y + i][x + j] = 3; // 고정 블록 표시
            }
        }
    }
    clear_lines(); // 위에서부터 줄이 완성됐는지 확인 후 삭제
    set_random_block(); // 다음 블록 생성
    if (is_collision(y, x, block_state))
    {
        // 블럭이 생성하자마자 충돌하면 게임 오버
        game = GAME_END;
    }
    else
    {
        place_block(); // 새 블록 화면에 표시
        point += 25; // 블럭 놓는데 성공하면 25점
    }
    // 블록이 완전히 고정된 후에는 hold 다시 가능
    hold_used_in_turn = 0;
}

void clear_lines(void)
{
    int row, col, full;
    for (row = 0; row < 20; row++)
    {
        full = 1;
        for (col = 1; col < 9; col++)
        {
            // 내부 8칸만 검사
            if (tetris_table[row][col] != 3)
            {
                full = 0;
                break;
            }
        }
        if (full)
        {
            // 해당 줄을 지우고 위쪽 블록을 한 칸씩 내리기
            int r;
            for (r = row; r > 0; r--)
            {
                for (col = 1; col < 9; col++)
                {
                    tetris_table[r][col] = tetris_table[r - 1][col];
                }
            }
            // 맨 위 행은 모두 빈칸(0)으로 초기화, 벽/바닥은 그대로 놔둠
            for (col = 1; col < 9; col++)
            {
                tetris_table[0][col] = 0;
            }
            point += 100; // 한 줄 삭제 시 점수 추가
        }
    }
}

int get_key(void)
{
#ifdef _WIN32
    if (_kbhit()) {
        return _getch();
    }
    return -1;
#else
    char c;
    int n = read(STDIN_FILENO, &c, 1);
    if (n == 1)
    {
        return (int)c;
    }
    return -1;
#endif
}

void process_key(int key)
{
    if (key == 'j' || key == 'J')
    {
        move_left();
    }
    else if (key == 'l' || key == 'L')
    {
        move_right();
    }
    else if (key == 'k' || key == 'K')
    {
        move_down();
    }
    else if (key == 'i' || key == 'I')
    {
        rotate_block();
    }
    else if (key == 'a' || key == 'A')
    {
        // 블록 바로 떨어뜨리기
        remove_block();
        while (!is_collision(y + 1, x, block_state))
        {
            y++;
        }
        place_block();
        lock_block(); // 바로 고정
    }
    else if (key == 'p' || key == 'P')
    {
        game = GAME_END;
    }

    else if (key == 's' || key == 'S')
    {
        hold_block();
    }
}
#ifndef _WIN32
/* SIGALRM 시그널 핸들러 */
void alarm_handler(int signum)
{
    // 주기마다, 블록 내리기
    move_down();
    draw_table();
}

/* 타이머 초기화 (500ms 간격) */
void init_timer(void)
{
    signal(SIGALRM, alarm_handler);
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 500000; // 0.5초
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 500000; // 주기적 0.5초
    setitimer(ITIMER_REAL, &timer, NULL);
}

/* 타이머 종료(0으로 설정) */
void stop_timer(void)
{
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
}
#endif

void save_result(void)
{
    // 덮어쓰기 전에 메모리로 올려서 저장
    int result_counts = load_results_count();
    struct result* results = load_results();
    FILE* fp;

    // 현재 시간 받아오기
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);

    printf("\n\t\t\tEnter your name: ");
    scanf("%s", temp_result.name);

    // 덮어쓸 거니까, write
    fp = fopen("results.txt", "w");
    if (!fp) return;

    temp_result.point = point;
    temp_result.year = tm_info->tm_year + 1900;
    temp_result.month = tm_info->tm_mon + 1;
    temp_result.day = tm_info->tm_mday;
    temp_result.hour = tm_info->tm_hour;
    temp_result.min = tm_info->tm_min;
    temp_result.rank = 1; // 순위는 나중에 읽어들인 뒤 계산하거나 생략 가능

    if (results == NULL)
        fprintf(fp, "%d\t%s\t%ld\t%04d\t%02d\t%02d\t%02d\t%02d\n",
                temp_result.rank, temp_result.name, temp_result.point,
                temp_result.year, temp_result.month, temp_result.day,
                temp_result.hour, temp_result.min);

    else
    {
        int i;
        // 순위 높으면 그대로 출력
        for (i = 0; i < result_counts; i++)
        {
            if (results[i].point <= temp_result.point)break;
            // 파일에 쓰기 (예: 순위,이름,점수,연,월,일,시,분)
            fprintf(fp, "%d\t%s\t%ld\t%04d\t%02d\t%02d\t%02d\t%02d\n",
                    results[i].rank, results[i].name, results[i].point,
                    results[i].year, results[i].month, results[i].day,
                    results[i].hour, results[i].min);
        }
        // 내 순위 출력, 순위 = 1 + i (기존 인덱스)
        fprintf(fp, "%d\t%s\t%ld\t%04d\t%02d\t%02d\t%02d\t%02d\n",
                i + 1, temp_result.name, temp_result.point,
                temp_result.year, temp_result.month, temp_result.day,
                temp_result.hour, temp_result.min);
        for (; i < result_counts; i++)
        {
            fprintf(fp, "%d\t%s\t%ld\t%04d\t%02d\t%02d\t%02d\t%02d\n",
                    results[i].rank + 1, results[i].name, results[i].point,
                    results[i].year, results[i].month, results[i].day,
                    results[i].hour, results[i].min);
        }
    }
    fclose(fp);
    free(results);
}

/* 메뉴 표시*/
int display_menu(void)
{
    //
    int menu = 0;

    while (1)
    {
        clear_screen(); // system("clear");
        printf("\n\n\t\t\t\tText Tetris");
        printf("\n\t\t\t============================");
        printf("\n\t\t\t\tGAME MENU\t");
        printf("\n\t\t\t============================");
        printf("\n\t\t\t   1) Game Start");
        printf("\n\t\t\t   2) Search history");
        printf("\n\t\t\t   3) Record Output");
        printf("\n\t\t\t   4) QUIT");
        printf("\n\t\t\t============================");
        printf("\n\t\t\t\t\t SELECT : ");
        scanf("%d", &menu);
        if (menu >= 1 && menu <= 4)
        {
            return menu;
        }
        else
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
            {
                ; // 남은 입력 버퍼 버리기
            }
            printf("Wrong Input. Please enter right number in 1~4.\n");
            press_any_key();
        }
    }
    return 0;
}

void press_any_key(void)
{
#ifdef _WIN32
    /* Windows: _getch() 한 번 호출해서 키 입력 대기 */
    printf("\n\t\t\tPress any key to continue...");
    fflush(stdout);
    _getch();   // Enter 없이 바로 리턴
#else
    struct termios oldt, raw;
    tcgetattr(STDIN_FILENO, &oldt);

    raw = oldt;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    printf("\n\t\t\tPress any key to continue...");
    fflush(stdout);

    char ch = 0;
    read(STDIN_FILENO, &ch, 1);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
}

void search_result(void)
{
    char search_name[MAX_NAME_LEN];
    clear_screen(); // system("clear");
    printf("\n\n\t\t\t\tText Tetris");
    printf("\n\t\t\t============================");
    printf("\n\t\t\t\tSearch history");
    printf("\n\t\t\t============================");
    printf("\n\t\t\t\tEnter name : ");
    scanf("%s", search_name);

    FILE* fp;
    char name[MAX_NAME_LEN];
    long sc;
    int rank, yy, mm, dd, hh, mi;
    fp = fopen("results.txt", "r");
    if (!fp)
    {
        printf("\n\t\t\tNo records found.\n");
        press_any_key();
        return;
    }
    printf("\t\t\tRank\tName\tPoint\tDate");
    while (fscanf(fp, "%d\t%s\t%ld\t%04d\t%02d\t%02d\t%02d\t%02d",
                  &rank, name, &sc, &yy, &mm, &dd, &hh, &mi) == 8)
    {
        if (!strcmp(search_name, name))
            printf("\n\t\t\t%d\t%s\t%ld\t%04d-%02d-%02d %02d:%02d",
                   rank, name, sc, yy, mm, dd, hh, mi);
    }
    fclose(fp);
    printf("\n\t\t\t============================\n");

    press_any_key();
}

void load_best_point(void)
{
    FILE* fp;
    char name[MAX_NAME_LEN];
    long sc;
    int rank, yy, mm, dd, hh, mi;
    fp = fopen("results.txt", "r");
    if (!fp)
    {
        return;
    }
    while (fscanf(fp, "%d\t%s\t%ld\t%04d\t%02d\t%02d\t%02d\t%02d",
                  &rank, name, &sc, &yy, &mm, &dd, &hh, &mi) == 8)
    {
        if (best_point < sc)best_point = sc;
    }
    fclose(fp);
}

int load_results_count(void)
{
    int count = 0;

    FILE* fp;
    char name[MAX_NAME_LEN];
    long sc;
    int rank, yy, mm, dd, hh, mi;
    fp = fopen("results.txt", "r");
    if (!fp)
    {
        return 0;
    }
    while (fscanf(fp, "%d\t%s\t%ld\t%04d\t%02d\t%02d\t%02d\t%02d",
                  &rank, name, &sc, &yy, &mm, &dd, &hh, &mi) == 8)
    {
        count++;
    }
    fclose(fp);
    return count;
}

struct result* load_results(void)
{
    int result_counts = load_results_count();
    struct result* results = malloc(sizeof(struct result) * result_counts);
    FILE* fp;
    fp = fopen("results.txt", "r");
    if (!fp || result_counts == 0)
    {
        return NULL;
    }
    for (int i = 0; i < result_counts; ++i)
    {
        fscanf(fp, "%d\t%s\t%ld\t%04d\t%02d\t%02d\t%02d\t%02d",
               &results[i].rank, results[i].name, &results[i].point,
               &results[i].year, &results[i].month, &results[i].day,
               &results[i].hour, &results[i].min);
    }
    fclose(fp);
    return results;
}

void record_output(void)
{
    FILE* fp;
    char name[MAX_NAME_LEN];
    long point;
    int rank, yy, mm, dd, hh, mi;
    clear_screen();
    printf("\n\n\t\t\t\tText Tetris");
    printf("\n\t\t\t============================");
    printf("\n\t\t\t\tAll Records\t");
    printf("\n\t\t\t============================\n");
    fp = fopen("results.txt", "r");
    if (!fp)
    {
        printf("\n\t\t\tNo records found.\n");
        press_any_key();
        return;
    }
    printf("\t\t\tRank\tName\tPoint\tDate\n");
    while (fscanf(fp, "%d\t%s\t%ld\t%04d\t%02d\t%02d\t%02d\t%02d",
                  &rank, name, &point, &yy, &mm, &dd, &hh, &mi) == 8)
    {
        printf("\t\t\t%d\t%s\t%ld\t%04d-%02d-%02d %02d:%02d\n",
               rank, name, point, yy, mm, dd, hh, mi);
    }
    fclose(fp);
    printf("\t\t\t============================\n");
    press_any_key();
}

char get_next_block_char()
{
    switch (next_block_number)
    {
    case 0:
        return 'i';
    case 1:
        return 't';
    case 2:
        return 's';
    case 3:
        return 'z';
    case 4:
        return 'l';
    case 5:
        return 'j';
    case 6:
        return 'o';
    default:
        return '?';
    }
}

// (현재 블록이 충돌 직전에 멈출 y 좌표를 반환)
int compute_ghost_y(void)
{
    int test_y = y;
    // 충돌이 발생할 때까지 y를 한 칸씩 내림
    while (!is_collision(test_y + 1, x, block_state))
    {
        test_y++;
    }
    return test_y;
}

void hold_block(void)
{
    if (hold_used_in_turn) return; // 한 턴에 1회만 허용
    remove_block(); // 현재 블록 제거
    int temp = block_number;

    if (hold_block_number == 7)
    {
        // 처음 hold
        hold_block_number = block_number;
        set_random_block(); // next -> 현재로, 다음 블록 갱신
    }
    else
    {
        // 교체
        block_number = hold_block_number;
        hold_block_number = temp;
    }

    // 블록 상태 및 위치 초기화
    block_state = 0;
    x = 3;
    y = 0;
    place_block();
    hold_used_in_turn = 1;
}

void get_terminal_size(int* rows, int* cols)
{
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleScreenBufferInfo(hStdout, &csbi)) {
        *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
        *rows = 24;
        *cols = 80;
    }
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
    {
        *rows = 24; // fallback
        *cols = 80;
    }
    else
    {
        *rows = w.ws_row;
        *cols = w.ws_col;
    }
#endif
}

int is_terminal_size_sufficient(void)
{
    int rows, cols;
    get_terminal_size(&rows, &cols);

    // 요구되는 최소 크기
    const int MIN_ROWS = 35;
    const int MIN_COLS = 60;

    if (rows < MIN_ROWS || cols < MIN_COLS)
    {
        printf("\n\t\t\t[!] Terminal window is too small.\n");
        printf("\t\t\tMinimum required size: %d × %d\n", MIN_ROWS, MIN_COLS);
        printf("\t\t\tCurrent size: %d × %d\n", rows, cols);
        printf("\t\t\tPlease resize the terminal window.\n");
        return 0;
    }

    return 1; // 충분한 크기
}
