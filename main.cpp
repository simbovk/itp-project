#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <ctime>
using namespace std;
/*
 * clui library, by nima heydari nasab
 * @radio_nima
 * and some changes by roozbeh sharifnasab
 * @rsharifnasab
 * and parsa noori
 * @parsanoori
 */

#ifndef _CLUI
#define _CLUI

/*
 * common includes
 */
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * includes for windows
 */
#if defined(_WIN32) || defined(_WIN64) || defined(WI32)

#define OS_UNIX 0
#include <Windows.h>
#include <conio.h>

#define _NO_OLDNAMES // for MinGW compatibility
#define getch _getch
#define kbhit _kbhit

/*
 * includes for unix-like systems
 * MacOS or linux
 */
#else

#define OS_UNIX 1
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#endif

/*
 * define colors that can be chosen
 * in change_color function
 */
#define COLOR_DEFAULT 0
#define COLOR_RED 1
#define COLOR_BOLD_RED 2
#define COLOR_GREEN 3
#define COLOR_BOLD_GREEN 4
#define COLOR_YELLOW 5
#define COLOR_BOLD_YELLOW 6
#define COLOR_BLUE 7
#define COLOR_BOLD_BLUE 8
#define COLOR_MAGENTA 9
#define COLOR_BOLD_MAGENTA 10
#define COLOR_CYAN 11
#define COLOR_BOLD_CYAN 12

/*
 * colors added in version2
 * this is use 8 bit terminal
 */
#define COLOR_ORANGE_1 13
#define COLOR_ORANGE_2 14
#define COLOR_ORANGE_3 15

/*
 * define colors that can be chosen
 * in change_background_color function
 */
#define BG_DEFAULT 0
#define BG_BLUE 1
#define BG_CYAN 2
#define BG_GREEN 3
#define BG_PURPLE 4
#define BG_RED 5
#define BG_WHITE 6
#define BG_YELLOW 7

/*
 * the file stdout is line buffered hence
 * if you want to print out to terminal and
 * remain in the same line you should
 * use this function
 */
void flush()
{
    fflush(stdout);
    fflush(stderr);
}

void __enable_raw_mode()
{
#if OS_UNIX
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &term);
#else

#endif
}

void __disable_raw_mode()
{
#if OS_UNIX
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
#else

#endif
}

/*
 * A cross platform functin to detect
 * whether keyboard is hit
 */
bool is_keyboard_hit()
{
#if OS_UNIX
    static struct termios oldt, newt;
    int cnt_easy = 0;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_iflag = 0;     // input mode
    newt.c_oflag = 0;     // output mode
    newt.c_cc[VMIN] = 1;  // minimum time to wait
    newt.c_cc[VTIME] = 1; // minimum characters to wait for
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ioctl(0, FIONREAD, &cnt_easy); // Read count
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100;
    select(STDIN_FILENO + 1, NULL, NULL, NULL, &tv); // A small time delay
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return cnt_easy > 0;
#else
    return kbhit();
#endif
}

/*
 * since getchar is a nonstandard function
 * this funtion was implemented.
 * Note that windows has this function
 * implicitly
 */
#if OS_UNIX
int getch()
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON /* | ECHO */);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}
#endif

int nb_getch(void)
{
    if (is_keyboard_hit())
        return getch();
    else
        return 0;
}

/*
 * Simply clears the terminal screen
 * Uses OS'es command suited for this
 * action to prevent ANSI's problems on
 * different OSes
 */
void clear_screen()
{
#if OS_UNIX
    system("clear");
#else
    system("CLS");
#endif
}

/*
 * NOTE: NEEDS ANSI SUPPORT
 * change output color of terminal
 * by RGB input
 * this will only work for 24-bit terminals
 * tested in linux terminals
 * and new Windows10 cmd and powershell
 */
void change_color_rgb(int r, int g, int b)
{
    if (0 <= r && r <= 255
        && 0 <= g && g <= 255
        && 0 <= b && b <= 255){
        printf("\033[38;2;%d;%d;%dm", r, g, b);
        flush();
    }

}

/*
 * NOTE: NEEDS ANSI SUPPORT
 * change color in 8 bit terminal
 */
void change_color(int color)
{

    const char* colors[] = { "[0m", "[0;31m", "[1;31m", "[0;32m",
                             "[1;32m", "[0;33m", "[1;33m", "[0;34m",
                             "[1;34m", "[0;35m", "[1;35m", "[0;36m",
                             "[1;36m", "[38;5;202m", "[38;5;208m", "[38;5;214m" };

    if (color >= 0 && color <= 15) {
        printf("\033");
        printf("%s", colors[color]);
    }
    flush();
}

void change_background_color(int color)
{
    const char* colors[] = { "[40m", "[44m", "[46m", "[42m",
                             "[45m", "[41m", "[47m", "[43m" };

    if (color >= 0 && color <= 7) {
        printf("\033");
        printf("%s", colors[color]);
    }
    flush();
}

/*
 * NOTE: NEEDS ANSI SUPPORT
 */
void reset_color() {
    change_color(COLOR_DEFAULT);
}

/*
 * invoke when you're done with this library
 */
void quit()
{
    reset_color();
    __disable_raw_mode();
    clear_screen();
    exit(0);
}

static void sigint_handler(int dummy)
{
    // to get rid of unused-parameter warning
    (void)dummy;
    quit();
}

/*
 * library
 */
void init_clui()
{
    clear_screen();
    signal(SIGINT, sigint_handler);
    __enable_raw_mode();
    flush();
}

/*
 * a cross platform function to
 * suspends execution for some
 * milli seconds
 */
void delay(size_t milli_seconds)
{
    flush();
#if OS_UNIX
    usleep(1000 * milli_seconds);
#else
    Sleep(milli_seconds);
#endif
}

/*
 * returns the windows rows
 */
int get_window_rows()
{
#if OS_UNIX
    struct winsize max;
    ioctl(0, TIOCGWINSZ, &max);
    return (int)max.ws_row;
#else
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return (int)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
#endif
}

/*
 * returns the window cols
 */
int get_window_cols()
{
#if OS_UNIX
    struct winsize max;
    ioctl(0, TIOCGWINSZ, &max);
    return (int)max.ws_col;
#else
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return (int)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
#endif
}
/*
 * NOTE: NEEDS ANSI SUPPORT
 * gives the x of the cursor
 * linux version might be a liitle slow
 */
int get_cursor_x()
{
    flush();
    int x = 0;
#if OS_UNIX
    system("echo -ne \"\\033[6n\";\
            read -s -d\\[ garbage;\
            read -s -d R cursor_loc;\
            echo $cursor_loc > /tmp/cursor_pos");
    FILE* fp = fopen("/tmp/cursor_pos", "r");
    if (!fp) {
        return 0;
    }
    int dummy;
    fscanf(fp, "%d;%d", &x, &dummy);
    fclose(fp);
#else
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    x = csbi.dwCursorPosition.Y + 1;
#endif
    return x;
}

/*
 * NOTE: NEEDS ANSI SUPPORT
 * gives the x of the cursor
 * linux version might be a little slow!
 */
int get_cursor_y()
{
    flush();
    int y = 0;
#if OS_UNIX
    system("echo -ne \"\\033[6n\";\
            read -s -d\\[ garbage;\
            read -s -d R cursor_loc;\
            echo $cursor_loc > /tmp/cursor_pos");
    FILE* fp = fopen("/tmp/cursor_pos", "r");
    if (!fp) {
        return 0;
    }
    int dummy;
    fscanf(fp, "%d;%d", &dummy, &y);
    fclose(fp);
#else
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    y = csbi.dwCursorPosition.X + 1;
#endif
    return y;
}

/*
 * NOTE: NEEDS ANSI SUPPORT
 * moves cursor up n times
 */
// this define will solve backward compability issue
#define corsur_up cursor_up
void cursor_up(int n) {
    printf("\033[%dA", n);
    flush();
}

/*
 * NOTE: NEEDS ANSI SUPPORT
 * moves cursor down n times
 */
void cursor_down(int n) {
    printf("\033[%dB", n);
    flush();
}
/*
 * NOTE: NEEDS ANSI SUPPORT
 * moves cursor forward n time
 */
void cursor_forward(int n) {
    printf("\033[%dC", n);
    flush();
}

/*
 * NOTE: NEEDS ANSI SUPPORT
 * moves cursor backwards n time
 */
