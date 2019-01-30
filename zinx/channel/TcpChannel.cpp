#include "TcpChannel.h"
#include "server.h"
#include <string>

TcpListenChannel::TcpListenChannel(unsigned short usPort):m_usPort(usPort)
{
}

TcpListenChannel::~TcpListenChannel()
{
}

bool TcpListenChannel::TcpAfterConnection(int _iDataFd, struct sockaddr_in *pstClientAddr)
{
    std::cout<<"client IP is "<<std::string(inet_ntoa(pstClientAddr->sin_addr))<<" Port is "<<ntohs(pstClientAddr->sin_port)<<std::endl;
    close(_iDataFd);

    return true;
}

bool TcpListenChannel::readFd(uint32_t _event, RawData * pstData)
{
	bool bRet = false;
	
	if (0 != (_event & EPOLLIN))
	{
        int iDataFd = -1;
        struct sockaddr_in stClientAddr;
        socklen_t lAddrLen = sizeof(stClientAddr);
        
        iDataFd = accept(m_fd, (struct sockaddr *)&stClientAddr, &lAddrLen);
        if (0 <= iDataFd)
        {
            bRet = TcpAfterConnection(iDataFd, &stClientAddr);
        }
        else
        {
            perror("__FUNC__:accept:");
        }
	}

	return bRet;
}

bool TcpListenChannel::writeFd(const RawData * pstData)
{
	return false;
}

bool TcpListenChannel::init()
{
    bool bRet = false;
    int iListenFd = -1;
    struct sockaddr_in stServaddr;

    iListenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 <= iListenFd)
    {
        memset(&stServaddr,0,sizeof(stServaddr));
        stServaddr.sin_family = AF_INET;
        stServaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        stServaddr.sin_port = htons(m_usPort);
        if (0 == bind(iListenFd, (struct sockaddr *)&stServaddr, sizeof(stServaddr)))
        {
            if (0 == listen(iListenFd, 10))
            {
                bRet = true;
                m_fd = iListenFd;
            }
            else
            {
                perror("__FUNC__:listen:");
            }
        }
        else
        {
            perror("__FUNC__:bind:");
        }
    }
    else
    {
        perror("__FUNC__:socket:");
    }
        

	return bRet;
}

void TcpListenChannel::fini()
{
    if (0 <= m_fd)
    {
        close(m_fd);
    }
}

TcpDataChannel::TcpDataChannel(int _iDataFd, Aprotocol * _protocol):Achannel(_protocol)
{
    m_fd = _iDataFd;
}
TcpDataChannel::~TcpDataChannel()
{
}

bool TcpDataChannel::init()
{
    m_event = EPOLLIN|EPOLLHUP|EPOLLERR;
    return true;
}
void TcpDataChannel::fini()
{
}

bool TcpDataChannel::readFd(uint32_t _event, RawData * pstData)
{
    bool bRet = false;

    if (0 != (_event & EPOLLIN))
    {
        bRet = TcpProcDataIn(pstData);
        if (false == bRet)
        {
            TcpProcHup();
        }
    }

    if (0 != (_event & (EPOLLHUP|EPOLLERR)))
    {
        bRet = false;
        TcpProcHup();
    }

    return bRet;
}

bool TcpDataChannel::writeFd(const RawData * pstData)
{
    bool bRet = false;
    if (pstData->iLength == send(m_fd, pstData->pucData, pstData->iLength, 0))
    {
        bRet = true;
    }
    return bRet;
}

bool TcpDataChannel::TcpProcDataIn(RawData * pstData)
{
    bool bRet = false;
    ssize_t iReadLen = -1;
    unsigned char aucBuff[1024] = {0};

    while (0 < (iReadLen = recv(m_fd, aucBuff, sizeof(aucBuff), MSG_DONTWAIT)))
    {
        unsigned char *pucTempData = (unsigned char *)calloc(1UL, iReadLen + pstData->iLength);
        if (NULL != pucTempData)
        {
            bRet = true;
            memcpy(pucTempData, pstData->pucData, pstData->iLength);
            memcpy(pucTempData + pstData->iLength, aucBuff, iReadLen);
            pstData->iLength += iReadLen;
            if (NULL != pstData->pucData)
            {
                free(pstData->pucData);
            }
            pstData->pucData = pucTempData;
        }
    }

    return bRet;
}

void TcpDataChannel::TcpProcHup()
{
}
