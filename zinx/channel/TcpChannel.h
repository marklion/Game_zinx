#ifndef _TCP_CHANNLE_H_
#define _TCP_CHANNLE_H_

#include "Achannel.h"
#include "message/Amessage.h"

class TcpListenChannel:public Achannel{
public:
    explicit TcpListenChannel(unsigned short usPort);
    virtual ~TcpListenChannel();

	// 通过 Achannel 继承
	virtual bool readFd(uint32_t _event, RawData * pstData) override;
	virtual bool writeFd(const RawData * pstData) override;

	virtual bool init() override;
	virtual void fini() override;
	virtual bool TcpAfterConnection(int _iDataFd, struct sockaddr_in *pstClientAddr);
private:
    unsigned short m_usPort = 0;
};

class TcpDataChannel:public Achannel{
public:
    TcpDataChannel(int _iDataFd, Aprotocol *_protocol);
    virtual ~TcpDataChannel();
    virtual bool readFd(uint32_t _event, RawData * pstData) override;
	virtual bool writeFd(const RawData * pstData) override;

	virtual bool init() override;
	virtual void fini() override;
	virtual bool TcpProcDataIn(RawData * pstData);
	virtual void TcpProcHup();
};

#endif