#include "Achannel.h"
#include <string>

Achannel::Achannel(Aprotocol *_pxProtocol):m_pxProtocol(_pxProtocol)
{
}

Achannel::Achannel()
{
}


Achannel::~Achannel()
{
    if (-1 != m_fd)
    {
        close(m_fd);
    }

    if (NULL != m_pxProtocol)
    {
        delete m_pxProtocol;
    }
}

uint32_t Achannel::GetEvent()
{
    return m_event;
}

int Achannel::GetFd()
{
    return m_fd;
}

Aprotocol *Achannel::GetProtocol()
{
    return m_pxProtocol;
}

std::string Achannel::Convert2Printable(const RawData * pstData)
{
    char *pcTemp = (char *)calloc(1UL, pstData->iLength*3 + 1);
    int iCurPos = 0;

    if (NULL != pcTemp)
    {
        for (int i = 0; i < pstData->iLength; ++i)
        {
            iCurPos += sprintf(pcTemp+iCurPos, "%02X ", pstData->pucData[i]);
        }
        pcTemp[iCurPos] = 0;
    }
    
    std::string szRet = std::string(pcTemp);
    free(pcTemp);
    
    return szRet;
}

