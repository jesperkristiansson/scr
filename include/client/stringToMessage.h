#ifndef STRING_TO_MESSAGE_H
#define STRING_TO_MESSAGE_H

#include "common/clientMessage.h"

#include <string>

ClientMessage::MessagePointer string_to_message(const std::string &string);

#endif