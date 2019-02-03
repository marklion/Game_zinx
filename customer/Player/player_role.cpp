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
#include <string> 
#include <fstream> 
#include <iostream> 

using namespace std;

default_random_engine PlayerRole::e(time(NULL));
RandomName g_xRandModule;


class Proc0Msg:public IIdMsgProc{
    virtual bool ProcMsg(IdMsgRole * _pxRole, IdMessage * _pxMsg)
    {

        return true;
    }
};

PlayerRole::PlayerRole()
{
	x = e() % 10 + 160;
	z = e() % 17 + 134;
	szName = g_xRandModule.GetName();
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
        pxPlayer->UpdatePos(pxPosPkg->x(), pxPosPkg->y(), pxPosPkg->z(), pxPosPkg->v());

        return true;
    }
};

bool PlayerRole::init()
{
    cout << szName << " is comming"<<endl;
    bool bRet = true;
    bRet |= register_id_func(0, new Proc0Msg());
    bRet |= register_id_func(3, new Proc3Msg());
    bRet |= register_id_func(2, new Proc2Msg());
    if (true == bRet)
    {
        AOIMgr::GetAOIMgr()->Add2GridByPos(this, (int)x, (int)z);
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
    p->set_x(x);
    p->set_y(y);
    p->set_z(z);
    p->set_v(v);
    p->set_bloodvalue(iBlood);

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
        p->set_x(pxPlayer->x);
        p->set_y(pxPlayer->y);
        p->set_z(pxPlayer->z);
        p->set_v(pxPlayer->v);
        p->set_bloodvalue(pxPlayer->iBlood);
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
    AOIMgr::GetAOIMgr()->GetPlayersByPos((int)x, (int)z, _players);
}

bool PlayerRole::SyncId()
{
    PlayerMsg *pxPlayMsg = new PlayerMsg(1);
    pb::SyncPid *pxSync = new pb::SyncPid();
    pxSync->set_pid(iPid);
    pxSync->set_username(szName);
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
    
    AOIMgr::GetAOIMgr()->RemoveFromGridByPos(this, (int)x, (int)z);
    g_xRandModule.ReleaseName(szName);
    cout<<szName<<" exit"<<endl;
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
    pxSelfPos->set_x(x);
    pxSelfPos->set_y(y);
    pxSelfPos->set_z(z);
    pxSelfPos->set_v(v);
    pxSelfPos->set_bloodvalue(iBlood);
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
        pxOtherPos->set_x(pxOtherPlayer->x);
        pxOtherPos->set_y(pxOtherPlayer->y);
        pxOtherPos->set_z(pxOtherPlayer->z);
        pxOtherPos->set_v(pxOtherPlayer->v);
        pxOtherPos->set_bloodvalue(pxOtherPlayer->iBlood);
        pb::BroadCast *pxOtherBroad = new pb::BroadCast();
        pxOtherBroad->set_allocated_p(pxOtherPos);
        pxOtherBroad->set_pid(pxOtherPlayer->iPid);
        pxOtherBroad->set_tp(2);

        PlayerMsg *pxOtherMsg = new PlayerMsg(200);
        pxOtherMsg->pxProtobufMsg = pxOtherBroad;

        Response stResp2Self;
        stResp2Self.pxMsg = pxOtherMsg;
        stResp2Self.pxSender = this;

        Server::GetServer()->send_resp(&stResp2Self);
    }
}

void PlayerRole::UpdatePos(float _x, float _y, float _z, float _v)
{
    int oldGid = AOIMgr::GetAOIMgr()->GetGidbyPos((int)x, (int)z);
    int newGid = AOIMgr::GetAOIMgr()->GetGidbyPos((int)_x, (int)_z);

    x = _x;
    y = _y;
    z = _z;
    v = _v;

    if (oldGid != newGid)
    {
        AOIMgr::GetAOIMgr()->RemovePlayerFromGrid(this, oldGid);
        AOIMgr::GetAOIMgr()->AddPlayerToGrid(this, newGid);
       OnExchangeAioGrid(oldGid, newGid); 
    }

    pb::Position *pxSelfPos = new pb::Position();
    pxSelfPos->set_x(x);
    pxSelfPos->set_y(y);
    pxSelfPos->set_z(z);
    pxSelfPos->set_v(v);
    pxSelfPos->set_bloodvalue(iBlood);
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

RandomName::RandomName()
{
}

RandomName::~RandomName()
{
    auto itr = m_names.begin();
    while (itr != m_names.end())
    {
        auto pData = (*itr);
        delete pData;
        itr = m_names.erase(itr);
    }
}

void RandomName::LoadFile()
{
    ifstream fFirstName;
    ifstream fSecondName;
    string tmpFirst;
    string tmpSecond;

    fFirstName.open(RANDOM_FIRST_NAME);
    fSecondName.open(RANDOM_SECOND_NAME);

    if (fFirstName.is_open() && fSecondName.is_open())
    {
        while (getline(fFirstName, tmpFirst))
        {
            FirstName *pstFirst = new FirstName();
            pstFirst->szFirstName = tmpFirst;
            m_names.push_back(pstFirst);
            while (getline(fSecondName, tmpSecond))
            {
                pstFirst->vecLastName.push_back(tmpSecond);
            }
            fSecondName.clear(ios::goodbit);
            fSecondName.seekg(ios::beg);
        }

        fFirstName.close();
        fSecondName.close();
    }
}

string RandomName::GetName()
{
    string szRet;

    if (0 < m_names.size())
    {
        int iRandFirst = PlayerRole::e() % m_names.size();
        FirstName *pstFirst = m_names[iRandFirst];
        int iRandSecond = PlayerRole::e() % pstFirst->vecLastName.size();
        
        szRet = pstFirst->szFirstName + " " + pstFirst->vecLastName[iRandSecond];

        pstFirst->vecLastName.erase(pstFirst->vecLastName.begin() + iRandSecond);
        if (0 >= pstFirst->vecLastName.size())
        {
            m_names.erase(m_names.begin() + iRandFirst);
            delete pstFirst;
        }
    }
    else
    {
        szRet = "Not Support";
    }

    return szRet;
}

void RandomName::ReleaseName(std :: string szName)
{
    int iSpace = szName.find(" ");
    string szFirstName = szName.substr(0, iSpace);
    string szSecondName = szName.substr(iSpace + 1, szName.size());

    auto itr = m_names.begin();
    for (; itr != m_names.end(); itr++)
    {
        if ((*itr)->szFirstName == szFirstName)
        {
            break;
        }
    }

    FirstName *pstFirst = NULL;
    if (m_names.end() != itr)
    {
        pstFirst = (*itr);
    }
    else
    {
        pstFirst = new FirstName();
        pstFirst->szFirstName = szFirstName;
        m_names.push_back(pstFirst);
    }
    pstFirst->vecLastName.push_back(szSecondName);
}

