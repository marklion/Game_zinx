#ifndef _PLAY_PROTOCOL_H_
#define _PLAY_PROTOCOL_H_

#include "zinx.h"
class PlayerMsg;
class PlayerRole;

class PlayerProtocol:public Aprotocol{
public:
    Arole *pxBindRole = NULL;
    PlayerProtocol(PlayerRole *_bindPlayer);
    virtual bool raw2request(const RawData *pstData, std::list<Request *> &_ReqList) override; 
    virtual bool response2raw(const Response * pstResp, RawData * pstData) override; 
private:
    RawData m_lasttime_data;
    int iCurPos = 0;
    PlayerMsg *GetMessagefromRaw();
};

#endif