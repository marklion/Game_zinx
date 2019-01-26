#ifndef _PLAY_CHANNEL_H_
#define _PLAY_CHANNEL_H_

#include "zinx.h"

class PlayerChannel:public TcpDataChannel{
public:
    PlayerChannel(int _iDataFd, Aprotocol *_protocol);
    virtual void TcpProcHup() override;
};

class PlayerLstChannel:public TcpListenChannel{
private:
    int m_NewPlayerId = 0;
public:
    PlayerLstChannel();
    virtual bool TcpAfterConnection(int _iDataFd, struct sockaddr_in * pstClientAddr) override;
};

#endif
