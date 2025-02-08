#include "Command_Object_Type.h"
#include "Attack_Object_Type.h"
#include <iostream>

using namespace std;
Command_Object_Type::Command_Object_Type(string owner, Position pos, int dmg, int def, float hp, int move_dist)
	:Object(owner, pos, dmg, def, hp, move_dist, "Command")
{
}

bool Command_Object_Type::canSpawn(const Position& _spawnPos, const vector<vector<Object*>>& _board) const
{
	return(_spawnPos.y == 0 || _spawnPos.y == _board.size() - 1 && _board[_spawnPos.y][_spawnPos.x] == nullptr);
}

void Command_Object_Type::requestSpawn(const string& _unitType, const Position& spawnPos, const vector<vector<Object*>>& _board, const string& _newOwner)
{
	if (canSpawn(spawnPos, _board))
	{
		int spawn_time = unitSpawnTime[_unitType];
		if (spawnRequests.empty())
		{
			spawnRequests.push_back({ _unitType, spawnPos, _newOwner, spawn_time });
			cout << "Spawn requested add : " << _unitType << " " << spawnPos.x << " " << spawnPos.y << " " << _newOwner << " " << spawn_time << endl;
		}
		else
		{
			cout << " Spawn requested already exist" << "\n";
		}
	}
	else
	{
		cout << "Cannot spawn position" << endl;
	}
}

void Command_Object_Type::updateSpawnRequests(vector<vector<Object*>>& _board, vector<Object*>& _player)
{
	for (auto it = spawnRequests.begin(); it != spawnRequests.end();)
	{
		it->remainingTurns--;
		if (it->remainingTurns == 0)
		{
			Object* new_unit = nullptr;
			if (it->unitType == "Soldier")
			{
				new_unit = new Attack_Object_Type(it->newOwner, it->spawnPos, 10, 5, 100.0, 1, 1, "Soldier");
			}
			else if (it->unitType == "RedBack")
			{
				new_unit = new Attack_Object_Type(it->newOwner, it->spawnPos, 8, 4, 80.0, 2, 1, "RedBack");
			}
			else if (it->unitType == "Tank")
			{
				new_unit = new Attack_Object_Type(it->newOwner, it->spawnPos, 15, 10, 150.0, 2, 3, "Tank");
			}
			else if (it->unitType == "K9")
			{
				new_unit = new Attack_Object_Type(it->newOwner, it->spawnPos, 12, 6, 90.0, 4, 1, "K9");
			}

			for (auto& obj : _player)
			{
				if (obj == nullptr)
				{
					obj = new_unit;
					break;
				}
				else
				{
					continue;
				}
			}
			_board[it->spawnPos.y][it->spawnPos.x] = new_unit;
			cout << "Spawned : " << it->unitType << " " << it->spawnPos.x << " " << it->spawnPos.y << " " << it->newOwner << endl;
			it = spawnRequests.erase(it);
		}
		else
		{
			++it;
		}
	}
}
