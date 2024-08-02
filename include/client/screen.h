#ifndef SCREEN_H
#define SCREEN_H

#include <ncurses.h>

#include <string>

struct _Window{
    int height;
    int width;
    int starty;
    int startx;
    WINDOW *win;
};

class Screen{
    public:
        Screen();
        ~Screen();

        void get_login(std::string &username, std::string &password);

        bool start_app(int delay = 0);
        void set_info(const std::string &str);
        void put_message(const std::string &str);
        bool get_input(std::string &str);
        void update_screen();
        void resize_window();
    private:
        std::string buffer = std::string();
        unsigned int index = 0;
        struct _Window info;
        struct _Window top;
        struct _Window bottom;
        std::string title;
        int delay;

        static bool already_exists;
};

#endif