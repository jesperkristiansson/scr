#include "common/clientMessage.h"
#include "common/clientMessages.h"

MessageErrorStatus Message::peek(const std::byte *buf, std::size_t size, MessageType &type_ret){
        if(size < sizeof(MessageType)){
            return MessageErrorStatus::NotEnoughData;
        }

        const MessageType *_buf = reinterpret_cast<const MessageType*>(buf);
        type_ret = *_buf;

        //check validity of message
        if(type_ret >= MessageType::NUM_ELEMENTS){
            return MessageErrorStatus::InvalidData;
        }

        return MessageErrorStatus::Success;
}

typedef std::unique_ptr<Message>(*MessageCreater)();

MessageCreater messageCreaters[] = {
    JoinMessage::create,
    MessageMessage::create,
    QuitMessage::create,
};

Message::MessagePointer Message::CreateMessage(MessageType type){
    size_t index = static_cast<size_t>(type);
    size_t num_messages = sizeof(messageCreaters)/sizeof(*messageCreaters);
    if(index >= num_messages){
        return nullptr;
    }
    return messageCreaters[index]();
}

Message::MessagePointer Message::CreateMessage(const std::byte *buf, std::size_t size){
    MessageType type;
    MessageErrorStatus status = peek(buf, size, type);
    if(status != MessageErrorStatus::Success){
        return nullptr;
    }

    return CreateMessage(type);
}