#pragma once
#include <string>
#include <vector>

using namespace std;

enum ObjectState {
	beforeAction,
	afterAttack,
	afterMove
};

struct Position
{
	int x;
	int y;
};
class Object
{
protected:
	string owner;
	Position position;
	int damage;
	int defense;
	float healthPoints;
	int moveDistance;
	ObjectState state;
	string unitType;

public:
	Object(string _owner, Position _position, int _damage, int _defense, float _healthPoints, int _moveDistance, string _unitType);
	virtual ~Object() = default;

	virtual void move(int _cursorX, int _cursorY, vector<vector<Object*>>& _board);
	vector<Position>getMovablePositions(vector<vector<Object*>>& _board) const;

	int getMoveDistance() const;
	int getDefense() const;
	float getHealthPoints() const;
	Position getPosition() const;
	string getName() const;
	string getUnitType() const;
	ObjectState getObjectState() const;


	void setHealthPoints(float hp);
	void setPosition(Position pos);
	// 추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌
	void setState(ObjectState _state);

	string ObjectInfo() const;
	void printPosition(const vector<Position>& positions);
};
