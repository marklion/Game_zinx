#include "player_channel.h"
#include "player_protocol.h"
#include "player_role.h"
#include "player_message.h"
#include "aoi.h"
#include <list>
#include <vector>
#include <random>
#include <time.h>
#include <algorithm>

using namespace std;

class Proc0Msg:public IIdMsgProc{
    virtual bool ProcMsg(IdMsgRole * _pxRole, IdMessage * _pxMsg)
    {
        std::cout<<"proc 0 Msg"<<std::endl;

        return true;
    }
};

PlayerRole::PlayerRole()
{
	default_random_engine e(time(NULL));
	x = e() % 10 + 160;
	z = e() % 17 + 134;
}

class Proc2Msg:public IIdMsgProc{
    virtual bool ProcMsg(IdMsgRole * _pxRole, IdMessage * _pxMsg)
    {
        PlayerMsg *pxMsg = dynamic_cast<PlayerMsg *>(_pxMsg);
        pb::Talk *pxTalkMsg = dynamic_cast<pb::Talk *>(pxMsg->pxProtobufMsg);
        PlayerRole *pxPlayer = dynamic_cast<PlayerRole *>(_pxRole);
        pxPlayer->Talk(pxTalkMsg->content());

        return true;
    }
};

class Proc3Msg:public IIdMsgProc{
    virtual bool ProcMsg(IdMsgRole * _pxRole, IdMessage * _pxMsg)
    {
        PlayerMsg *pxMsg = dynamic_cast<PlayerMsg *>(_pxMsg);
        pb::Position *pxPosPkg = dynamic_cast<pb::Position *>(pxMsg->pxProtobufMsg);

        PlayerRole *pxPlayer = dynamic_cast<PlayerRole *>(_pxRole);
        pxPlayer->UpdatePos((int)(pxPosPkg->x()), (int)(pxPosPkg->y()), (int)(pxPosPkg->z()), (int)(pxPosPkg->v()));

        return true;
    }
};

bool PlayerRole::init()
{
    bool bRet = true;
    bRet |= register_id_func(0, new Proc0Msg());
    bRet |= register_id_func(3, new Proc3Msg());
    bRet |= register_id_func(2, new Proc2Msg());
    if (true == bRet)
    {
        AOIMgr::GetAOIMgr()->Add2GridByPos(this, this->x, this->z);
        bRet = SyncId();
        bRet |= SyncSurrounding(); 
    }
    return bRet;
}

bool PlayerRole::SyncSurrounding()
{
    bool bRet = true;
    
    pb::BroadCast *pxSyncSelf = new pb::BroadCast();
    pxSyncSelf->set_pid(iPid);
    pxSyncSelf->set_tp(2);
    pb::Position *p = pxSyncSelf->mutable_p();
    p->set_x((float)(x));
    p->set_y((float)(y));
    p->set_z((float)(z));
    p->set_v((float)(v));

    PlayerMsg *pxSyncSelfMsg = new PlayerMsg(200);
    pxSyncSelfMsg->pxProtobufMsg = pxSyncSelf;
    
    list<PlayerRole *> players;
    GetSurroundingPlayers(players);

    Response stResp2SurPlayers;
    pb::SyncPlayers *pxSyncPlayers = new pb::SyncPlayers();
    for (auto itr = players.begin(); itr != players.end();itr++)
    {
        PlayerRole *pxPlayer = (*itr);
        stResp2SurPlayers.pxMsg = pxSyncSelfMsg;
        stResp2SurPlayers.pxSender = pxPlayer;
        bRet |= Server::GetServer()->send_resp(&stResp2SurPlayers);

        pb::Player *pxSurPlayer = pxSyncPlayers->add_ps();
        pxSurPlayer->set_pid(pxPlayer->iPid);
        pb::Position *p = pxSurPlayer->mutable_p();
        p->set_x((float)(pxPlayer->x));
        p->set_y((float)(pxPlayer->y));
        p->set_z((float)(pxPlayer->z));
        p->set_v((float)(pxPlayer->v));

    }
    PlayerMsg *pxSurPlayerMsg = new PlayerMsg(202);
    pxSurPlayerMsg->pxProtobufMsg = pxSyncPlayers;
    Response stResp2Self;
    stResp2Self.pxMsg = pxSurPlayerMsg;
    stResp2Self.pxSender = this;
    bRet |= Server::GetServer()->send_resp(&stResp2Self);
    
	return bRet;
}

