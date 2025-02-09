#include "Object.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

Object::Object(string _owner, Position _position, int _damage, int _defense, float _healthPoints, int _moveDistance, string _unitType)
	:owner(_owner), position(_position), damage(_damage), defense(_defense), healthPoints(_healthPoints), moveDistance(_moveDistance), state(beforeAction), unitType(_unitType)
{
}

// GET
int Object::getDefense() const
{
	return defense;
}

float Object::getHealthPoints() const
{
	return healthPoints;
}

Position Object::getPosition() const
{
	return position;
}

string Object::getName() const
{
	return owner;
}

string Object::getUnitType() const
{
	return unitType;
}

ObjectState Object::getObjectState() const {
	return state;
}

int Object::getMoveDistance() const
{
	return moveDistance;
}


// SET
void Object::setPosition(Position pos)
{
	position = pos;
}

void Object::setHealthPoints(float hp)
{
	healthPoints = hp;
}

void Object::setState(ObjectState _state)
{
	state = _state;
}


void Object::move(int _cursorX, int _cursorY, vector<vector<Object*>>& _board)
{
	vector<Position> movable_positions = getMovablePositions(_board);

	bool is_movable = false;

	for (const auto& pos : movable_positions)
	{
		if (pos.x == _cursorX && pos.y == _cursorY)
		{
			is_movable = true;
			break;
		}
	}

	if (!is_movable)
	{
		cout << "Cannot move" << endl;
		return;
	}

	else
	{
		if (_board[_cursorY][_cursorX] != nullptr)
		{
			cout << "이미 다른 객체가 있습니다.\n";
			return;
		}

		Position new_position = { _cursorX, _cursorY };
		_board[position.y][position.x] = nullptr;
		setPosition(new_position);
		_board[_cursorY][_cursorX] = this;
		state = afterMove;
	}
}


vector<Position> Object::getMovablePositions(vector<vector<Object*>>& _board) const
{
	vector<Position> movable_positions;
	int rows = _board.size();
	int cols = _board[0].size();
	for (int i = position.y - moveDistance; i <= position.y + moveDistance; i++)
	{
		for (int j = position.x - moveDistance; j <= position.x + moveDistance; j++)
		{
			float distance = sqrt(pow(abs(i - position.y), 2) + pow(abs(j - position.x), 2));
			if (distance <= moveDistance && j >= 0 && j < cols && i >= 0 && i < rows)
			{
				movable_positions.push_back({ j, i });
			}
		}
	}
	return movable_positions;
}



string Object::ObjectInfo() const
{
	ostringstream info;
	info << "Owner: " << owner << " Position: " << position.x << " " << position.y << " Damage: " << damage << " Defense: " << defense << " HealthPoints: " << healthPoints << " MoveDistance: " << moveDistance << " State: " << state;
	return info.str();
}


void Object::printPosition(const vector<Position>& positions)
{
	for (const auto& pos : positions)
	{
		cout << pos.x << " " << pos.y << endl;
	}
}