//
// Created by 박우현 on 25. 6. 1.
//
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>

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

// 화면 지우는 명령어
#ifdef _WIN32
  #define CLEAR_SCREEN_CMD "cls"
#else
#define CLEAR_SCREEN_CMD "clear"
#endif


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


char i_block[4][4][4] =
{
    {
        // 첫 번째 회전 상태
        {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}
    },
    {
        // 두 번째 회전 상태
        {0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1}
    },
    {
        // 세 번째 회전 상태
        {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}
    },
    {
        // 네 번째 회전 상태
        {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}
    }
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
    {{0, 1, 0, 0},
        {0, 1, 0, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0}},

    {{1, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}},

    {{1, 1, 0, 0},
        {1, 0, 0, 0},
        {1, 0, 0, 0},
        {0, 0, 0, 0}},

    {
        {1, 1, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}}
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
    char name[30];
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
int x = 4, y = 0; /*블록의 위치*/
int game = GAME_END; /*게임 시작, 게임 종료*/
int best_point = 0; /* 최고 점수*/
long point = 0; /* 현재 점수*/

/* 터미널 입출력 제어를 위한 원래 터미널 설정 저장 */
struct termios orig_termios;

/* 타이머용 변수 */
struct itimerval timer;

/* 블록 모양 포인터 배열로 묶어 두기 */
char (*blocks[7])[4][4][4] = {
    &i_block, &t_block, &s_block, &z_block, &l_block, &j_block, &o_block
};


int display_menu(void); /* 메뉴 표시*/

int game_start(void); // 게임 시작
void search_result(void); // Search history
void print_result(void); //  Record Output

void enable_raw_mode(void); // 터미널 입력 모드 non-canonical
void disable_raw_mode(void); // 터미널 입력 모드 canonical

void init_table(void); // 게임보드 초기화
void draw_table(void); // 게임보드 그리기
void clear_screen(void); // 화면 지우기
void clear_lines(void); // 줄 지우기
void lock_block(void);
void clear_line(void);
int get_key(void);
void process_key(int key);
void alarm_handler(int signum);
void init_timer(void);
void stop_timer(void);
void spawn_block(void);
void place_block(void);
void save_result(void);
void press_any_key(void);


void clear_screen(void)
{
    system(CLEAR_SCREEN_CMD);
}

int game_start(void)
{
    // 1) 터미널 설정
    enable_raw_mode();
    // 화면 완전히 지우기 (ANSI 코드)
    printf("\033[2J");
    printf("\033[H");

    // 2) 테이블 초기화
    init_table();
    srand((unsigned int)time(NULL));
    next_block_number = rand() % 7;
    spawn_block();
    place_block();

    // 3) 스크린 그리기
    draw_table();

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

    // 6) 게임 종료 처리
    stop_timer();
    disable_raw_mode();

    // 게임 오버 메시지
    printf("\nGame Over! Your score: %ld\n", point);

    if (point > best_point)
    {
        printf("\nBest Record!\n");
        best_point = point;
        // best_point 저장 로직 삽입 가능 (파일 I/O 등)
    }
    save_result();
    point = 0; // 초기화
    press_any_key();
    // 메뉴로 돌아가려면 1 또는 0 반환
    return 1;
}

void search_result(void)
{
}

void print_result(void)
{
    FILE* fp;
    char name[30];
    long sc;
    int yy, mm, dd, hh, mi;
    printf("\n-- All Records --\n");
    fp = fopen("results.txt", "r");
    if (!fp)
    {
        printf("No records found.\n");
        press_any_key();
        return;
    }
    while (fscanf(fp, "%s %ld %d %d %d %d %d",
                  name, &sc, &yy, &mm, &dd, &hh, &mi) == 7)
    {
        printf("%s\t%ld\t%04d-%02d-%02d %02d:%02d\n",
               name, sc, yy, mm, dd, hh, mi);
    }
    fclose(fp);
    printf("------------------\n");
    press_any_key();
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

    // 커서 맨 위로 이동 (ANSI escape code)
    printf("\033[H\t\t\t");

    for (i = 0; i < 21; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (tetris_table[i][j] == 0)
            {
                printf("  "); // 빈 칸 (스페이스 두 칸)
            }
            else if (tetris_table[i][j] == 1)
            {
                printf("□ "); // 벽 또는 바닥
            }
            else if (tetris_table[i][j] == 2)
            {
                printf("▦ "); // 움직이는 블록 (선택적 표시)
            }
            else if (tetris_table[i][j] == 3)
            {
                printf("▢ "); // 고정된 블록 (선택적 구분)
            }
        }
        printf("\n\t\t\t");
    }
    // 점수 등 정보 출력
    printf("Score: %ld   Next: %d\n", point, next_block_number);
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
    // 바닥(가장 아래 행, 1) 설정
    for (j = 0; j < 10; j++)
    {
        tetris_table[20][j] = 1;
    }
}

