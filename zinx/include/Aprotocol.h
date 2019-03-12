#ifndef _APROTOCOL_H_
#define _APROTOCOL_H_
#include "include.h"
#include "Amessage.h"

struct RawData{
    unsigned char *pucData = NULL;
    int iLength = 0;
    bool SetData(unsigned char *_pucData, int _iLength);
    bool AppendData(const RawData *_pxData);
    ~RawData();
};

/*
这是一个抽象类，负责将Achannel对象收发的原始数据和Amessage对象之间进行转换。
每个Achannel对象可以绑定最多一(0~1)个Aprotocol对象。
开发者应该继承该类并重写部分方法，实现通信协议功能
*/
class Aprotocol{
public:
    Aprotocol();
    virtual ~Aprotocol();

    /*
    描述：该函数会在绑定的Achannel对象收到数据后调用，
          用来把原始数据pstData转换成若干Request对象组成的list。
          开发者需要在派生类重写该函数将报文解析，封包，校验等功能实现。
    参数：
        pstData是待处理的原始数据。
              其结构包含unsigned char *pucData和int iLength两个核心变量分别存储数据缓冲区指针和数据长度。
        _ReqList是传出参数，用来存放转换后的若干Request对象。
              开发者需要将堆对象添加到表中，当数据处理完成后框架会自动释放堆内存。
    返回值：若正常产生了Request对象则返回true；若由于数据不全等因素导致转换失败则返回false。
    */
    virtual bool raw2request(const RawData *pstData, std::list<Request *> &_ReqList) = 0;

    /*
    描述：该函数会在Achannel对象的发送函数被调用之前调用。
          在该函数内需要开发者实现请求消息的序列化，从而方便后续数据发送。
    参数：
        pstResp包含待发送消息Amessage和发送者Arole。
        pstData是传出参数，用来存放序列化完成的数据。
    返回值：若正常序列化则返回true，否则返回false。
    */
    virtual bool response2raw(const Response *pstResp, RawData *pstData) = 0;
};

#endif