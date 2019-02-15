#ifndef _PLAYER_ROLE_H_
#define _PLAYER_ROLE_H_

#include "zinx.h"
#include <vector>
#include <random>
#include <string>

#define RANDOM_FIRST_NAME "random_first.txt"
#define RANDOM_SECOND_NAME "random_last.txt"

class Grid;
class PlayerRole:public IdMsgRole{
private:
    void ViewsLost(Grid *_pxGrid);
    void ViewsAppear(Grid *_pxGrid);
    bool need_update(float _x, float _y, float _z, float _v);
public:
    PlayerRole();
    virtual bool init();
    virtual void fini();
    int iPid = 0;
    float x = 0;
    float y = 0;
    float z = 0;
    float v = 0;
    std::string szName;
    int iBlood = 100;

    bool SyncSurrounding();
    void GetSurroundingPlayers(std::list<PlayerRole *> &_players);
    bool SyncId();
    void OnExchangeAioGrid(int _oldGid, int _newGid);
    void UpdatePos(float _x, float _y, float _z, float _v);
    void Talk(const std::string &szContent);
    static std::default_random_engine e;
};

struct FirstName {
    std::string szFirstName;
    std::vector<std::string> vecLastName;
};

class RandomName{
public:
    RandomName();
    ~RandomName();
    void LoadFile();
    std::string GetName();
    void ReleaseName(std::string szName);
private:
    std::vector<FirstName *> m_names;
};

#endif
