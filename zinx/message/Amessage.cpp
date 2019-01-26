#include "Amessage.h"
#include "role/Arole.h"
Amessage::Amessage()
{
}

Amessage::~Amessage()
{
}

Request::~Request()
{
    if (NULL != pxMsg)
    {
        delete pxMsg;
    }
}


Response::~Response()
{
    if (NULL != pxMsg)
    {
        delete pxMsg;
    }
}

IdMessage::IdMessage(int _id):Id(_id)
{
}




