#include "Aprotocol.h"
#include <stdlib.h>

bool RawData::AppendData(const RawData *pstData)
{
    bool bRet = false;
    unsigned char *pucTmpData = (unsigned char *)calloc(1UL, pstData->iLength + this->iLength);

    if (NULL != pucTmpData)
    {
        memcpy(pucTmpData, this->pucData, this->iLength);
        memcpy(pucTmpData + this->iLength, pstData->pucData, pstData->iLength);
        this->iLength += pstData->iLength;
        if (NULL != this->pucData)
        {
            free(this->pucData);
        }
        this->pucData = pucTmpData;
        bRet = true;
    }
    

    return bRet;
}

bool RawData::SetData(unsigned char * _pucData, int _iLength)
{
    bool bRet = false;
    unsigned char *pucTemp = (unsigned char *)calloc(1UL, _iLength);
    if (NULL != pucTemp)
    {
        memcpy(pucTemp, _pucData, _iLength);
        if (NULL != pucData)
        {
            free(pucData);
        }
        pucData = pucTemp;
        iLength = _iLength;
        bRet = true;
    }

    return bRet;
}

RawData::~RawData()
{
    if (NULL != this->pucData)
    {
        free(this->pucData);
        this->pucData = NULL;
    }

    this->iLength = 0;
}

Aprotocol::Aprotocol()
{

}

Aprotocol::~Aprotocol()
{
}