void cursor_backward(int n)
{
    printf("\033[%dD", n);
    flush();
}

/*
 * NOTE: NEEDS ANSI SUPPORT
 * moves cursor to the given position
 */
void cursor_to_pos(int row, int col)
{
    printf("\033[%d;%dH", row, col);
    flush();
}

/*
 * NOTE: NEEDS ANSI SUPPORT
 * saves cursor position for further use
 */
void save_cursor() {
    printf("\0337");
    flush();
}

/*
 * NOTE: NEEDS ANSI SUPPORT
 * restors cursor to the last saved
 * position
 */
void restore_cursor() {
    printf("\0338");
    flush();
}

/*
 * NOTE: NEEDS ANSI SUPPORT
 * plays beep! :)
 */
void play_beep() {
    printf("\07");
    flush();
}

#endif
struct level{
    int level;
}l1;
struct save{
    clock_t time_start;
    char cnt[21][21];
    char cnt_mine[21][21];
    int counter;
    int point;
    char username[1000];
}s1 , s2 , s3 , s4;
struct user{
    char username[1000];
    int point;
};
user u[10];
int flag_save = 0;
int row_user , col_user;
char cnt_user[21][21];
char cnt_mine_user[21][21];
int counter_user = 0;
int  point = 0;
char cnt_name[1000];
char cnt_regular[13][13];
char cnt_hard[21][21];
char cnt_mine_regular[13][13];
char cnt_mine_hard[21][21];
char cnt_mine_easy[6][6];// number & mine
char cnt_easy[6][6]; // board
int counter_easy = 0;
int counter_regular = 0;
int counter_hard = 0;
void give_easy(char cnt[][6]);
void give_regular(char cnt[][13]);
void give_hard(char cnt[][21]);
void give_user();
void board_easy(char cnt[][6]);
void board_regular(char cnt[][13]);
void board_hard(char cnt[][21]);
void board_user();
void minesweeper_easy(bool is_loaded);
void minesweeper_regular(bool is_loaded);
void minesweeper_hard(bool is_loaded);
void minesweeper_user(bool is_loaded);
void minesweeper_user_save(bool is_loaded);
void change_easy(int index1, int index2);
void change_regular(int index1, int index2);
void save_name(char user[]);
void change_hard(int index1, int index2);
void change_user(int index1, int index2);
void mine_easy();
bool check_name(char user[]);
void mine_regular();
void mine_hard();
void mine_user();
int return_idx(char user[]);
void save_point(int idx);
void title();
int x_easy , y_easy;
int x_regular , y_regular;
int x_hard , y_hard;
int x_user , y_user;
char key_easy1;
char key_regular1;
char key_hard1;
char key_user1;
int size_of_cnt(char list[]);
int num_1 , num_2;
int bomb_user;
clock_t time_start_easy;
clock_t time_start_regular;
clock_t time_start_hard;
clock_t time_start_user;
int player_count = 0;
void swap(int &x, int &y) {
    int temp = x;
    x = y;
    y = temp;
}
void bubble_sort(user a[], int size) {
    for (int i = 0; i < size - 1; i++)
        for (int j = 0; j < size - 1 - i; j++)
            if (a[j + 1].point > a[j].point){
                swap(a[j].point, a[j + 1].point);
                char temp[1000];
                for (int k = 0; k < 1000; ++k) {
                    temp[k] = a[j + 1].username[k];
                }
                for (int k = 0; k < 1000; ++k) {
                     a[j + 1].username[k] = a[j].username[k];
                }
                for (int k = 0; k < 1000; ++k) {
                    a[j].username[k] = temp[k];
                }
            }

}
void menu(){
    while(true) {
        char reg;
        int game;
        int rating;
        change_color_rgb(255, 5, 5);
        cout << "                             " << "***welcome to minesweeper***" << endl;
        title();
        change_color_rgb(0, 255, 21);
        cout << "1.New Game    2.Load Game     3.Change Name    4.Leaderboard      5.Quit" << endl;
        cout << "Choose : ";
        cin >> game;
        if (game == 1) {
            point = 0;
            cout << "Did You Register Your name before?(Y/N)" << endl;
            cin >> reg;
            if (reg == 'Y') {
                char cont; // continue
                cout << "Continue With Your Current Name?(Y/N)";
                cin >> cont;
                if (cont == 'Y') {}
                else if (cont == 'N') {
                    point = 0;
                    player_count++;
                    cout << endl;
                    cout << "Enter Your New Name:";
                    cin >> cnt_name;
                    cout << endl;
                    save_name(cnt_name);
                    clear_screen();
                }
            }
            else if (reg == 'N') {
                point = 0;
                player_count++;
                cout << "Enter Your Name:";
                cin >> cnt_name;
                save_name(cnt_name);
                cout << endl;
                cout << "welcome;)" << ",";
                cout << cnt_name << endl;
                delay(500);
                clear_screen();
            }
            cout << endl;
            clear_screen();
            cout << "DIFFICULTY: 1.easy        2.regular          3.hard         4.User Suggestion" << endl;
            cout << "ENTER DIFFICULTY:";
            cin >> rating;
            l1.level = rating;
            clear_screen();
            if (rating == 1)
                minesweeper_easy(false);
            else if (rating == 2)
                minesweeper_regular(false);
            else if (rating == 3)
                minesweeper_hard(false);
            else if (rating == 4)
                minesweeper_user(false);
        }
        else if (game == 2 ) {
            if(flag_save == 0){
                cout << "You dont have any save!" << endl;
                delay(1000);
                clear_screen();
                menu();
            }
            else if (flag_save == 1) {
                int save;
                clear_screen();
                cout << "1.save1     2.save2     3.save3     4.save4" << endl;
                cout << "choose :";
                cin >> save;
                clear_screen();
                if (save == 1) {
                    if (l1.level == 1) {
                        for (int i = 0; i < 6; ++i) {
                            for (int j = 0; j < 6; ++j) {
                                cnt_mine_easy[i][j] = s1.cnt_mine[i][j];
                                cnt_easy[i][j] = s1.cnt[i][j];
                            }
                        }
                        counter_easy = s1.counter;
                        time_start_easy = s1.time_start;
                        int size = size_of_cnt(s1.username);
                        for (int i = 0; i < size; ++i) {
                            cnt_name[i] = s1.username[i];
                        }
                        point = s1.point;
                        minesweeper_easy(true);
                    } else if (l1.level == 2) {
                        for (int i = 0; i < 13; ++i) {
                            for (int j = 0; j < 13; ++j) {
                                cnt_mine_regular[i][j] = s1.cnt_mine[i][j];
                                cnt_regular[i][j] = s1.cnt[i][j];
                            }
                        }
                        counter_regular = s1.counter;
                        time_start_regular = s1.time_start;
                        int size = size_of_cnt(s1.username);
                        for (int i = 0; i < size; ++i) {
                            cnt_name[i] = s1.username[i];
                        }
                        point = s1.point;
                        minesweeper_regular(true);
                    } else if (l1.level == 3) {
                        for (int i = 0; i < 21; ++i) {
                            for (int j = 0; j < 21; ++j) {
                                cnt_mine_hard[i][j] = s1.cnt_mine[i][j];
                                cnt_hard[i][j] = s1.cnt[i][j];
                            }
                        }
                        counter_hard = s1.counter;
                        time_start_hard = s1.time_start;
                        int size = size_of_cnt(s1.username);
                        for (int i = 0; i < size; ++i) {
                            cnt_name[i] = s1.username[i];
                        }
                        point = s1.point;
                        minesweeper_hard(true);
                    } else if (l1.level == 4) {
                        for (int i = 0; i < col_user; ++i) {
                            for (int j = 0; j < row_user; ++j) {
                                cnt_mine_user[i][j] = s1.cnt_mine[i][j];
                                cnt_user[i][j] = s1.cnt[i][j];
                            }
                        }
                        counter_user = s1.counter;
                        time_start_user = s1.time_start;
                        int size = size_of_cnt(s1.username);
                        for (int i = 0; i < size; ++i) {
                            cnt_name[i] = s1.username[i];
                        }
                        point = s1.point;
                        minesweeper_user_save(true);
                    }
                }
                if (save == 2) {
                    if (l1.level == 1) {
                        for (int i = 0; i < 6; ++i) {
                            for (int j = 0; j < 6; ++j) {
                                cnt_mine_easy[i][j] = s2.cnt_mine[i][j];
                                cnt_easy[i][j] = s2.cnt[i][j];
                            }
                        }
                        counter_easy = s2.counter;
                        time_start_easy = s2.time_start;
                        int size = size_of_cnt(s2.username);
                        for (int i = 0; i < size; ++i) {
                            cnt_name[i] = s2.username[i];
                        }
                        point = s2.point;
                        minesweeper_easy(true);
                    } else if (l1.level == 2) {
                        for (int i = 0; i < 13; ++i) {
                            for (int j = 0; j < 13; ++j) {
                                cnt_mine_regular[i][j] = s2.cnt_mine[i][j];
                                cnt_regular[i][j] = s2.cnt[i][j];
                            }
                        }
                        counter_regular = s2.counter;
                        time_start_regular = s2.time_start;
                        int size = size_of_cnt(s2.username);
                        for (int i = 0; i < size; ++i) {
                            cnt_name[i] = s2.username[i];
                        }
                        point = s2.point;
                        minesweeper_regular(true);
                    } else if (l1.level == 3) {
                        for (int i = 0; i < 21; ++i) {
                            for (int j = 0; j < 21; ++j) {
                                cnt_mine_hard[i][j] = s2.cnt_mine[i][j];
                                cnt_hard[i][j] = s2.cnt[i][j];
                            }
                        }
                        counter_hard = s2.counter;
                        time_start_hard = s2.time_start;
                        int size = size_of_cnt(s2.username);
                        for (int i = 0; i < size; ++i) {
                            cnt_name[i] = s2.username[i];
                        }
                        point = s2.point;
                        minesweeper_hard(true);
                    } else if (l1.level == 4) {
                        for (int i = 0; i < col_user; ++i) {
                            for (int j = 0; j < row_user; ++j) {
                                cnt_mine_user[i][j] = s2.cnt_mine[i][j];
                                cnt_user[i][j] = s2.cnt[i][j];
                            }
                        }
                        counter_user = s2.counter;
                        time_start_user = s2.time_start;
                        int size = size_of_cnt(s2.username);
                        for (int i = 0; i < size; ++i) {
                            cnt_name[i] = s2.username[i];
                        }
                        point = s2.point;
                        minesweeper_user_save(true);
                    }
                }
                if (save == 3) {
                    if (l1.level == 1) {
                        for (int i = 0; i < 6; ++i) {
                            for (int j = 0; j < 6; ++j) {
                                cnt_mine_easy[i][j] = s3.cnt_mine[i][j];
                                cnt_easy[i][j] = s3.cnt[i][j];
                            }
                        }
                        counter_easy = s3.counter;
                        time_start_easy = s3.time_start;
                        int size = size_of_cnt(s3.username);
                        for (int i = 0; i < size; ++i) {
                            cnt_name[i] = s3.username[i];
                        }
                        point = s3.point;
                        minesweeper_easy(true);
                    } else if (l1.level == 2) {
                        for (int i = 0; i < 13; ++i) {
                            for (int j = 0; j < 13; ++j) {
                                cnt_mine_regular[i][j] = s3.cnt_mine[i][j];
                                cnt_regular[i][j] = s3.cnt[i][j];
                            }
                        }
                        counter_regular = s3.counter;
                        time_start_regular = s3.time_start;
                        int size = size_of_cnt(s3.username);
                        for (int i = 0; i < size; ++i) {
                            cnt_name[i] = s3.username[i];
                        }
                        point = s3.point;
                        minesweeper_regular(true);
                    } else if (l1.level == 3) {
                        for (int i = 0; i < 21; ++i) {
                            for (int j = 0; j < 21; ++j) {
                                cnt_mine_hard[i][j] = s3.cnt_mine[i][j];
                                cnt_hard[i][j] = s3.cnt[i][j];
                            }
                        }
                        counter_hard = s3.counter;
                        time_start_hard = s3.time_start;
                        int size = size_of_cnt(s3.username);
                        for (int i = 0; i < size; ++i) {
                            cnt_name[i] = s3.username[i];
                        }
                        point = s3.point;
                        minesweeper_hard(true);
                    } else if (l1.level == 4) {
                        for (int i = 0; i < col_user; ++i) {
                            for (int j = 0; j < row_user; ++j) {
                                cnt_mine_user[i][j] = s3.cnt_mine[i][j];
                                cnt_user[i][j] = s3.cnt[i][j];
                            }
                        }
                        counter_user = s3.counter;
                        time_start_user = s3.time_start;
                        int size = size_of_cnt(s3.username);
                        for (int i = 0; i < size; ++i) {
                            cnt_name[i] = s3.username[i];
                        }
                        point = s3.point;
                        minesweeper_user_save(true);
                    }
                }
                if (save == 4) {
                    if (l1.level == 1) {
                        for (int i = 0; i < 6; ++i) {
                            for (int j = 0; j < 6; ++j) {
                                cnt_mine_easy[i][j] = s4.cnt_mine[i][j];
                                cnt_easy[i][j] = s4.cnt[i][j];
                            }
                        }
                        counter_easy = s4.counter;
                        time_start_easy = s4.time_start;
                        int size = size_of_cnt(s4.username);
                        for (int i = 0; i < size; ++i) {
                            cnt_name[i] = s4.username[i];
                        }
                        point = s4.point;
                        minesweeper_easy(true);
                    } else if (l1.level == 2) {
                        for (int i = 0; i < 13; ++i) {
                            for (int j = 0; j < 13; ++j) {
                                cnt_mine_regular[i][j] = s4.cnt_mine[i][j];
                                cnt_regular[i][j] = s4.cnt[i][j];
                            }
                        }
                        counter_regular = s4.counter;
                        time_start_regular = s4.time_start;
                        int size = size_of_cnt(s4.username);
                        for (int i = 0; i < size; ++i) {
                            cnt_name[i] = s4.username[i];
                        }
                        point = s4.point;
                        minesweeper_regular(true);
                    } else if (l1.level == 3) {
                        for (int i = 0; i < 21; ++i) {
                            for (int j = 0; j < 21; ++j) {
                                cnt_mine_hard[i][j] = s4.cnt_mine[i][j];
                                cnt_hard[i][j] = s4.cnt[i][j];
                            }
                        }
                        counter_hard = s4.counter;
                        time_start_hard = s4.time_start;
                        int size = size_of_cnt(s4.username);
                        for (int i = 0; i < size; ++i) {
                            cnt_name[i] = s4.username[i];
                        }
                        point = s4.point;
                        minesweeper_hard(true);
                    } else if (l1.level == 4) {
                        for (int i = 0; i < col_user; ++i) {
                            for (int j = 0; j < 21; row_user) {
                                cnt_mine_user[i][j] = s4.cnt_mine[i][j];
                                cnt_user[i][j] = s4.cnt[i][j];
                            }
                        }
                        counter_user = s4.counter;
                        time_start_user = s4.time_start;
                        int size = size_of_cnt(s4.username);
                        for (int i = 0; i < size; ++i) {
                            cnt_name[i] = s4.username[i];
                        }
                        point = s4.point;
                        minesweeper_user_save(true);
                    }
                }
            }
        }
        else if (game == 3) {
            while(true) {
                cout << "Enter Your New Name:";
                cin >> cnt_name;
                if (check_name(cnt_name)) {
                    point = 0;
                    player_count++;
                    cout << endl;
                    save_name(cnt_name);
                    clear_screen();
                    menu();
                }
                else{
                    char a;
                    cout << "this name is a duplicate , are you sure you want to continue ?(Y/N):";
                    cin >> a;
                    if(a == 'Y'){
                        clear_screen();
                        point += u[player_count].point;
                        menu();
                    }
                    else if(a == 'N'){
                        clear_screen();
                        continue;
                    }
                }
            }
        } else if (game == 4) {
            clear_screen();
            char back;
            bubble_sort(u, player_count);
            cout << "LEADERBOARD" << endl;
            cout << endl;
            cout << endl;
            for (int i = 0, j = 1; i < player_count; ++i, j++) {
                    cout << j << "." << u[i].username << ":" << u[i].point << endl;
            }
            cout << endl;
            cout << "For Back To Menu PLease Enter Q:";
            if(cin >> back){
                clear_screen();
                continue;
            }
        }
        else if (game == 5) {
            char Quit[17] = {'S' , 'e' , 'e' ,' ' ,'Y' , 'o' ,'u' ,' ', 'N' , 'e' , 'x' , 't' , ' ' , 'T' , 'i' , 'm' , 'e'};
            for (int i = 0; i < 17; ++i) {
                delay(75);
                cout << Quit[i];
            }
            delay(500);
            quit();
        }
    }
}

