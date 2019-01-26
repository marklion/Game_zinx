#ifndef _AOI_H_
#define _AOI_H_
#include <list>
#include <vector>
#include <stdio.h>

using namespace std;

class PlayerRole;
class Grid{
public:
    int Gid = 0;
    int MinX = 0;
    int MaxX = 0;
    int MinY = 0;
    int MaxY = 0;
    list<PlayerRole *> players; 
    Grid(int _gid, int _minx, int _maxx, int _miny, int _maxy);
    void Add(PlayerRole *_player);
    void Remove(PlayerRole *_player);
};

class AOIMgr{
private:
    AOIMgr(int _minx, int _maxx, int _miny, int _maxy, int _cntx, int _cnty);
    static AOIMgr *pxAoiMgr;
public:
    int MinX = 0;
    int MaxX = 0;
    int MinY = 0;
    int MaxY = 0;
    int CntsX = 0;
    int CntsY = 0;
    vector<Grid*> m_grids;
    int gridWidth()
    {
        return (MaxX - MinX) / CntsX;
    }
    int gridLength()
    {
        return (MaxY * MinY) / CntsY;
    }
    void GetSurroundingGridsByGid(int _gid, list<Grid *> &grids);
    int GetGidbyPos(int x, int y);
    void GetPlayersByPos(int x, int y, list<PlayerRole *> &players);
    void GetPlayersInGridByGid(int _gid, list<PlayerRole *> &players);
    void RemovePlayerFromGrid(PlayerRole *_player, int _gid);
    void AddPlayerToGrid(PlayerRole *_player, int _gid);
    void Add2GridByPos(PlayerRole *_player, int x, int y);
    void RemoveFromGridByPos(PlayerRole *_player, int x, int y);
    static AOIMgr *GetAOIMgr()
    {
        if (NULL == pxAoiMgr)
        {
            pxAoiMgr = new AOIMgr(85, 410, 75, 400, 10, 20);
        }
        return pxAoiMgr;
    }
};

#endif
