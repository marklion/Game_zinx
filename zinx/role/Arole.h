#ifndef _AROLE_H_
#define _AROLE_H_
#include "include.h"
class Achannel;
class Amessage;

class Arole{
public:
    Arole();
    virtual ~Arole();
    virtual bool init() = 0;
    virtual bool proc_msg(Amessage *pxMsg) = 0;
    virtual void fini() = 0;
    Achannel *GetChannel();
    void SetChannel(Achannel *pxChannel);
protected:
    Achannel *m_pxOutChannel = NULL;
};

#endif