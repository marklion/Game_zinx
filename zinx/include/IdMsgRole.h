#ifndef _ID_MSG_ROLE_H_
#define _ID_MSG_ROLE_H_

#include "Arole.h"
#include "Amessage.h"
#include <map>

class IdMsgRole;

/*
该类是纯虚类，包含一个纯虚函数ProcMsg用于处理IdMessage。
若开发者需要使用IdMessage类或其派生类作为消息封装，则建议继承该类并在ProcMsg中实现具体功能
*/
class IIdMsgProc{
public:
    /*
    描述：该函数会在IdMsgRole对象处理消息的过程中被调用。
          开发者需要重写该函数，按照实际的业务需求处理各种消息。
    参数：
        _pxRole是当前正在处理消息的对象。
        _pxMsg是消息本身
    返回值：业务流程正常执行则返回true。
            遇到意料之外的系统错误则返回false。
    */
    virtual bool ProcMsg(IdMsgRole *_pxRole, IdMessage *_pxMsg) = 0;
    virtual ~IIdMsgProc()
    {
    }
};

/*
该类继承自Arole类，用于处理IdMessage类型的消息。
将其实例化后需调用register_id_func函数将消息类型和对应的消息处理对象（IIdMsgProc对象）进行注册。
该类构造的对象添加到server中后，若有IdMessage被指定由该对象处理，则之前注册的相应的IIdMsgProc会被调用处理该消息。
开发者的纯业务处理应该由该类的派生类实现，并合理地注册多个IIdMsgProc对业务请求分类处理
*/
class IdMsgRole:public Arole{
public:
    virtual bool init() override;
    virtual void fini() override;
    virtual ~IdMsgRole();
    virtual bool proc_msg(Amessage * pxMsg) override;

    /*
    描述：注册消息类型和其对应的处理接口。
          该函数应该在IdMsgRole对象创建后调用。
    参数:
        _id是消息的类型编号
        Iproc是处理消息的派生对象。
    返回值：true->成功，false->失败。
    */
    bool register_id_func(int _id, IIdMsgProc *_Iproc);
    void unregister_id_func(int _id);
protected:
    std::map<int, IIdMsgProc*> m_IdMsgMap;
};

#endif
