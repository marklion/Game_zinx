#ifndef _SERVER_H_
#define _SERVER_H_

#include "Achannel.h"
#include "Arole.h"
#include "Amessage.h"
#include <list>
#include <map>

/*
框架包含一个server类的单例对象，所有IO和数据处理的业务都应该依附于该对象。
*/
class Server{
public:
    virtual ~Server();
    /*
    描述：该函数用于初始化server实例，应该在第一次获取server实例后调用。
    返回值：true->成功，false->失败
    */
    bool init();
    void fini();
    
    /*
    描述:该函数用于将Achannel及其派生类对象安装到server实例中。
    参数：pxChannel是待添加的通道对象，必须是堆对象。
    返回值：true->成功，false->失败
    */
    bool install_channel(Achannel *pxChannel);

    /*
    描述：该函数用于摘除server中的Arole对象。
          摘除后pxChannel对象并没有被释放。
    参数：pxChannel是待摘除对象。
    */
    void uninstall_channel(Achannel *pxChannel);

    /*
    描述：该函数用于将pxRole对象添加到server实例中。
    参数：
        szCharacter用于指定pxRole对象的特征或分类。
            通常可以设置为类名，被指定的特征相同的pxRole会在server内被组成一张线性表。
        pxRole表示待添加的角色对象，必须是堆对象。
    返回值：true->成功，false->失败
    */
    bool add_role(std::string szCharacter, Arole *pxRole);

    /*
    描述：该函数用于摘除server对象中的Arole对象。
          摘除后pxRole对象并没有被释放。
    参数：
        szCharacter表示pxRole对象的特征或分类。
    pxRole表示待摘除对象。
    */
    void del_role(std::string szCharacter, Arole *pxRole);

    /*
    描述：当运行时需要向外发送请求时，应该调用该函数。
          不建议直接调用Achannel对象的发送方法。
    参数：pstResp中必须包含待发送的Amessage对象和发送该消息的Arole对象。
          若该对象Arole没有绑定出口Achannel，则该函数什么都不会做。
    返回值：true->成功，false->失败。
    */
    bool send_resp(Response *pstResp);
    bool handle(Request *pstReq);

    /*
    描述：该函数是框架的运行入口，
          调用该函数后程序会进入循环阻塞等待之前添加的Achannel对象产生相应的IO并作出处理。
          该函数不会返回除非server实例未初始化或在运行时成员函数ShutDownMainLoop被调用。
    返回值：true->调用ShutDownMainLoop退出，false->server实例未初始化成功。
    */
    bool run();

    /*
    描述：该函数用于停止框架主循环。
          可以在server实例run起来之后任意时间调用。
    */
    void ShutDownMainLoop();

    /*
    描述：该函数用于获取特征为szCharacter的所有Arole对象。
          Arole对象被组织到list中，这个list不应该被修改内容和或释放。
    参数：szCharacter表示pxRole对象的特征或分类。
    返回值：包含所有符合条件的Arole对象；没找到则为空。
    */
    std::list<Arole *> *GetRoleListByCharacter(std::string szCharacter);
    /*
    描述：该函数用于获取server实例。
          因为server类被设计为单例模式，所以能且只能通过该函数获取唯一server实例。
    返回值：server对象；若失败则返回空。
    */
    static Server *GetServer()
    {
        if (NULL == Server::pxServer)
        {
            Server::pxServer = new Server();
        }

        return Server::pxServer;
    }
private:
    std::map<std::string, std::list<Arole *>> m_role_list_map;
    std::map<int, Achannel*>m_channel_map;
    int m_epollfd = -1;
    bool m_need_exit = false;
    static Server *pxServer;
    Server();
};

#endif

