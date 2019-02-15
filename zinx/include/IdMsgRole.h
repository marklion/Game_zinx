#ifndef _ID_MSG_ROLE_H_
#define _ID_MSG_ROLE_H_

#include "Arole.h"
#include "Amessage.h"
#include <map>

class IdMsgRole;

class IIdMsgProc{
public:
    virtual bool ProcMsg(IdMsgRole *_pxRole, IdMessage *_pxMsg) = 0;
    virtual ~IIdMsgProc()
    {
    }
};

class IdMsgRole:public Arole{
public:
    virtual bool init() override;
    virtual void fini() override;
    virtual ~IdMsgRole();
    virtual bool proc_msg(Amessage * pxMsg) override;
    bool register_id_func(int _id, IIdMsgProc *_Iproc);
    void unregister_id_func(int _id);
protected:
    std::map<int, IIdMsgProc*> m_IdMsgMap;
};

#endif
