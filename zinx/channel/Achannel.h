#ifndef _ACHANNEL_H_
#define _ACHANNEL_H_

#include "protocol/Aprotocol.h"
#include <sys/epoll.h>
#include "include.h"

class Achannel{
public:
    explicit Achannel(Aprotocol *_pxProtocol);
    Achannel();
    virtual ~Achannel();
    virtual bool init() = 0;
    virtual bool readFd(uint32_t _event , RawData *pstData) = 0;
    virtual bool writeFd(const RawData *pstData) = 0;
    virtual void fini() = 0;
    uint32_t GetEvent();
    int GetFd();
    Aprotocol *GetProtocol();
protected:
    Aprotocol *m_pxProtocol = NULL;
    static std::string Convert2Printable(const RawData *pstData);
    int m_fd = -1;
    uint32_t m_event = EPOLLIN;
};

#endif