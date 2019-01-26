#include "server.h"
#include <list>
using namespace std;

Server *Server::pxServer = NULL;

Server::Server()
{   
}

bool Server::init()
{
    bool bRet = false;

    int iFd = epoll_create(1);
    if (0 <= iFd)
    {
        m_epollfd = iFd;
        bRet = true;
    }

    return bRet;
}

void Server::fini()
{
    auto list_itr = m_role_list_map.begin();

    while (list_itr != m_role_list_map.end())
    {
		auto my_pair = *list_itr++;
        auto RoleList = my_pair.second;
		auto key = my_pair.first;
		auto itr = RoleList.begin();

		while (itr != RoleList.end())
		{
			auto pxRole = (*itr++);
			del_role(key, pxRole);
			delete pxRole;
		}
    }

    auto channel_map_itr = m_channel_map.begin();

    while (channel_map_itr != m_channel_map.end())
    {
        auto pxChannel = (*channel_map_itr++).second;
        uninstall_channel(pxChannel);
        delete pxChannel;
    }

    close(m_epollfd);
}

Server::~Server()
{
    fini();
}

bool Server::add_role(string szCharacter, Arole * pxRole)
{
    bool bRet = false;

    if (true == pxRole->init())
    {
		auto itr = m_role_list_map.find(szCharacter);
		if (itr == m_role_list_map.end())
		{
			list<Arole *> RoleList;
            RoleList.push_back(pxRole);
			m_role_list_map[szCharacter] = RoleList;
		}
		else
		{
			(*itr).second.push_back(pxRole);
		}
        bRet = true;
    }

    return bRet;
}

void Server::del_role(string szCharacter, Arole * pxRole)
{
    auto itr = m_role_list_map.find(szCharacter);

    if (itr != m_role_list_map.end())
    {
        (*itr).second.remove(pxRole);
		if ((*itr).second.empty())
		{
			m_role_list_map.erase(itr);
		}
        pxRole->fini();
    }
}

bool Server::install_channel(Achannel * pxChannel)
{
    struct epoll_event stEvent;
    bool bRet = false;

    if (true == pxChannel->init())
    {
        stEvent.events = pxChannel->GetEvent();
        stEvent.data.ptr = (void *)pxChannel;
        
        if (0 == epoll_ctl(m_epollfd, EPOLL_CTL_ADD, pxChannel->GetFd(), &stEvent))
        {
            m_channel_map[pxChannel->GetFd()] = pxChannel;
            bRet = true;
        }
    }

    return bRet;
    
}

void Server::uninstall_channel(Achannel * pxChannel)
{
    epoll_ctl(m_epollfd, EPOLL_CTL_DEL, pxChannel->GetFd(), NULL);
    m_channel_map.erase(pxChannel->GetFd());
    pxChannel->fini();
}

bool Server::handle(Request * pstReq)
{
    return pstReq->pxProcessor->proc_msg(pstReq->pxMsg);
}

bool Server::send_resp(Response * pstResp)
{
    bool bRet = false;

    if ((NULL != pstResp) && (NULL != pstResp->pxMsg) && (NULL != pstResp->pxSender))
    {
        Achannel *pxDestChannel = pstResp->pxSender->GetChannel();
        if (NULL != pxDestChannel)
        {
            RawData stRD;
            if (true == pxDestChannel->GetProtocol()->response2raw(pstResp, &stRD))
            {
                bRet = pxDestChannel->writeFd(&stRD);
            }
        }
    }

    return bRet;
}

bool Server::run()
{
    int iEpollRet = -1;

    if (0 > m_epollfd)
    {
        return false;
    }

	while (true != m_need_exit)
	{
        struct epoll_event atmpEvent[100];
        iEpollRet = epoll_wait(m_epollfd, atmpEvent, 100, -1);
        if (-1 == iEpollRet)
        {
            if (EINTR == errno)
            {
                continue;
            }
            else
            {
                break;
            }
        }
        for (int i = 0; i < iEpollRet; i++)
        {
            Request *pstReq;
            RawData stRD;
            Achannel *pxchannel = static_cast<Achannel *>(atmpEvent[i].data.ptr);
            
            if (true == pxchannel->readFd(atmpEvent[i].events, &stRD))
            {
                Aprotocol *pxProtocol = pxchannel->GetProtocol();
                list<Request *> ReqList;
                if ((NULL != pxProtocol) && (true == pxProtocol->raw2request(&stRD, ReqList)))
                {
                    auto itr = ReqList.begin();
                    while (itr != ReqList.end())
                    {
                        pstReq = (*itr);
                        (void)handle(pstReq);
                        ReqList.erase(itr++);
                        delete pstReq;
                    }
                }
            }
            else
            {
                uninstall_channel(pxchannel);
                delete pxchannel;
                break;
            }
        }
	}

	return true;
}

void Server::ShutDownMainLoop()
{
    m_need_exit = true;
}

list<Arole*>* Server::GetRoleListByCharacter(std::string szCharacter)
{
	list<Arole*> *pxRoleList = NULL;
	auto itr = m_role_list_map.find(szCharacter);

	if (itr != m_role_list_map.end())
	{
		pxRoleList = &((*itr).second);
	}

	return pxRoleList;
}

