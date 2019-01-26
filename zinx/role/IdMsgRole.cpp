#include "IdMsgRole.h"

bool IdMsgRole::init()
{
    return true;
}

void IdMsgRole::fini()
{
}

IdMsgRole::~IdMsgRole()
{
    auto itr = m_IdMsgMap.begin();
    while (itr != m_IdMsgMap.end())
    {
        int Id = (*itr).first;
        auto pxIdproc = (*itr++).second;
        unregister_id_func(Id);
        delete pxIdproc;
    }
}

bool IdMsgRole::proc_msg(Amessage * pxMsg)
{
    bool bRet = false;
    IdMessage *pxIdMsg = dynamic_cast<IdMessage *>(pxMsg);

    if (NULL != pxIdMsg)
    {
        IIdMsgProc *pxIproc = m_IdMsgMap[pxIdMsg->Id];
        if (NULL != pxIproc)
        {
            bRet = pxIproc->ProcMsg(this, pxIdMsg);
        }
    }

    return bRet;
}

bool IdMsgRole::register_id_func(int _id, IIdMsgProc *Iproc)
{
    m_IdMsgMap[_id] = Iproc;

    return true;
}

void IdMsgRole::unregister_id_func(int _id)
{
    m_IdMsgMap.erase(_id);
}
