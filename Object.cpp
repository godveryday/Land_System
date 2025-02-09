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

		// K9과 Command의 상관관계 확인
		for (const auto& row : _board)
		{
			for (const auto& obj : row)
			{
				if (obj != nullptr)
				{
					if (obj->getUnitType() == "K9" && unitType == "Command" && obj->getName() == owner)
					{
						float distanceToK9 = sqrt(pow(_cursorX - obj->getPosition().x, 2) + pow(_cursorY - obj->getPosition().y, 2));
						if (distanceToK9 > 3)
						{
							cout << "Command는 K9의 위치로부터 거리 3을 벗어날 수 없습니다.\n";
							return;
						}
					}
					else if (obj->getUnitType() == "Command" && unitType == "K9" && obj->getName() == owner)
					{
						float distanceToCommand = sqrt(pow(_cursorX - obj->getPosition().x, 2) + pow(_cursorY - obj->getPosition().y, 2));
						if (distanceToCommand > 3)
						{
							cout << "K9는 Command의 위치로부터 거리 3을 벗어날 수 없습니다.\n";
							return;
						}
					}
				}
			}
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
	Position k9Pos = { -1, -1 };
	Position commandPos = { -1, -1 };

	//for (int i = position.y - moveDistance; i <= position.y + moveDistance; i++)
	//{
	//	for (int j = position.x - moveDistance; j <= position.x + moveDistance; j++)
	//	{
	//		float distance = sqrt(pow(abs(i - position.y), 2) + pow(abs(j - position.x), 2));
	//		if (distance <= moveDistance && j >= 0 && j < cols && i >= 0 && i < rows)
	//		{
	//			movable_positions.push_back({ j, i });
	//		}
	//	}
	//}
	// K9와 Command 유닛의 위치를 찾기
	for (const auto& row : _board)
	{
		for (const auto& obj : row)
		{
			if (obj != nullptr)
			{
				if (obj->getUnitType() == "K9" && obj->getName() == owner)
				{
					k9Pos = obj->getPosition();
				}
				else if (obj->getUnitType() == "Command" && obj->getName() == owner)
				{
					commandPos = obj->getPosition();
				}
			}
		}
	}

	// 이동 가능한 위치 계산
	for (int i = position.y - moveDistance; i <= position.y + moveDistance; i++)
	{
		for (int j = position.x - moveDistance; j <= position.x + moveDistance; j++)
		{
			float distance = sqrt(pow(abs(i - position.y), 2) + pow(abs(j - position.x), 2));
			if (distance <= moveDistance && j >= 0 && j < cols && i >= 0 && i < rows)
			{
				if (unitType == "K9" && commandPos.x != -1 && commandPos.y != -1)
				{
					float distanceToCommand = sqrt(pow(abs(i - commandPos.y), 2) + pow(abs(j - commandPos.x), 2));
					if (distanceToCommand <= 3)
					{
						movable_positions.push_back({ j, i });
					}
				}
				else if (unitType == "Command" && k9Pos.x != -1 && k9Pos.y != -1)
				{
					float distanceToK9 = sqrt(pow(abs(i - k9Pos.y), 2) + pow(abs(j - k9Pos.x), 2));
					if (distanceToK9 <= 3)
					{//근데 이런식으로 하면 처음 찾은 K9바탕으로 이동 가능 영역이 나옴
						movable_positions.push_back({ j, i });
					}
				}
				else if (unitType != "K9" && unitType != "Command")
				{
					movable_positions.push_back({ j, i });
				}
				else if (unitType == "Command" && (k9Pos.x == -1 || k9Pos.y == -1)) // K9가 없을 경우
				{   
					movable_positions.push_back({ j, i });
				}
			}
		}
	}
	return movable_positions;
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

bool Object::Dead(vector<vector<Object*>>& _board, vector<Object*>& _playerA, vector<Object*>& _playerB)
{
	if (healthPoints <= 0)
	{
		// 보드에서 제거
		_board[position.y][position.x] = nullptr;

		// playerA 또는 playerB의 벡터에서 제거
		if (owner == "PlayerA")
		{
			for (int i = 0; i < _playerA.size(); i++)
			{
				if (_playerA[i] == this)
				{
					_playerA[i] = nullptr;
					break;
				}
			}
		}
		else if (owner == "PlayerB")
		{
			for (int i = 0; i < _playerB.size(); i++)
			{
				if (_playerB[i] == this)
				{
					_playerB[i] = nullptr;
					break;
				}
			}
		}
		return true;
	}
	return false;
}