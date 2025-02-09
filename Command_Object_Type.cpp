#include "Command_Object_Type.h"
#include "Attack_Object_Type.h"
#include <iostream>

using namespace std;
Command_Object_Type::Command_Object_Type(string owner, Position pos, int dmg, int def, float hp, int move_dist)
	:Object(owner, pos, dmg, def, hp, move_dist, "Command")
{}



void Command_Object_Type::requestSpawn(const string& _unitType, const Position& spawnPos, const vector<vector<Object*>>& _board, const string& _newOwner)
{

	int spawn_time = unitSpawnTime[_unitType];
	spawnRequests.push_back({ _unitType, spawnPos, _newOwner, spawn_time });
	cout << "생성 대기줄에 유닛을 추가합니다 : " << _unitType << " " << spawnPos.x << " " << spawnPos.y << " " << _newOwner << " " << spawn_time << endl;
	state = afterSpawn;
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
				new_unit = new Attack_Object_Type(it->newOwner, it->spawnPos, 10, 5, 60.0, 3, 1, "Soldier");  // owner, pos, damage, defense, hp, move_dist, attack_range
			}
			else if (it->unitType == "RedBack")
			{
				new_unit = new Attack_Object_Type(it->newOwner, it->spawnPos, 8, 4, 80.0, 2, 2, "RedBack");
			}
			else if (it->unitType == "Tank")
			{
				new_unit = new Attack_Object_Type(it->newOwner, it->spawnPos, 15, 10, 150.0, 2, 2, "Tank");
			}
			else if (it->unitType == "K9")
			{
				new_unit = new Attack_Object_Type(it->newOwner, it->spawnPos, 30, 6, 90.0, 3, 3, "K9");
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



vector<Position> Command_Object_Type::getSpawnablePositions(vector<vector<Object*>>& _board) const
{
	vector<Position> spawnable_positions;
	int rows;
	int cols = _board[0].size();

	if (getName() == "PlayerA")
	{
		rows = 0;
	}
	else
	{
		rows = 11;
	}

	for (int i = 0; i < cols; i++)
	{
		spawnable_positions.push_back({ i, rows });
	}

	return spawnable_positions;
}