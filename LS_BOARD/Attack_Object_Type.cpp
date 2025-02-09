#include "Attack_Object_Type.h"
#include <iostream>
#include <cmath>
#include <unordered_map>
#include<windows.h>

using namespace std;

Attack_Object_Type::Attack_Object_Type(string owner, Position pos, int dmg, int def, float hp, int move_dist, int attack_range, string unit_type)
	:Object(owner, pos, dmg, def, hp, move_dist, unit_type), attackRange(attack_range)
{
}
int Attack_Object_Type::getAttackRange() const
{
	return attackRange;
}

void Attack_Object_Type::attack(Object& _target)
{
	if (canAttack(_target))
	{

		// 유닛 타입에 따른 데미지 배율 정의
		unordered_map<string, unordered_map<string, float>> damageMultipliers = {
			{"Soldier", {{"Command", 1.5}}},
			{"Tank", {{"K9", 1.5}}},
			{"K9", {{"RedBack", 1.5}}},
			{"RedBack", {{"K9", 1.5}}},
		};

		// 기본 데미지 배율은 1.0
		float multiplier = 1.0;

		// 유닛 타입에 따른 데미지 배율 적용
		if (damageMultipliers.find(unitType) != damageMultipliers.end() &&
			damageMultipliers[unitType].find(_target.getUnitType()) != damageMultipliers[unitType].end())
		{
			multiplier = damageMultipliers[unitType][_target.getUnitType()];
		}


		// 데미지 계산 시 배율 적용
		int actual_damage = static_cast<int>((damage * multiplier) - _target.getDefense());
		if (actual_damage < 0) actual_damage = 0;  // 데미지가 음수가 되지 않도록 설정

		float new_health = _target.getHealthPoints() - actual_damage;
		_target.setHealthPoints(new_health);
		
		if (new_health <= 0)
		{
			// 여기 죽는 로직 추가하기 , 여기 죽는 로직 추가하기, 여기 죽는 로직 추가하기, 여기 죽는 로직 추가하기, 여기 죽는 로직 추가하기, 여기 죽는 로직 추가하기
		}

		cout << "target_HP: " << _target.getHealthPoints() << endl;
		Sleep(1000);
		state = afterAttack;
	}
	else
	{
		cout << "Cannot attack" << endl;
	}
}

bool Attack_Object_Type::canAttack(const Object& _target) const
{
	Position target_pos = _target.getPosition();

	if (owner == _target.getName())
	{
		cout << "같은 소유자";
		return false;
	}
	else
	{
		cout << "target_position: " << target_pos.x << " " << target_pos.y << endl;
		cout << "attacker_position: " << position.x << " " << position.y << endl;
	}

	// Soldier만 Command 객체를 타격할 수 있도록 조건 추가
	if (_target.getUnitType() == "Command" && unitType != "Soldier")
	{
		cout << "오직 Soldier만 Command 객체를 타격할 수 있습니다." << endl;
		return false;
	}

	int distance = sqrt(pow(target_pos.x - position.x, 2) + pow(target_pos.y - position.y, 2));
	return distance <= attackRange;
}


vector<Position> Attack_Object_Type::getAttackablePositions(const vector<vector<Object*>>& _board) const
{
	vector<Position> attackable_positions;
	int rows = _board.size();
	int cols = _board[0].size();

	for (int dx = -attackRange; dx <= attackRange; dx++)
	{
		for (int dy = -attackRange; dy <= attackRange; dy++)
		{
			int new_x = position.x + dx;
			int new_y = position.y + dy;
			int distance = abs(dx) + abs(dy); // 맨해튼 거리 계산, 체크해야됌

			if (new_x >= 0 && new_x < cols && new_y >= 0 && new_y < rows && distance <= attackRange)
			{
				attackable_positions.push_back({ new_x, new_y });
			}
		}
	}
	return attackable_positions;
}