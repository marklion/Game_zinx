#ifndef _TCP_CHANNLE_H_
#define _TCP_CHANNLE_H_

#include "Achannel.h"
#include "Amessage.h"

/*
该类继承自Achannel类，用于处理TCP监听。
构造对象时需要携带参数usPort作为监听端口号，
将该类的实例添加到server中后，若有tcp连接到来则成员函数TcpAfterConnection会被调用。
开发者应该重写TcpAfterConnection方法以实现所需功能
*/
class TcpListenChannel:public Achannel{
public:
    explicit TcpListenChannel(unsigned short usPort);
    virtual ~TcpListenChannel();

	virtual bool readFd(uint32_t _event, RawData * pstData) override;
	virtual bool writeFd(const RawData * pstData) override;

	virtual bool init() override;
	virtual void fini() override;

	/*
    描述：该函数会在新的TCP连接建立后被调用。
          一般情况下，需要开发者重写该函数，在函数内创建TcpDataChannel对象并添加到server实例中。
    参数：
        _iDataFd是新建立的数据socket。
        pstClientAddr是客户端的地址结构封装。
    返回值：一般返回true。若返回false，则该TCP监听端口会被从server实例中摘除并关闭。
	*/
	virtual bool TcpAfterConnection(int _iDataFd, struct sockaddr_in *pstClientAddr);
private:
    unsigned short m_usPort = 0;
};

/*
该类继承自Achannel类，用于处理TCP数据报文。
构造对象时必须携带参数_iDataFd作为TCP数据socket。
该类实例应该在客户端连接到来后构造，然后添加到server中，
若socket收到数据，则成员函数TcpProcDataIn会被调用；
若socket发生异常（如对端关闭）则成员函数TcpProcHup会被调用，然后该实例会自动从server中摘除并析构掉。
不建议开发人员重写该类的虚函数，一般建议将数据层面的处理逻辑放到该类绑定的Aprotocol对象中处理。
*/
class TcpDataChannel:public Achannel{
public:
    TcpDataChannel(int _iDataFd, Aprotocol *_protocol);
    virtual ~TcpDataChannel();
    virtual bool readFd(uint32_t _event, RawData * pstData) override;
	virtual bool writeFd(const RawData * pstData) override;

	virtual bool init() override;
	virtual void fini() override;

	/*
	描述：该函数用于读取tcp客户端发来的数据。
    	未重写之前的行为是将原始数据存到pstData中
    参数：pstData用于存储读取到的数据
    返回值：若正常读取到数据则返回true，否则返回false。
            返回false后TcpDataChannel对象将会被摘除并析构。
	*/
	virtual bool TcpProcDataIn(RawData * pstData);
	virtual void TcpProcHup();
};

#endif