void PlayerRole::GetSurroundingPlayers(std::list<PlayerRole *> &_players)
{
    AOIMgr::GetAOIMgr()->GetPlayersByPos(x, y, _players);
}

bool PlayerRole::SyncId()
{
    PlayerMsg *pxPlayMsg = new PlayerMsg(1);
    pb::SyncPid *pxSync = new pb::SyncPid();
    pxSync->set_pid(iPid);
    pxPlayMsg->pxProtobufMsg = pxSync;
    
    Response stResp;
    stResp.pxMsg = pxPlayMsg;
    stResp.pxSender = this;
    
    return Server::GetServer()->send_resp(&stResp);
}

void PlayerRole::fini()
{
    pb::SyncPid *pxSyncpid = new pb::SyncPid();
    pxSyncpid->set_pid(this->iPid);

    PlayerMsg *pxMsg = new PlayerMsg(201);
    pxMsg->pxProtobufMsg = pxSyncpid;

    Response stResp;
    stResp.pxMsg = pxMsg;
    
    list<PlayerRole *> players;
    GetSurroundingPlayers(players);
    auto itr = players.begin();
    for (; itr != players.end(); itr++)
    {
        stResp.pxSender = (*itr);
        Server::GetServer()->send_resp(&stResp);
    }
    
    AOIMgr::GetAOIMgr()->RemoveFromGridByPos(this, x, z);
}

void PlayerRole::OnExchangeAioGrid(int _oldGid, int _newGid)
{
    list<Grid *> OldGrids;
    list<Grid *> NewGrids;
    
    AOIMgr::GetAOIMgr()->GetSurroundingGridsByGid(_oldGid, OldGrids);    
    AOIMgr::GetAOIMgr()->GetSurroundingGridsByGid(_newGid, NewGrids);

    for (auto itr = OldGrids.begin(); itr != OldGrids.end(); itr++)
    {
        Grid *pxGrid = (*itr);
        auto find_itr = find(NewGrids.begin(), NewGrids.end(), pxGrid);

        //leave
        if (find_itr == NewGrids.end())
        {
            ViewsLost(pxGrid);
        }
    }
    for (auto itr = NewGrids.begin(); itr != NewGrids.end(); itr++)
    {
        Grid *pxGrid = (*itr);
        auto find_itr = find(OldGrids.begin(), OldGrids.end(), pxGrid);
        //enter
        if (find_itr == OldGrids.end())
        {
            ViewsAppear(pxGrid);
        }
    }
}

void PlayerRole::ViewsLost(Grid * _pxGrid)
{
    pb::SyncPid *pxSyncSelfId = new pb::SyncPid();
    pxSyncSelfId->set_pid(this->iPid);

    PlayerMsg *pxOthersMsg = new PlayerMsg(201);
    pxOthersMsg->pxProtobufMsg = pxSyncSelfId;

    Response stResp2Others;
    stResp2Others.pxMsg = pxOthersMsg;

    list<PlayerRole *> &surplayers = _pxGrid->players;
    for (auto itr = surplayers.begin(); itr != surplayers.end(); itr++)
    {
        PlayerRole *pxPlayer = (*itr);
        stResp2Others.pxSender = pxPlayer;
        Server::GetServer()->send_resp(&stResp2Others);

        pb::SyncPid *pxSyncOthersId = new pb::SyncPid();
        pxSyncOthersId->set_pid(pxPlayer->iPid);
        PlayerMsg *pxSelfMsg = new PlayerMsg(201);
        pxSelfMsg->pxProtobufMsg = pxSyncOthersId;
        Response stResp2Self;
        stResp2Self.pxMsg = pxSelfMsg;
        stResp2Self.pxSender = this;
        Server::GetServer()->send_resp(&stResp2Self);
    }
}

