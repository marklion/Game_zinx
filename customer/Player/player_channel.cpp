#include "player_channel.h"
#include "player_protocol.h"
#include "player_role.h"
#include "player_message.h"
#include <iostream>

PlayerChannel::PlayerChannel(int _iDataFd, Aprotocol *_protocol):TcpDataChannel(_iDataFd, _protocol)
{
}
void PlayerChannel::TcpProcHup()
{
    Arole *pxRole = NULL;
    PlayerProtocol *pxProtocol = dynamic_cast<PlayerProtocol *>(m_pxProtocol);
    if (NULL != pxProtocol)
    {
        pxRole = pxProtocol->pxBindRole;
        if (NULL != pxRole)
        {
            Server::GetServer()->del_role("PlayerRole", pxRole);
            delete pxRole;
            pxProtocol->pxBindRole = NULL;
        }
    }
}

PlayerLstChannel::PlayerLstChannel():TcpListenChannel(8899)
{

}

bool PlayerLstChannel::TcpAfterConnection(int _iDataFd, struct sockaddr_in * pstClientAddr)
{
    PlayerRole *pxPlayer = new PlayerRole();
    TcpDataChannel *pxTcpData = new PlayerChannel(_iDataFd, new PlayerProtocol(pxPlayer));
    pxPlayer->SetChannel(pxTcpData);
    pxPlayer->iPid = m_NewPlayerId++;
    
    Server::GetServer()->install_channel(pxTcpData);
    Server::GetServer()->add_role("PlayerRole", pxPlayer);

    
    return true;
}


