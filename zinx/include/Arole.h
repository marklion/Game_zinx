#ifndef _AROLE_H_
#define _AROLE_H_
#include "include.h"
class Achannel;
class Amessage;

/*
这是一个抽象类，可以绑定最多一个Achannel对象作为输出通道。
该类负责处理收到的各类Amessage对象，或产生并发送Amessage对象。
开发者应该核心的业务处理在该类的派生类中实现
*/
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