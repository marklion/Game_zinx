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

class Aprotocol{
public:
    Aprotocol();
    virtual ~Aprotocol();
    virtual bool raw2request(const RawData *pstData, std::list<Request *> &_ReqList) = 0;
    virtual bool response2raw(const Response *pstResp, RawData *pstData) = 0;
};

#endif