#include "aoi.h"
AOIMgr *AOIMgr::pxAoiMgr = NULL;
Grid::Grid(int _gid, int _minx, int _maxx, int _miny, int _maxy):Gid(_gid), MinX(_minx), MaxX(_maxx), MinY(_miny), MaxY(_maxy)
{
}

void Grid::Add(PlayerRole * _player)
{
    players.push_back(_player);
}

void Grid::Remove(PlayerRole * _player)
{
    players.remove(_player);
}

AOIMgr::AOIMgr(int _minx, int _maxx, int _miny, int _maxy, int _cntx, int _cnty):MinX(_minx),MaxX(_maxx), MinY(_miny), MaxY(_maxy), CntsX(_cntx), CntsY(_cnty)
{
    for (int y = 0; y < CntsY; ++y)
    {
        for (int x = 0; x < CntsX; x++)
        {
            int gid = y * CntsX + x;
            Grid *pxGrid = new Grid(gid, 
                MinX + x * gridWidth(),
                MinX + (x + 1) * gridWidth(),
                MinY + y * gridLength(),
                MinY + (y + 1) * gridLength());
            m_grids.push_back(pxGrid);
        }
    }
}

void AOIMgr::GetSurroundingGridsByGid(int _gid, list<Grid *> &outgrids)
{
    if (_gid < 0 || (unsigned int)_gid >= m_grids.size())
    {
        return;
    }

    outgrids.push_back(m_grids[_gid]);

    int x = _gid % CntsX;
    if (x > 0)
    {
        int gid = (_gid - 1);
        outgrids.push_back(m_grids[gid]);
        int y = gid / CntsX;
        if (y > 0)
        {
            outgrids.push_back(m_grids[gid - CntsX]);
        }
        if (y < (CntsY - 1))
        {
            outgrids.push_back(m_grids[gid + CntsX]);
        }
    }
    if (x < (CntsX - 1))
    {
        int gid = _gid + 1;
        outgrids.push_back(m_grids[gid]);
        int y = gid / CntsX;
        if (y > 0)
        {
            outgrids.push_back(m_grids[gid - CntsX]);
        }
        if (y < (CntsY - 1))
        {
            outgrids.push_back(m_grids[gid + CntsX]);
        }
    }    
}

int AOIMgr::GetGidbyPos(int x, int y)
{
    int gx = (x - MinX) / gridWidth();
    int gy = (y - MinY) / gridLength();

    return gy * CntsX + gx;
}

void AOIMgr::GetPlayersByPos(int x, int y, list < PlayerRole * > &players)
{
    int gid = GetGidbyPos(x, y);
	list<Grid *> Grids;

	GetSurroundingGridsByGid(gid, Grids);
	auto itr = Grids.begin();
	for (; itr != Grids.end(); itr++)
	{
		list<PlayerRole *> tmpList = list<PlayerRole *>((*itr)->players);
		players.splice(players.begin(), tmpList);
	}

	return;
}

void AOIMgr::GetPlayersInGridByGid(int _gid, list < PlayerRole * > & players)
{
	players = m_grids[_gid]->players;
}

void AOIMgr::RemovePlayerFromGrid(PlayerRole * _player, int _gid)
{
	m_grids[_gid]->Remove(_player);
}

void AOIMgr::AddPlayerToGrid(PlayerRole * _player, int _gid)
{
	m_grids[_gid]->Add(_player);
}

void AOIMgr::Add2GridByPos(PlayerRole * _player, int x, int y)
{
	int gid = GetGidbyPos(x, y);
	AddPlayerToGrid(_player, gid);
}

void AOIMgr::RemoveFromGridByPos(PlayerRole * _player, int x, int y)
{
	int gid = GetGidbyPos(x, y);
	RemovePlayerFromGrid(_player, gid);
}