void spawn_block(void)
{
    block_number = next_block_number;
    next_block_number = rand() % 7;
    block_state = 0;
    x = 4;
    y = 0;
}

/* 현재 블록을 테트리스판에 표시(값 2) */
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

/* 현재 블록을 테트리스판에서 지우기(값 0) */
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
                // 테이블 경계(인덱스) 벗어나면 충돌
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
        // 더 아래로 못 가면 잠금(lock)
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
    clear_lines(); // 줄이 완성됐는지 확인 후 삭제
    spawn_block(); // 다음 블록 생성
    if (is_collision(y, x, block_state))
    {
        // 새로 생성된 블록이 바로 충돌한다면 → 게임 오버
        game = GAME_END;
    }
    else
    {
        place_block(); // 새 블록 화면에 표시
    }
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
            // 맨 위 행은 모두 0(빈칸)으로 초기화, 벽/바닥은 그대로 놔둠
            for (col = 1; col < 9; col++)
            {
                tetris_table[0][col] = 0;
            }
            point += 100; // 한 줄 삭제 시 점수 추가 (임의로 100점)
        }
    }
}

int get_key(void)
{
    char c;
    int n = read(STDIN_FILENO, &c, 1);
    if (n == 1)
    {
        return (int)c;
    }
    return -1;
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
}

/* SIGALRM 시그널 핸들러 */
void alarm_handler(int signum)
{
    // 블록을 아래로 한 칸 이동시키거나, 충돌 시 고정
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

void save_result(void)
{
    FILE* fp;
    fp = fopen("results.txt", "a");
    if (!fp) return;

    // 현재 시간 받아오기
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);

    printf("Enter your name: ");
    scanf("%s", temp_result.name);

    temp_result.point = point;
    temp_result.year = tm_info->tm_year + 1900;
    temp_result.month = tm_info->tm_mon + 1;
    temp_result.day = tm_info->tm_mday;
    temp_result.hour = tm_info->tm_hour;
    temp_result.min = tm_info->tm_min;
    temp_result.rank = 0; // 순위는 나중에 읽어들인 뒤 계산하거나 생략 가능

    // 파일에 쓰기 (예: 이름,점수,연,월,일,시,분)
    fprintf(fp, "%s %ld %04d %02d %02d %02d %02d\n",
            temp_result.name, temp_result.point,
            temp_result.year, temp_result.month, temp_result.day,
            temp_result.hour, temp_result.min);
    fclose(fp);
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
        printf("\n\t\t\t\tGAME MENU\t\n");
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
                ; // 남은 입력 버리기
            }
            printf("Wrong Input. Please enter right number in 1~4.\n");
            press_any_key();
        }
    }
    return 0;
}


/// 테트리스 게임 메인 함수
/// 메뉴를 표시하고 사용자의 선택에 따라 게임을 시작하거나 결과를 검색하거나 종료합니다.
/// @param
/// @return
int main(void)
{
    int menu = 1;

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
            print_result();
        }
        else if (menu == 4)
        {
            exit(0);
        }
    }

    return 0;
}

void press_any_key(void) {
    struct termios oldt, raw;

    // 1) 현재 터미널 설정을 가져와서 저장
    tcgetattr(STDIN_FILENO, &oldt);

    // 2) raw 모드 설정 복사본 생성
    raw = oldt;
    //    ICANON을 끄면 입력 버퍼링(엔터 대기)이 해제되고,
    //    ECHO를 끄면 키를 누를 때 화면에 출력되지 않음
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;   // 최소 1글자만 읽어도 바로 리턴
    raw.c_cc[VTIME] = 0;  // 타임아웃 없음(무한 대기)

    // 3) 터미널을 raw 모드로 변경
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    // 4) 사용자에게 메시지 출력
    printf("Press any key to continue...");
    fflush(stdout);

    // 5) 한 글자만 읽어오기 (엔터 없이 바로 리턴)
    char ch = 0;
    read(STDIN_FILENO, &ch, 1);

    // 6) 원래 설정으로 복구
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    // (원하는 경우 눌린 키에 따라 추가 동작을 해도 되고,
    //   여기서는 단순히 아무 키나 누르면 빠져나오도록 함.)
}


// TODO: 프로그램 실행시