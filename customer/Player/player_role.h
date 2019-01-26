#ifndef _PLAYER_ROLE_H_
#define _PLAYER_ROLE_H_

#include "zinx.h"

#include <random>
class Grid;
class PlayerRole:public IdMsgRole{
private:
    void ViewsLost(Grid *_pxGrid);
    void ViewsAppear(Grid *_pxGrid);
    static std::default_random_engine e;
public:
    PlayerRole();
    virtual bool init();
    virtual void fini();
    int iPid = 0;
    float x = 0;
    float y = 0;
    float z = 0;
    float v = 0;

    bool SyncSurrounding();
    void GetSurroundingPlayers(std::list<PlayerRole *> &_players);
    bool SyncId();
    void OnExchangeAioGrid(int _oldGid, int _newGid);
    void UpdatePos(float _x, float _y, float _z, float _v);
    void Talk(const std::string &szContent);
};

#endif
