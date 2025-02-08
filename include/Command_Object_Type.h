#pragma once
#include"Object.h"
#include<unordered_map>
#include<vector>

using namespace std;

struct SpawnRequest
{
	string unitType;
	Position spawnPos;
	string newOwner;
	int remainingTurns;
};

class Command_Object_Type : public Object {
public:
	Command_Object_Type(string owner, Position pos, int dmg, int def, float hp, int move_dist);
	void requestSpawn(const string& _unitType, const Position& spawnPos, const vector<vector<Object*>>& _board, const string& _newOwner);
	bool canSpawn(const Position& _spawnPos, const vector<vector<Object*>>& _board) const;
	void updateSpawnRequests(vector<vector<Object*>>& _board, vector<Object*>& _player);
	vector<SpawnRequest> spawnRequests;

private:
	unordered_map<string, int> unitSpawnTime =
	{
		{"Soldier",1},
		{"RedBack",2},
		{"Tank",5},
		{"K9",3}
	};

};
