#ifndef HANDLER_H
#define HANDLER_H

#include "common/clientMessage.h"

template <typename TArgType>
class Handler{
    public:
        virtual void handle(JoinMessage& msg, TArgType &from) = 0;
        virtual void handle(MessageMessage &msg, TArgType &from) = 0;
        virtual void handle(QuitMessage &msg, TArgType &from) = 0;
        virtual void handle(ClientMessage& msg, TArgType &from) = 0;
    protected:
        Handler() = default;
};

#endif