void PlayerRole::ViewsAppear(Grid * _pxGrid)
{
    pb::Position *pxSelfPos = new pb::Position();
    pxSelfPos->set_x((float)x);
    pxSelfPos->set_y((float)y);
    pxSelfPos->set_z((float)z);
    pxSelfPos->set_v((float)v);
    pb::BroadCast *pxSelfBroad = new pb::BroadCast();
    pxSelfBroad->set_allocated_p(pxSelfPos);
    pxSelfBroad->set_pid(iPid);
    pxSelfBroad->set_tp(2);

    PlayerMsg *pxSelfMsg = new PlayerMsg(200);
    pxSelfMsg->pxProtobufMsg = pxSelfBroad;

    Response stResp2Others;
    stResp2Others.pxMsg = pxSelfMsg;
    list<PlayerRole *> &surplayers = _pxGrid->players;
    for (auto itr = surplayers.begin(); itr != surplayers.end(); itr++)
    {
        PlayerRole *pxOtherPlayer = (*itr);
        stResp2Others.pxSender = pxOtherPlayer;
        Server::GetServer()->send_resp(&stResp2Others);

        pb::Position *pxOtherPos = new pb::Position();
        pxOtherPos->set_x((float)(pxOtherPlayer->x));
        pxOtherPos->set_y((float)(pxOtherPlayer->y));
        pxOtherPos->set_z((float)(pxOtherPlayer->z));
        pxOtherPos->set_v((float)(pxOtherPlayer->v));
        pb::BroadCast *pxOtherBroad = new pb::BroadCast();
        pxOtherBroad->set_allocated_p(pxOtherPos);
        pxOtherBroad->set_pid(pxOtherPlayer->iPid);
        pxOtherBroad->set_tp(2);

        PlayerMsg *pxOtherMsg = new PlayerMsg(200);
        pxOtherMsg->pxProtobufMsg = pxOtherBroad;

        Response stResp2Self;
        stResp2Self.pxMsg = pxOtherMsg;
        stResp2Self.pxSender = pxOtherPlayer;

        Server::GetServer()->send_resp(&stResp2Self);
    }
}

void PlayerRole::UpdatePos(int _x, int _y, int _z, int _v)
{
    int oldGid = AOIMgr::GetAOIMgr()->GetGidbyPos(x, y);
    int newGid = AOIMgr::GetAOIMgr()->GetGidbyPos(x, y);

    x = _x;
    y = _y;
    z = _z;
    v = _v;

    cout<<"Update Pos"<<" x="<<x<<" y="<<y<<" z="<<z<<" v="<<v<<endl;

    if (oldGid != newGid)
    {
        AOIMgr::GetAOIMgr()->RemovePlayerFromGrid(this, oldGid);
        AOIMgr::GetAOIMgr()->AddPlayerToGrid(this, newGid);
       OnExchangeAioGrid(oldGid, newGid); 
    }

    pb::Position *pxSelfPos = new pb::Position();
    pxSelfPos->set_x((float)x);
    pxSelfPos->set_y((float)y);
    pxSelfPos->set_z((float)z);
    pxSelfPos->set_v((float)v);
    pb::BroadCast *pxSelfBroad = new pb::BroadCast;
    pxSelfBroad->set_pid(iPid);
    pxSelfBroad->set_tp(4);
    pxSelfBroad->set_allocated_p(pxSelfPos);

    PlayerMsg *pxSelfMsg = new PlayerMsg(200);
    pxSelfMsg->pxProtobufMsg = pxSelfBroad;
    
    Response stResp2Players;
    stResp2Players.pxMsg = pxSelfMsg;

    list<PlayerRole *> players;
    GetSurroundingPlayers(players);
    for (auto itr = players.begin(); itr != players.end(); itr++)
    {
        stResp2Players.pxSender = (*itr);
        Server::GetServer()->send_resp(&stResp2Players);
    }
}

void PlayerRole::Talk(const string &szContent)
{
    pb::BroadCast *pxBroadTalk = new pb::BroadCast();
    pxBroadTalk->set_pid(iPid);
    pxBroadTalk->set_tp(1);
    pxBroadTalk->set_content(szContent);

    PlayerMsg *pxMsg = new PlayerMsg(200);
    pxMsg->pxProtobufMsg = pxBroadTalk;

    Response stResp;
    stResp.pxMsg = pxMsg;
    
    auto pRoleList = Server::GetServer()->GetRoleListByCharacter("PlayerRole");
    if (NULL != pRoleList)
    {
        for (auto itr = pRoleList->begin();itr != pRoleList->end(); itr++)
        {
            PlayerRole *pxPlayer = dynamic_cast<PlayerRole *>(*itr);
            if (NULL != pxPlayer)
            {
                stResp.pxSender = pxPlayer;
                Server::GetServer()->send_resp(&stResp);
            }
        }
    }
}

