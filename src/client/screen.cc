#include "client/screen.h"

#include <stdexcept>

#include <string.h>
#include <ctype.h>
#include <signal.h>

bool Screen::already_exists = false;

static bool resize_flag = false;

/* Signal handler for SIGWINCH (terminal resize) */
static void resize_handler(int){
    resize_flag = true;
}

Screen::Screen() {
    if(already_exists){
        throw std::runtime_error("an instance of Screen already exists");
    }
    already_exists = true;

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = resize_handler;
    int res = sigaction(SIGWINCH, &sa, NULL);   //possibly save and restore old handler?
    if(res != 0){
        throw std::runtime_error("sigaction error");
    }

    initscr(); /* start the curses mode */
}

Screen::~Screen() {
    endwin();
    already_exists = false;
}

void Screen::get_login(std::string &username, std::string &password){
    nocbreak();     //buffered input
    echo();

    const int buf_size = 64;
    char buf[buf_size];

    int res;

    printw("Username: ");
    refresh();
    int x,y;
    getyx(stdscr, y, x);
    // if a signal happens, getnstr returns ERR, if it's due to a terminal resize, we retry
    do{
        res = mvgetnstr(y, x, buf, buf_size);
    } while(res == ERR && resize_flag);
    username = std::string(buf);

    noecho();
    printw("Password: ");
    refresh();
    getyx(stdscr, y, x);
    do{
        res = mvgetnstr(y, x, buf, buf_size);
    } while(res == ERR && resize_flag);
    password = std::string(buf);
}

static unsigned int change_cursor_index(unsigned int index, int delta, unsigned int bound){
    if(delta < 0 && index < (unsigned int) -delta){
        return 0;
    } else if(delta >= 0 && index + delta > bound){
        return bound;
    }
    return index + delta;
}

static void index_to_cursor(unsigned int index, unsigned int &x, unsigned int &y, unsigned int max_x){
    x = index % max_x;
    y = index / max_x;
}

bool Screen::start_app(int delay){
    this->delay = delay;
    //bound delay 0-255
    if(delay < 0){
        delay = 0;
    } else if(delay > 255){
        delay = 255;
    }

    //set how long to block while asking for input: (delay/10) seconds
    //delay == 0 means nonblocking
    if(delay == 0){
        cbreak();
        nodelay(stdscr, true);
    } else{
        halfdelay(delay);
    }

    noecho();
    keypad(stdscr, TRUE);

    info.height = 1;
    info.width = COLS;
    info.starty = 0;
    info.startx = 0;

    int bottom_height = 5;

    bottom.height = bottom_height;
    bottom.width = COLS;
    bottom.starty = LINES - bottom_height;
    bottom.startx = 0;

    top.height = LINES - bottom_height - info.height - 1;
    top.width = COLS;
    top.starty = info.height;
    top.startx = 0;

    info.win = newwin(info.height, info.width, info.starty, info.startx);
    bottom.win = newwin(bottom.height, bottom.width, bottom.starty, bottom.startx);
    top.win = newwin(top.height, top.width, top.starty, top.startx);
    mvhline(LINES - bottom_height - 1, 0, '-', COLS);

    scrollok(top.win, true);
    scrollok(bottom.win, true);

    leaveok(top.win, true);
    leaveok(bottom.win, true);

    werase(top.win);
    refresh();
    wrefresh(top.win);

    move(bottom.starty, bottom.startx);
    return true;
}

void Screen::set_info(const std::string &str){
    title = str;
    size_t available_space = info.width;
    size_t str_len = str.size();
    if(str_len > available_space){
        str_len = available_space;
        //print warning?
    }

    //clear old
    werase(info.win);
    //center
    int start_x = (info.width - str_len)/2;
    mvwaddstr(info.win, 0, start_x, str.substr(0, str_len).c_str());
    wrefresh(info.win);
}

void Screen::put_message_nosave(const std::string &str){
    wprintw(top.win, "%s\n", str.c_str());
    wnoutrefresh(top.win);
}

void Screen::put_message(const std::string &str){
    put_message_nosave(str);
    history.push_back(str);     //todo: have an upper limit on number of messages
}

bool Screen::get_input(std::string &str){
    bool enter_pressed = false;

    int c;
    while(!enter_pressed && (c = getch()) != ERR){
        bool modified = false;
        switch(c){
            case KEY_DOWN:
                index = change_cursor_index(index, COLS, buffer.size());
                break;
            case KEY_UP:
                index = change_cursor_index(index, -COLS, buffer.size());
                break;
            case KEY_LEFT:
                index = change_cursor_index(index, -1, buffer.size());
                break;
            case KEY_RIGHT:
                index = change_cursor_index(index, 1, buffer.size());
                break;
            case '\n':
                str = buffer;
                enter_pressed = true;

                buffer = "";
                index = 0;
                modified = true;
                break;
            case KEY_BACKSPACE:
                if(index == 0){
                    break;
                }
                buffer.erase(index - 1, 1);
                index = change_cursor_index(index, -1, buffer.size());
                modified = true;
                break;
            default:
                //ignore characters that aren't printable ascii
                if(!isprint(c)){
                    break;
                }
                buffer.insert(buffer.begin() + index, (char)c);
                index = change_cursor_index(index, 1, buffer.size());
                modified = true;
        }
        if(modified){
            //todo: only print from cursor and onwards
            werase(bottom.win);
            mvwaddstr(bottom.win, 0, 0, buffer.c_str());
            wrefresh(bottom.win);
        }
        unsigned int x, y;
        index_to_cursor(index, x, y, COLS);
        move(bottom.starty + y, bottom.startx + x);
    }

    return enter_pressed;
}

void Screen::update_screen(){
    unsigned int x, y;
    index_to_cursor(index, x, y, COLS);
    move(bottom.starty + y, bottom.startx + x);
    doupdate();
}

void Screen::resize_window(){
    if(!resize_flag){
        return;
    }

    resize_flag = false;
    clear();
    endwin();
    initscr();
    start_app(delay);
    set_info(title);

    //reprint lower window
    mvwaddstr(bottom.win, 0, 0, buffer.c_str());
    wrefresh(bottom.win);

    //reprint upper window
    for(const std::string &message : history){
        put_message_nosave(message);
    }
    update_screen();
}