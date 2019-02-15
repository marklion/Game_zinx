#ifndef _AMESSAGE_H_
#define _AMESSAGE_H_
#include "include.h"

class Arole;


class Amessage{
public:
    Amessage();
    virtual ~Amessage();
};

struct Request{
    Amessage *pxMsg = NULL;
    Arole *pxProcessor = NULL;
    ~Request();
};

struct Response{
    Amessage *pxMsg = NULL;
    Arole *pxSender = NULL;
    ~Response();
};

class IdMessage:public Amessage{
public:
    explicit IdMessage(int _id);  
    int Id;
    virtual ~IdMessage()
    {
    }
};


#endif
