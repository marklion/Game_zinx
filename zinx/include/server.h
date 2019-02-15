#ifndef _SERVER_H_
#define _SERVER_H_

#include "Achannel.h"
#include "Arole.h"
#include "Amessage.h"
#include <list>
#include <map>

class Server{
public:
    virtual ~Server();
    bool init();
    void fini();
    bool install_channel(Achannel *pxChannel);
    void uninstall_channel(Achannel *pxChannel);
    bool add_role(std::string szCharacter, Arole *pxRole);
    void del_role(std::string szCharacter, Arole *pxRole);
    bool send_resp(Response *pstResp);
    bool handle(Request *pstReq);
    bool run();
    void ShutDownMainLoop();
    std::list<Arole *> *GetRoleListByCharacter(std::string szCharacter);
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