void save_point(int idx){
    u[player_count - 1].point += point;
}
bool check_name(char user[]){
    int flag = 1;
    int size = size_of_cnt(user);
    for (int i = 0; i < player_count; ++i) {
        for (int j = 0; j < size; ++j) {
            if(u[i].username[j] != user[j])
                flag = 0;
        }
    }
    if(flag == 1)
        return false;
    else
        return true;

}
void save_name(char user[] ){
    int size = size_of_cnt(user);
    for (int i = 0; i < size; ++i) {
        u[player_count - 1].username[i] = user[i];
    }
}
int size_of_cnt(char list[]){
    int i = 0, size = 0 ;
    while ( list[i] != '\0')
    {
        size++;
        i++;
    }
    return size;
}
void check_empty_number_user(int idx1 , int idx2){
    for (int j = idx1 - 1; j <= idx1 + 1; ++j) {
        for (int k = idx2 - 1; k <= idx2 + 1; ++k) {
            if (k > 0 && j > 0 && k < col_user && j < row_user) {
                if (j != idx1 || k != idx2) {
                    if (cnt_mine_user[idx1][idx2] == '0')
                        change_user(j , k);
                }
            }
        }
    }
}
void check_empty_number_easy(int idx1 , int idx2){
    for (int j = idx1 - 1; j <= idx1 + 1; ++j) {
        for (int k = idx2 - 1; k <= idx2 + 1; ++k) {
            if (k > 0 && j > 0 && k < 6 && j < 6) {
                if (j != idx1 || k != idx2) {
                    if (cnt_mine_easy[idx1][idx2] == '0')
                        change_easy(j , k);
                }
            }
        }
    }
}
void check_empty_number_regular(int idx1 , int idx2){
    for (int j = idx1 - 1; j <= idx1 + 1; ++j) {
        for (int k = idx2 - 1; k <= idx2 + 1; ++k) {
            if (k > 0 && j > 0 && k < 13 && j < 13) {
                if (j != idx1 || k != idx2) {
                    if (cnt_mine_regular[idx1][idx2] == '0')
                        change_regular(j , k);
                }
            }
        }
    }
}
void check_empty_number_hard(int idx1 , int idx2){
    for (int j = idx1 - 1; j <= idx1 + 1; ++j) {
        for (int k = idx2 - 1; k <= idx2 + 1; ++k) {
            if (k > 0 && j > 0 && k < 21 && j < 21) {
                if (j != idx1 || k != idx2) {
                    if (cnt_mine_hard[idx1][idx2] == '0')
                        change_hard(j , k);
                }
            }
        }
    }
}
int check_easy(){// check right position and winning
    counter_easy = 0;
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 6; ++j) {
            if (cnt_easy[i][j] == '$' || cnt_easy[i][j] == 'F' )
                counter_easy++;
        }
    }
}
int check_regular(){// check right position and winning
    counter_regular = 0;
    for (int i = 0; i < 13; ++i) {
        for (int j = 0; j < 13; ++j) {
            if (cnt_regular[i][j] == '$' || cnt_regular[i][j] == 'F' )
                counter_regular++;
        }
    }
}
int check_hard(){// check right position and winning
    counter_hard = 0;
    for (int i = 0; i < 21; ++i) {
        for (int j = 0; j < 21; ++j) {
            if (cnt_hard[i][j] == '$' || cnt_hard[i][j] == 'F' )
                counter_hard++;
        }
    }
}
int check_user(){// check right position and winning
    counter_user = 0;
    for (int i = 0; i < col_user; ++i) {
        for (int j = 0; j < row_user; ++j) {
            if (cnt_user[i][j] == '$' || cnt_user[i][j] == 'F' )
                counter_user++;
        }
    }
}
void title(){
    change_color_rgb(255, 5, 5);
    for (int i = 0; i < 2; ++i) {
        cout << endl;
    }
    cout << cnt_name << endl;
    change_color_rgb(15, 252, 3);
    cout << "w => up" << endl;
    cout << "A => left" << endl;
    cout << "S => down" << endl;
    cout << "D => right" << endl;
    cout << "F => flag & Remove flag" << endl;
    cout << "O for save" << endl;
    cout << "press space for check your Selection" << endl;
    cout << "Q for quit and return to menu" << endl;
    change_color_rgb(74, 245, 27);
    cout << "    #" << endl;
    cout << "   ###" << endl;
    cout << "  #####" << endl;
    cout << " #######"<<endl;
    cout << "#########"<< endl;
    cout << "   |||  "<<endl;
    change_color_rgb(255, 3, 3);
    cout << "merry christmas ;) , wish you all the best . "<< endl;
    cout << endl;
    cout << endl;
}
void flag_easy (char user){// flag
        if(cnt_easy[num_1][num_2] == 'F')
            cnt_easy[num_1][num_2]='$';
        else if(cnt_easy[num_1][num_2] =='$')
            cnt_easy[num_1][num_2] = 'F';
}
void flag_regular (char user){// flag
    if(cnt_regular[num_1][num_2] == 'F')
        cnt_regular[num_1][num_2]='$';
    else if(cnt_regular[num_1][num_2] =='$')
        cnt_regular[num_1][num_2] = 'F';
}
void flag_hard (char user){// flag
    if(cnt_hard[num_1][num_2] == 'F')
        cnt_hard[num_1][num_2]='$';
    else if(cnt_hard[num_1][num_2] =='$')
        cnt_hard[num_1][num_2] = 'F';
}
void flag_user (char user){// flag
    if(cnt_user[num_1][num_2] == 'F')
        cnt_user[num_1][num_2]='$';
    else if(cnt_user[num_1][num_2] =='$')
        cnt_user[num_1][num_2] = 'F';
}
void failed_position_easy() {
    int flag = 0;
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 6; ++j) { // failed position
            if (cnt_easy[i][j] == '*')//n
            {
                flag = 1;
                change_color_rgb(250, 0, 0);
                play_beep();
                char failed[8] = {'F', 'a', 'i', 'l', 'e', 'd', ':', '|'};
                for (int k = 0; k < 8; ++k) {
                    delay(75);
                    cout << failed[k];
                }
                delay(500);
                clear_screen();
                menu();
                cout << endl;
                change_easy(num_1, num_2);
            }


        check_easy();
        if (flag == 0 && counter_easy == 4) {// win position
            change_color_rgb(38, 255, 5);
            cout << "YOU WON!,congrats ;)" << endl;
            delay(500);
            clear_screen();
            menu();
        }
     }
    }

}
void failed_position_regular() {
    int flag = 0;
    for (int i = 0; i < 13; ++i) {
        for (int j = 0; j < 13; ++j) { // failed position
            if (cnt_regular[i][j] == '*')//n
            {
                flag = 1;
                change_color_rgb(250, 0, 0);
                play_beep();
                char failed[8] = {'F', 'a', 'i', 'l', 'e', 'd', ':', '|'};
                for (int k = 0; k < 8; ++k) {
                    delay(75);
                    cout << failed[k];
                }
                delay(500);
                clear_screen();
                menu();
                cout << endl;
                change_regular(num_1, num_2);
            }
            check_regular();
            if (flag == 0 && counter_regular == 28) {// win position
                change_color_rgb(38, 255, 5);
                cout << "YOU WON!,congrats ;)" << endl;
                delay(500);
                clear_screen();
                menu();
            }
        }
    }
}
void failed_position_hard() {
    int flag = 0;
    for (int i = 0; i < 21; ++i) {
        for (int j = 0; j < 21; ++j) { // failed position
            if (cnt_hard[i][j] == '*')//n
            {
                flag = 1;
                change_color_rgb(250, 0, 0);
                play_beep();
                char failed[8] = {'F', 'a', 'i', 'l', 'e', 'd', ':', '|'};
                for (int k = 0; k < 8; ++k) {
                    delay(75);
                    cout << failed[k];
                }
                delay(500);
                clear_screen();
                menu();
                cout << endl;
                change_hard(num_1, num_2);
            }
            check_hard();
            if (flag == 0 && counter_hard == 96) {// win position
                change_color_rgb(38, 255, 5);
                cout << "YOU WON!,congrats ;)" << endl;
                delay(500);
                clear_screen();
                menu();
            }
        }
    }
}
void failed_position_user() {
    int flag = 0;
    for (int i = 0; i < col_user; ++i) {
        for (int j = 0; j < row_user; ++j) { // failed position
            if (cnt_user[i][j] == '*')//n
            {
                flag = 1;
                change_color_rgb(250, 0, 0);
                play_beep();
                char failed[8] = {'F', 'a', 'i', 'l', 'e', 'd', ':', '|'};
                for (int k = 0; k < 8; ++k) {
                    delay(75);
                    cout << failed[k];
                }
                delay(500);
                clear_screen();
                menu();
                cout << endl;
                change_user(num_1, num_2);
            }
        check_user();
        if (flag == 0 && counter_user == bomb_user) {// win position
            change_color_rgb(38, 255, 5);
            cout << "YOU WON!,congrats ;)" << endl;
            delay(500);
            clear_screen();
            menu();
            }
        }
    }
}
void temp_easy(){
    /*int x_temp = get_cursor_x();
    int y_temp = get_cursor_y();*/
    save_cursor();
    cursor_to_pos(15,1);
    time_start_easy = clock();
    cout << "your time :";
    cout << time_start_easy / 1000 << endl;
    cout << "your point:" << point << endl;
    restore_cursor();
    //cursor_to_pos(y_temp,x_temp);
}
void temp_regular(){
    /*int x_temp = get_cursor_x();
    int y_temp = get_cursor_y();*/
    save_cursor();
    cursor_to_pos(28,1);
    time_start_regular = clock();
    cout << "your time :";
    cout << time_start_regular / 1000 << endl;
    cout << "your point:" << point << endl;
    restore_cursor();
    //cursor_to_pos(y_temp,x_temp);
}
void temp_hard(){
    /*int x_temp = get_cursor_x();
    int y_temp = get_cursor_y();*/
    save_cursor();
    cursor_to_pos(42,1);
    time_start_hard = clock();
    cout << "your time :";
    cout << time_start_hard / 1000 << endl;
    cout << "your point:" << point << endl;
    restore_cursor();
    //cursor_to_pos(y_temp,x_temp);
}
void temp_user(){
    /*int x_temp = get_cursor_x();
    int y_temp = get_cursor_y();*/
    save_cursor();
    cursor_to_pos(2 * (row_user) + 2,1);
    time_start_user = clock();
    cout << "your time :";
    cout << time_start_user / 1000 << endl;
    cout << "your point:" << point << endl;
    restore_cursor();
    //cursor_to_pos(y_temp,x_temp);
}
void entrance_easy() // get entrance_easy from user
{

    int count = 0;
    while(true) {
        key_easy1 = getch();
        if (key_easy1 == 'A' || key_easy1 == 'a') {
            cursor_backward(8);
            num_2--;
        }
        if (key_easy1 == 'W' || key_easy1 == 'w') {
            cursor_up(2);
            num_1--;
        }
        if (key_easy1 == 'D' || key_easy1 == 'd') {
            cursor_forward(8);
            num_2++;
        }
        if (key_easy1 == 'S' || key_easy1 == 's') {
            cursor_down(2);
            num_1++;
        }
        temp_easy();
        if (key_easy1 == 32) {
            change_color_rgb(255, 0, 0);
            change_easy(num_1, num_2);
            check_empty_number_easy(num_1 ,num_2);
            x_easy = get_cursor_x();
            y_easy = get_cursor_y();
            if (cnt_mine_easy[num_1][num_2] != '*') {
                if (cnt_mine_easy[num_1][num_2] == '0') {
                    for (int j = num_1 - 1; j <= num_1 + 1; ++j) {
                        for (int k = num_2 - 1; k <= num_2 + 1; ++k) {
                            if (k > 0 && j > 0 && k < 6 && j < 6) {
                                if (j != num_1 || k != num_2) {
                                    count++;
                                }
                            }
                        }
                    }
                    point += count + 1;
                }
                else if (cnt_mine_easy[num_1][num_2] != '0' && !(cnt_mine_easy[num_1][num_2] == 'F') && cnt_mine_easy[num_1][num_2] != '*')
                    point++;
            }
            save_point(player_count);
            clear_screen();
            board_easy(cnt_easy);
            cursor_to_pos(x_easy , y_easy);
            // break;
        }
        if (key_easy1 == 'F' || key_easy1 == 'f'){
            flag_easy(key_easy1);
            save_cursor();
            clear_screen();
            board_easy(cnt_easy);
            restore_cursor();
        }
        if (key_easy1 == 'Q' || key_easy1 == 'q') {
            delay(500);
            clear_screen();
            menu();
        }
        if (key_easy1 == 'O' || key_easy1 == 'o') {
            flag_save = 1;
            cursor_to_pos(35 , 0);
            int num;
            cout << "your slut: 1.save1 2.save2 3.save3 4.save4" << endl;
            cout << "Select :";
            cin >> num;
            clear_screen();
            if (num == 1) {
                for (int i = 0; i < 6; ++i) {
                    for (int j = 0; j < 6; ++j) {
                        s1.cnt[i][j] = cnt_easy[i][j];
                        s1.cnt_mine[i][j] = cnt_mine_easy[i][j];
                    }
                }
                s1.counter = counter_easy;
                s1.time_start = time_start_easy;
                s1.point = point;
                int size = size_of_cnt(cnt_name);
                for (int i = 0; i < size; ++i) {
                    s1.username[i] = cnt_name[i];
                }
                menu();
            }
            if (num == 2) {
                clear_screen();
                for (int i = 0; i < 6; ++i) {
                    for (int j = 0; j < 6; ++j) {
                        s2.cnt[i][j] = cnt_easy[i][j];
                        s2.cnt_mine[i][j] = cnt_mine_easy[i][j];
                    }
                }
                s2.counter = counter_easy;
                s2.time_start = time_start_easy;
                s2.point = point;
                int size = size_of_cnt(cnt_name);
                for (int i = 0; i < size; ++i) {
                    s2.username[i] = cnt_name[i];
                }
                menu();
            }
            if (num == 3) {
                clear_screen();
                for (int i = 0; i < 6; ++i) {
                    for (int j = 0; j < 6; ++j) {
                        s3.cnt[i][j] = cnt_easy[i][j];
                        s3.cnt_mine[i][j] = cnt_mine_easy[i][j];
                    }
                }
                s3.counter = counter_easy;
                s3.time_start = time_start_easy;
                s3.point = point;
                int size = size_of_cnt(cnt_name);
                for (int i = 0; i < size; ++i) {
                    s3.username[i] = cnt_name[i];
                }
                menu();
            }
            if (num == 4) {
                clear_screen();
                for (int i = 0; i < 6; ++i) {
                    for (int j = 0; j < 6; ++j) {
                        s4.cnt[i][j] = cnt_easy[i][j];
                        s4.cnt_mine[i][j] = cnt_mine_easy[i][j];
                    }
                }
                s4.counter = counter_easy;
                s4.time_start = time_start_easy;
                s4.point = point;
                int size = size_of_cnt(cnt_name);
                for (int i = 0; i < size; ++i) {
                    s4.username[i] = cnt_name[i];
                }
                menu();
            }
        }
    }
}
void entrance_regular()
{
    int count = 0;
    while(true) {
        key_regular1 = getch();
        if (key_regular1 == 'A' || key_regular1 == 'a') {
            cursor_backward(8);
            num_2--;
        }
        if (key_regular1 == 'W' || key_regular1 == 'w') {
            cursor_up(2);
            num_1--;
        }
        if (key_regular1 == 'D' || key_regular1 == 'd') {
            cursor_forward(8);
            num_2++;
        }
        if (key_regular1 == 'S' || key_regular1 == 's') {
            cursor_down(2);
            num_1++;
        }
        temp_regular();
        if (key_regular1 == 32) {
            change_regular(num_1, num_2);
            check_empty_number_regular(num_1, num_2);
            x_regular = get_cursor_x();
            y_regular = get_cursor_y();
            if (cnt_mine_regular[num_1][num_2] != '*') {
                if (cnt_mine_regular[num_1][num_2] == '0') {
                    for (int j = num_1 - 1; j <= num_1 + 1; ++j) {
                        for (int k = num_2 - 1; k <= num_2 + 1; ++k) {
                            if (k > 0 && j > 0 && k < 13 && j < 13) {
                                if (j != num_1 || k != num_2) {
                                    count++;
                                }
                            }
                        }
                    }
                    point += 2*(count + 1);
                } else if (cnt_mine_regular[num_1][num_2] != '0' && !(cnt_mine_regular[num_1][num_2] == 'F') && cnt_mine_regular[num_1][num_2] != '*')
                    point += 2;
            }
            save_point(player_count);
            clear_screen();
            board_regular(cnt_regular);
            cursor_to_pos(x_regular, y_regular);
            // break;
        }
        if (key_regular1 == 'F' || key_regular1 == 'f') {
            flag_regular(key_regular1);
            save_cursor();
            clear_screen();
            board_regular(cnt_regular);
            restore_cursor();
        }
        if (key_regular1 == 'Q' || key_regular1 == 'q') {
            delay(500);
            clear_screen();
            menu();
        }
        if (key_regular1 == 'O' || key_regular1 == 'o') {
            flag_save = 1;
            cursor_to_pos(37, 0);
            int num;
            cout << "your slut: 1.save1 2.save2 3.save3 4.save4" << endl;
            cout << "Select :";
            cin >> num;
            clear_screen();
            if (num == 1) {
                for (int i = 0; i < 13; ++i) {
                    for (int j = 0; j < 13; ++j) {
                        s1.cnt[i][j] = cnt_regular[i][j];
                        s1.cnt_mine[i][j] = cnt_mine_regular[i][j];
                    }
                }
                s1.counter = counter_regular;
                s1.time_start = time_start_regular;
                s1.point = point;
                int size = size_of_cnt(cnt_name);
                for (int i = 0; i < size; ++i) {
                    s1.username[i] = cnt_name[i];
                }
                menu();
            }
            if (num == 2) {
                clear_screen();
                for (int i = 0; i < 13; ++i) {
                    for (int j = 0; j < 13; ++j) {
                        s2.cnt[i][j] = cnt_regular[i][j];
                        s2.cnt_mine[i][j] = cnt_mine_regular[i][j];
                    }
                }
                s2.counter = counter_regular;
                s2.time_start = time_start_regular;
                s2.point = point;
                int size = size_of_cnt(cnt_name);
                for (int i = 0; i < size; ++i) {
                    s2.username[i] = cnt_name[i];
                }
                menu();
            }
            if (num == 3) {
                clear_screen();
                for (int i = 0; i < 13; ++i) {
                    for (int j = 0; j < 13; ++j) {
                        s3.cnt[i][j] = cnt_regular[i][j];
                        s3.cnt_mine[i][j] = cnt_mine_regular[i][j];
                    }
                }
                s3.counter = counter_regular;
                s3.time_start = time_start_regular;
                s3.point = point;
                int size = size_of_cnt(cnt_name);
                for (int i = 0; i < size; ++i) {
                    s3.username[i] = cnt_name[i];
                }
                menu();
            }
            if (num == 4) {
                clear_screen();
                for (int i = 0; i < 13; ++i) {
                    for (int j = 0; j < 13; ++j) {
                        s4.cnt[i][j] = cnt_regular[i][j];
                        s4.cnt_mine[i][j] = cnt_mine_regular[i][j];
                    }
                }
                s4.counter = counter_regular;
                s4.time_start = time_start_regular;
                s4.point = point;
                int size = size_of_cnt(cnt_name);
                for (int i = 0; i < size; ++i) {
                    s4.username[i] = cnt_name[i];
                }
                menu();
            }
        }

        change_color_rgb(4, 255, 0);
    }
}
void entrance_hard() // get entrance from and give point too user
{
    int count = 0;
    while(true) {
        key_hard1 = getch();
        if (key_hard1 == 'A' || key_hard1 == 'a') {
            cursor_backward(8);
            num_2--;
        }
        if (key_hard1 == 'W' || key_hard1 == 'w') {
            cursor_up(2);
            num_1--;
        }
        if (key_hard1 == 'D' || key_hard1 == 'd') {
            cursor_forward(8);
            num_2++;
        }
        if (key_hard1 == 'S' || key_hard1 == 's') {
            cursor_down(2);
            num_1++;
        }
        temp_hard();
        if (key_hard1 == 32) {
            change_hard(num_1, num_2);
            check_empty_number_hard(num_1, num_2);
            x_hard = get_cursor_x();
            y_hard = get_cursor_y();
            if (cnt_mine_hard[num_1][num_2] != '*') {
                if (cnt_mine_hard[num_1][num_2] == '0') {
                    for (int j = num_1 - 1; j <= num_1 + 1; ++j) {
                        for (int k = num_2 - 1; k <= num_2 + 1; ++k) {
                            if (k > 0 && j > 0 && k < 21 && j < 21) {
                                if (j != num_1 || k != num_2) {
                                    count++;
                                }
                            }
                        }
                    }
                    point += 3 * (count + 1);
                } else if (cnt_mine_hard[num_1][num_2] != '0' && !(cnt_mine_hard[num_1][num_2] == 'F') &&
                           cnt_mine_hard[num_1][num_2] != '*')
                    point += 3;
            }
            save_point(player_count);
            clear_screen();
            board_hard(cnt_hard);
            cursor_to_pos(x_hard, y_hard);
            // break;
        }
        if (key_hard1 == 'F' || key_hard1 == 'f') {
            flag_hard(key_hard1);
            save_cursor();
            clear_screen();
            board_hard(cnt_hard);
            restore_cursor();
        }
        if (key_hard1 == 'Q' || key_hard1 == 'q') {
            delay(500);
            clear_screen();
            menu();
        }
        if (key_hard1 == 'O' || key_hard1 == 'o') {
                flag_save = 1;
                cursor_to_pos(45, 0);
                int num;
                cout << "your slut: 1.save1 2.save2 3.save3 4.save4" << endl;
                cout << "Select :";
                cin >> num;
                clear_screen();
                if (num == 1) {
                    clear_screen();
                    for (int i = 0; i < 21; ++i) {
                        for (int j = 0; j < 21; ++j) {
                            s1.cnt[i][j] = cnt_hard[i][j];
                            s1.cnt_mine[i][j] = cnt_mine_hard[i][j];
                        }
                    }
                    s1.counter = counter_hard;
                    s1.time_start = time_start_hard;
                    s1.point = point;
                    int size = size_of_cnt(cnt_name);
                    for (int i = 0; i < size; ++i) {
                        s1.username[i] = cnt_name[i];
                    }
                    menu();
                }
                if (num == 2) {
                    clear_screen();
                    for (int i = 0; i < 21; ++i) {
                        for (int j = 0; j < 21; ++j) {
                            s2.cnt[i][j] = cnt_hard[i][j];
                            s2.cnt_mine[i][j] = cnt_mine_hard[i][j];
                        }
                    }
                    s2.counter = counter_hard;
                    s2.time_start = time_start_hard;
                    s2.point = point;
                    int size = size_of_cnt(cnt_name);
                    for (int i = 0; i < size; ++i) {
                        s2.username[i] = cnt_name[i];
                    }
                    menu();
                }
                if (num == 3) {
                    clear_screen();
                    for (int i = 0; i < 21; ++i) {
                        for (int j = 0; j < 21; ++j) {
                            s3.cnt[i][j] = cnt_hard[i][j];
                            s3.cnt_mine[i][j] = cnt_mine_hard[i][j];
                        }
                    }
                    s3.counter = counter_hard;
                    s3.time_start = time_start_hard;
                    s3.point = point;
                    int size = size_of_cnt(cnt_name);
                    for (int i = 0; i < size; ++i) {
                        s3.username[i] = cnt_name[i];
                    }
                    menu();
                }
                if (num == 4) {
                    clear_screen();
                    for (int i = 0; i < 21; ++i) {
                        for (int j = 0; j < 21; ++j) {
                            s4.cnt[i][j] = cnt_hard[i][j];
                            s4.cnt_mine[i][j] = cnt_mine_hard[i][j];
                        }
                    }
                    s4.counter = counter_hard;
                    s4.time_start = time_start_hard;
                    s4.point = point;
                    int size = size_of_cnt(cnt_name);
                    for (int i = 0; i < size; ++i) {
                        s4.username[i] = cnt_name[i];
                    }
                    menu();
                }

        }
    }
    change_color_rgb(4, 255, 0);
}
void entrance_user() // get entrance from and give point too user
{
    int count = 0;
    while(true) {
        key_user1 = getch();
        if (key_user1 == 'A' || key_user1 == 'a') {
            cursor_backward(8);
            num_2--;
        }
        if (key_user1 == 'W' || key_user1 == 'w') {
            cursor_up(2);
            num_1--;
        }
        if (key_user1 == 'D' || key_user1 == 'd') {
            cursor_forward(8);
            num_2++;
        }
        if (key_user1 == 'S' || key_user1 == 's') {
            cursor_down(2);
            num_1++;
        }
        temp_user();
        if (key_user1 == 32) {
            change_user(num_1, num_2);
            check_empty_number_user(num_1, num_2);
            x_user = get_cursor_x();
            y_user = get_cursor_y();
            if (cnt_mine_user[num_1][num_2] != '*') {
                if (cnt_mine_user[num_1][num_2] == '0') {
                    for (int j = num_1 - 1; j <= num_1 + 1; ++j) {
                        for (int k = num_2 - 1; k <= num_2 + 1; ++k) {
                            if (k > 0 && j > 0 && k < row_user && j < col_user) {
                                if (j != num_1 || k != num_2) {
                                    count++;
                                }
                            }
                        }
                    }
                    if ((int) col_user < 8 && (int) row_user < 8)
                        point += count + 1;
                    else if ((int) col_user > 8 && (int) row_user > 8 && (int) col_user < 14 && (int) row_user < 14)
                        point += 2 * (count + 1);
                    else if ((int) col_user > 14 && (int) row_user > 14 && (int) col_user < 21 && (int) row_user < 21)
                        point += 3 * (count + 1);
                } else if (cnt_mine_user[num_1][num_2] != '0' &&
                           !(cnt_mine_user[num_1][num_2] == 'F' || cnt_mine_user[num_1][num_2] == 'R') &&
                           cnt_mine_user[num_1][num_2] != '*') {//n
                    if ((int) col_user < 8 && (int) row_user < 8)
                        point++;
                    else if ((int) col_user > 8 && (int) row_user > 8 && (int) col_user < 14 && (int) row_user < 14)
                        point += 2;
                    else if ((int) col_user > 14 && (int) row_user > 14 && (int) col_user < 21 && (int) row_user < 21)
                        point += 3;
                }
                save_point(player_count);
                clear_screen();
                board_user();
                cursor_to_pos(x_user, y_user);
                // break;
            }
        }
            if (key_user1 == 'F' || key_user1 == 'f') {
                flag_user(key_user1);
                save_cursor();
                clear_screen();
                board_user();
                restore_cursor();
            }
            if (key_user1 == 'Q' || key_user1 == 'q') {
                delay(500);
                clear_screen();
                menu();
            }
            if (key_user1 == 'O' || key_user1 == 'o') {
                flag_save = 1;
                change_color_rgb(255, 0, 0);
                cursor_to_pos(2 * (row_user + 3), 0);
                int num;
                cout << "your slut: 1.save1   2.save2   3.save3   4.save4" << endl;
                cout << "Select :";
                cin >> num;
                if (num == 1) {
                    clear_screen();
                    for (int i = 0; i < col_user; ++i) {
                        for (int j = 0; j < row_user; ++j) {
                            s1.cnt[i][j] = cnt_user[i][j];
                            s1.cnt_mine[i][j] = cnt_mine_user[i][j];
                        }
                    }
                    s1.counter = counter_user;
                    s1.time_start = time_start_user;
                    s1.point = point;
                    int size = size_of_cnt(cnt_name);
                    for (int i = 0; i < size; ++i) {
                        s1.username[i] = cnt_name[i];
                    }
                    menu();
                }
                if (num == 2) {
                    clear_screen();
                    for (int i = 0; i < col_user; ++i) {
                        for (int j = 0; j < row_user; ++j) {
                            s2.cnt[i][j] = cnt_user[i][j];
                            s2.cnt_mine[i][j] = cnt_mine_user[i][j];
                        }
                    }
                    s2.counter = counter_user;
                    s2.time_start = time_start_user;
                    s2.point = point;
                    int size = size_of_cnt(cnt_name);
                    for (int i = 0; i < size; ++i) {
                        s2.username[i] = cnt_name[i];
                    }
                    menu();
                }
                if (num == 3) {
                    clear_screen();
                    for (int i = 0; i < col_user; ++i) {
                        for (int j = 0; j < row_user; ++j) {
                            s3.cnt[i][j] = cnt_user[i][j];
                            s3.cnt_mine[i][j] = cnt_mine_user[i][j];
                        }
                    }
                    s3.counter = counter_user;
                    s3.time_start = time_start_user;
                    s3.point = point;
                    int size = size_of_cnt(cnt_name);
                    for (int i = 0; i < size; ++i) {
                        s3.username[i] = cnt_name[i];
                    }
                    menu();
                }
                if (num == 4) {
                    clear_screen();
                    for (int i = 0; i < col_user; ++i) {
                        for (int j = 0; j < row_user; ++j) {
                            s4.cnt[i][j] = cnt_user[i][j];
                            s4.cnt_mine[i][j] = cnt_mine_user[i][j];
                        }
                    }
                    s4.counter = counter_user;
                    s4.time_start = time_start_user;
                    s4.point = point;
                    int size = size_of_cnt(cnt_name);
                    for (int i = 0; i < size; ++i) {
                        s4.username[i] = cnt_name[i];
                    }
                    menu();
                }

        }
    }
    change_color_rgb(4, 255, 0);
}
void board_easy(char cnt[][6])// board except of $
{
    change_color_rgb(255, 255, 255);
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; ++j)
        {
            cout << cnt[i][j] << '\t';
        }
        cout << endl;
        cout << endl;
    }
    failed_position_easy();
}
void board_regular(char cnt[][13])// board except of $
{
    change_color_rgb(255, 255, 255);
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13; ++j) {
            if (i == 0 || j == 0) {
                cout << (int) (cnt[i][j] - '0') << "\t";
                (char) (cnt[i][j]);
            } else {
                cout << cnt[i][j] << "\t";
                (char) (cnt[i][j]);
            }
        }
        cout << endl;
        cout << endl;
    }
    failed_position_regular();
}
void board_hard(char cnt[][21])// board except of $
{
    change_color_rgb(255, 255, 255);
    for (int i = 0; i < 21; i++) {
        for (int j = 0; j < 21; ++j) {
            if (i == 0 || j == 0) {
                cout << (int) (cnt[i][j] - '0') << "\t";
                (char) (cnt[i][j]);
            } else {
                cout << cnt[i][j] << "\t";
                (char) (cnt[i][j]);
            }
        }
        cout << endl;
        cout << endl;
    }
    failed_position_hard();
}
void board_user()// board except of $
{
    change_color_rgb(255, 255, 255);
    for (int i = 0; i < col_user; i++) {
        for (int j = 0; j < row_user; ++j) {
            if (i == 0 || j == 0) {
                cout << (int) (cnt_user[i][j] - '0') << "\t";
                (char) (cnt_user[i][j]);
            } else {
                cout << cnt_user[i][j] << "\t";
                (char) (cnt_user[i][j]);
            }
        }
        cout << endl;
        cout << endl;
    }
    failed_position_user();
}
void give_easy(char cnt[][6]) // building board with $ and number
{
    change_color_rgb(255, 13, 0);
    for (int k = 0; k < 6; ++k)
    {
        for (int l = 0; l < 6; ++l)
        {
            if (l == 0)
            {
                cnt[k][l] = '0' + k;//n
            }
            else if (k == 0)
            {
                cnt[k][l] = '0' + l;//n
            }
            else
            {
                cnt[k][l] = '$';//n
            }
        }
    }
    for (int k = 0; k < 6; ++k)
    {
        for (int l = 0; l < 6; ++l)
        {
            if (l == 0)
            {
                cnt_mine_easy[k][l] = '0' + k;//n
            }
            else if (k == 0)
            {
                cnt_mine_easy[k][l] = '0' + l;//n
            }
            else
            {
                cnt_mine_easy[k][l] = '0';//n
            }
        }
    }
}
void give_regular(char cnt[][13]) // building board with $ and number
{
    change_color_rgb(255, 13, 0);
    for (int k = 0; k < 13; ++k)
    {
        for (int l = 0; l < 13; ++l)
        {
            if (l == 0)
            {
                cnt[k][l] = '0' + k;//n
            }
            else if (k == 0)
            {
                cnt[k][l] = '0' + l;//n
            }
            else
            {
                cnt[k][l] = '$';//n
            }
        }
    }
    for (int k = 0; k < 13; ++k)
    {
        for (int l = 0; l < 13; ++l)
        {
            if (l == 0)
            {
                cnt_mine_regular[k][l] = '0' + k;//n
            }
            else if (k == 0)
            {
                cnt_mine_regular[k][l] = '0' + l;//n
            }
            else
            {
                cnt_mine_regular[k][l] = '0';//n
            }
        }
    }
}
void give_hard(char cnt[][21]) // building board with $ and number
{
    change_color_rgb(255, 13, 0);
    for (int k = 0; k < 21; ++k)
    {
        for (int l = 0; l < 21; ++l)
        {
            if (l == 0)
            {
                cnt[k][l] = '0' + k;//n
            }
            else if (k == 0)
            {
                cnt[k][l] = '0' + l;//n
            }
            else
            {
                cnt[k][l] = '$';//n
            }
        }
    }
    for (int k = 0; k < 21; ++k)
    {
        for (int l = 0; l < 21; ++l)
        {
            if (l == 0)
            {
                cnt_mine_hard[k][l] = '0' + k;//n
            }
            else if (k == 0)
            {
                cnt_mine_hard[k][l] = '0' + l;//n
            }
            else
            {
                cnt_mine_hard[k][l] = '0';//n
            }
        }
    }
}
void give_user() // building board with $ and number
{
    change_color_rgb(255, 13, 0);
    for (int k = 0; k < col_user; ++k)
    {
        for (int l = 0; l < row_user; ++l)
        {
            if (l == 0)
            {
                cnt_user[k][l] = '0' + k;//n
            }
            else if (k == 0)
            {
                cnt_user[k][l] = '0' + l;//n
            }
            else
            {
                cnt_user[k][l] = '$';//n
            }
        }
    }
    for (int k = 0; k < col_user; ++k)
    {
        for (int l = 0; l < row_user; ++l)
        {
            if (l == 0)
            {
                cnt_mine_user[k][l] = '0' + k;//n
            }
            else if (k == 0)
            {
                cnt_mine_user[k][l] = '0' + l;//n
            }
            else
            {
                cnt_mine_user[k][l] = '0';//n
            }
        }
    }
}
void change_easy(int index1, int index2)
{
    cnt_easy[index1][index2] = cnt_mine_easy[index1][index2];
}
void change_regular(int index1, int index2)
{
    cnt_regular[index1][index2] = cnt_mine_regular[index1][index2];
}
void change_hard(int index1, int index2)
{
    cnt_hard[index1][index2] = cnt_mine_hard[index1][index2];
}
void change_user(int index1, int index2)
{
    cnt_user[index1][index2] = cnt_mine_user[index1][index2];
}
void mine_easy()
{
    change_color_rgb(255, 0, 25);
    for (int i = 0; i < 4; ++i)
    {
        int random1 = (rand() % 5) + 1;
        int random2 = (rand() % 5) + 1;
        if(cnt_mine_easy[random1][random2] == '*'){
            i--;
            continue;
        }
        cnt_mine_easy[random1][random2] = '*';
        for (int j = random1 - 1; j <= random1 + 1; ++j)
        {
            for (int k = random2 - 1; k <= random2 + 1; ++k)
            {
                if (k > 0 && j > 0 && k < 6 && j < 6)
                {
                    if (j != random1 || k != random2)
                    {
                        if (cnt_mine_easy[j][k] != '*')//n
                            cnt_mine_easy[j][k]++;//n
                    }
                }
            }
        }
    }
}
void mine_regular()
{
    change_color_rgb(255, 0, 25);
    for (int i = 0; i < 28; ++i) // building mine
    {
        int random1 = (rand() % 12) + 1;
        int random2 = (rand() % 12) + 1;
        if(cnt_mine_regular[random1][random2] == '*'){
            i--;
            continue;
        }
        cnt_mine_regular[random1][random2] = '*';
        for (int j = random1 - 1; j <= random1 + 1; ++j)// building number
        {
            for (int k = random2 - 1; k <= random2 + 1; ++k)
            {
                if (k > 0 && j > 0 && k < 13 && j < 13)
                {
                    if (j != random1 || k != random2)
                    {
                        if (cnt_mine_regular[j][k] != '*')//n
                            cnt_mine_regular[j][k]++;//n
                    }
                }
            }
        }
    }
}
void mine_hard()
{
    change_color_rgb(255, 0, 25);
    for (int i = 0; i < 96; ++i) // building mine
    {
        int random1 = (rand() % 20) + 1;
        int random2 = (rand() % 20) + 1;
        if(cnt_mine_hard[random1][random2] == '*'){
            i--;
            continue;
        }
        cnt_mine_hard[random1][random2] = '*';
        for (int j = random1 - 1; j <= random1 + 1; ++j)// building number
        {
            for (int k = random2 - 1; k <= random2 + 1; ++k)
            {
                if (k > 0 && j > 0 && k < 21 && j < 21)
                {
                    if (j != random1 || k != random2)
                    {
                        if (cnt_mine_hard[j][k] != '*')//n
                            cnt_mine_hard[j][k]++;//n
                    }
                }
            }
        }
    }
}
void mine_user()
{
    change_color_rgb(255, 0, 25);
    for (int i = 0; i < bomb_user; ++i) // building mine
    {
        int random1 = (rand() % (col_user - 1)) + 1;
        int random2 = (rand() % (row_user - 1)) + 1;
        if(cnt_mine_user[random1][random2] == '*'){
            i--;
            continue;
        }
        cnt_mine_user[random1][random2] = '*';
        for (int j = random1 - 1; j <= random1 + 1; ++j)// building number
        {
            for (int k = random2 - 1; k <= random2 + 1; ++k)
            {
                if (k > 0 && j > 0 && k < col_user && j < row_user)
                {
                    if (j != random1 || k != random2)
                    {
                        if (cnt_mine_user[j][k] != '*')//n
                            cnt_mine_user[j][k]++;//n
                    }
                }
            }
        }
    }
}
void minesweeper_easy(bool is_loaded) {
    num_1 = 1;
    num_2 = 1;
    if(!is_loaded){
        give_easy(cnt_easy);
        mine_easy();
    }
    board_easy(cnt_easy);
    cursor_to_pos(3 , 9);
    while(true) {
        entrance_easy();
    }
}
void minesweeper_regular(bool is_loaded) {
    num_1 = 1;
    num_2 = 1;
    if(!is_loaded){
        give_regular(cnt_regular);
        mine_regular();
    }
    board_regular(cnt_regular);
    cursor_to_pos(3 , 9);
    while(true) {
        entrance_regular();
    }
}
void minesweeper_hard(bool is_loaded) {
    num_1 = 1;
    num_2 = 1;
    if(!is_loaded){
        give_hard(cnt_hard);
        mine_hard();
    }
    board_hard(cnt_hard);
    cursor_to_pos(3 , 9);
    while(true) {
        entrance_hard();
    }
}
void minesweeper_user(bool is_loaded) {
    cout << "Enter Your Suggestion For Column & Row:";
    cin >> col_user >> row_user;
    col_user += 1;
    row_user += 1;
    cout << endl;
    cout << endl;
    cout << "Enter Your Suggestion For Bomb:";
    cin >> bomb_user;
    cout << endl;
    delay(500);
    clear_screen();
    num_1 = 1;
    num_2 = 1;
    if (!is_loaded) {
        give_user();
        mine_user();
    }
    board_user();
    cursor_to_pos(3, 9);
    while (true) {
        entrance_user();
    }
}
void minesweeper_user_save(bool is_loaded) {
    cout << "Enter your suggested col & row :";
    cin >> col_user >> row_user;
    col_user += 1;
    row_user += 1;
    cout << endl;
    cout << endl;
    cout << "Enter Your Suggested Bomb:";
    cin >> bomb_user;
    cout << endl;
    delay(500);
    clear_screen();
    num_1 = 1;
    num_2 = 1;
    if(!is_loaded){
        give_user();
        mine_user();
    }
    board_user();
    cursor_to_pos(3 , 9);
    while(true) {
        entrance_user();
    }
}
int main() {
    srand(time(NULL));
    init_clui();
    menu();
    return 0;
}