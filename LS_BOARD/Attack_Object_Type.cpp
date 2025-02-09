#include "Attack_Object_Type.h"
#include <iostream>
#include <cmath>

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
		int actual_damage = damage - _target.getDefense();
		float new_health = _target.getHealthPoints() - actual_damage;
		_target.setHealthPoints(new_health);
		cout << "target_HP: " << _target.getHealthPoints() << endl;
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

	int distance = sqrt(pow(target_pos.x - position.x, 2) + pow(target_pos.y - position.y, 2));
	cout << "distance: " << distance << endl;
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
			int distance = sqrt(dx * dx + dy * dy);

			if (new_x >= 0 && new_x < cols && new_y >= 0 && new_y < rows && distance <= attackRange)
			{
				if (_board[new_y][new_x] != nullptr && _board[new_y][new_x] != this)
				{
					attackable_positions.push_back({ new_x, new_y });
				}
			}

		}
	}

	return attackable_positions;
}