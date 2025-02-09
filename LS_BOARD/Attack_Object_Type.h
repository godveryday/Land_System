#pragma once
#include"Object.h"
#include<vector>

using namespace std;

class Attack_Object_Type : public Object {
public:
	Attack_Object_Type(string owner, Position pos, int dmg, int def, float hp, int move_dist, int attack_range, string unit_type);
	void attack(Object& _target);
	bool canAttack(const Object& _target) const;
	vector<Position> getAttackablePositions(const vector<vector<Object*>>& _board) const;
	int getAttackRange() const;
	//void printPosition(const vector<Position>& positions);

private:
	int attackRange;
};