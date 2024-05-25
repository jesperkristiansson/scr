#ifndef HANDLER_H
#define HANDLER_H

#include "message.h"

class Handler{
    public:
        virtual void handle(JoinMessage& msg, int from) = 0;
        virtual void handle(MessageMessage &msg, int from) = 0;
        virtual void handle(QuitMessage &msg, int from) = 0;
        virtual void handle(Message& msg, int from) = 0;
    protected:
        Handler() = default;
};

#endif