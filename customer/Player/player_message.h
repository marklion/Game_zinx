#ifndef _PLAYER_MESSAGE_H_
#define _PLAYER_MESSAGE_H_

#include "zinx.h"
#include "msg.pb.h"

class PlayerMsg:public IdMessage{
public:
    ::google::protobuf::Message *pxProtobufMsg = NULL;
    PlayerMsg(int _id):IdMessage(_id)
    {
    }
    virtual ~PlayerMsg()
    {
        if (NULL != pxProtobufMsg)
            delete pxProtobufMsg;
    }
};

#endif 
