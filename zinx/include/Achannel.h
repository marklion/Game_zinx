#ifndef _ACHANNEL_H_
#define _ACHANNEL_H_

#include "Aprotocol.h"
#include <sys/epoll.h>
#include "include.h"

/*
这是一个抽象类，负责处理文件层面的数据IO。
其中大部分方法都是基于linux的系统调用实现。
类实例应该按需安装(install)或卸载(uninstall)。
开发者应该继承该类并重写部分方法，实现原始数据收发功能
*/
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