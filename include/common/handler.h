#ifndef HANDLER_H
#define HANDLER_H

#include "common/message.h"

class Handler{
    public:
        virtual void handle(JoinMessage& msg, User &from) = 0;
        virtual void handle(MessageMessage &msg, User &from) = 0;
        virtual void handle(QuitMessage &msg, User &from) = 0;
        virtual void handle(Message& msg, User &from) = 0;
    protected:
        Handler() = default;
};

#endif