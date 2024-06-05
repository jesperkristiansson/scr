#ifndef STRING_TO_MESSAGE_H
#define STRING_TO_MESSAGE_H

#include "common/message.h"

#include <string>

MessagePointer string_to_message(const std::string &string);

#endif