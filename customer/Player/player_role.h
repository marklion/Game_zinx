#ifndef _PLAYER_ROLE_H_
#define _PLAYER_ROLE_H_

#include "zinx.h"

class Grid;
class PlayerRole:public IdMsgRole{
private:
    void ViewsLost(Grid *_pxGrid);
    void ViewsAppear(Grid *_pxGrid);
public:
    PlayerRole();
    virtual bool init();
    virtual void fini();
    int iPid = 0;
    int x = 0;
    int y = 0;
    int z = 0;
    int v = 0;

    bool SyncSurrounding();
    void GetSurroundingPlayers(std::list<PlayerRole *> &_players);
    bool SyncId();
    void OnExchangeAioGrid(int _oldGid, int _newGid);
    void UpdatePos(int _x, int _y, int _z, int _v);
    void Talk(const std::string &szContent);
};

#endif