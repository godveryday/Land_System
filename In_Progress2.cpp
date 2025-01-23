#include<iostream>
#include<windows.h>
#include<vector>
#include<string>
#include<conio.h>

using namespace std;

#define BOARD_HEIGHT 12
#define BOARD_WIDTH 7

class Object {
protected:
    struct Position {
        int x;
        int y;
    }position;

    int damage;
    int defense;
    int hp;
    int move_distance;
    string name;

public:
    Object(int x, int y, int dmg, int def, int health, int move, string n) {
        position.x = x;
        position.y = y;
        damage = dmg;
        defense = def;
        hp = health;
        move_distance = move;
        name = n;
    }

    virtual ~Object() {}

    Position getPosition() { return position; }
    string getName() { return name; }
    bool isAlive() { return hp > 0; }
    int getMoveDistance() { return move_distance; }

    void takeDamage(int dmg) {
        hp -= max(0, dmg - defense);
    }
};


class Attacking_Object : public Object {
protected:
    int attack_range;

public:
    Attacking_Object(int x, int y, int dmg, int def, int hp, int move, int range, string name)
        : Object(x, y, dmg, def, hp, move, name), attack_range(range) {
    }

    virtual void move(int new_x, int new_y) {
        position.x = new_x;
        position.y = new_y;
    }

    virtual void attack(Object* target) {
        if (target) target->takeDamage(damage);
    }

    int getAttackRange() { return attack_range; }
};


class Command_Object : public Object {
protected:
    int spawn_range;

public:
    Command_Object(int x, int y, int def, int hp, int move, int range, string name)
        : Object(x, y, 0, def, hp, move, name), spawn_range(range) {
    }

    virtual void move(int new_x, int new_y) {
        position.x = new_x;
        position.y = new_y;
    }

    virtual Object* spawn(string unit_type, int x, int y) = 0;
    int getSpawnRange() { return spawn_range; }
};

class Tank : public Attacking_Object {
public:
    Tank(int x, int y) : Attacking_Object(x, y, 50, 30, 200, 2, 3, "T") {}
};

class K9 : public Attacking_Object {
public:
    K9(int x, int y) : Attacking_Object(x, y, 40, 20, 150, 3, 4, "K") {}
};

class Redback : public Attacking_Object {
public:
    Redback(int x, int y) : Attacking_Object(x, y, 30, 15, 100, 4, 2, "R") {}
};

class Soldier : public Attacking_Object {
public:
    Soldier(int x, int y) : Attacking_Object(x, y, 20, 10, 80, 5, 1, "S") {}
};

class CommandCenter : public Command_Object {
public:
    CommandCenter(int x, int y) : Command_Object(x, y, 40, 300, 0, 2, "C") {}

    Object* spawn(string unit_type, int x, int y) override {
        if (unit_type == "Tank") return new Tank(x, y);
        if (unit_type == "K9") return new K9(x, y);
        if (unit_type == "Redback") return new Redback(x, y);
        if (unit_type == "Soldier") return new Soldier(x, y);
        return nullptr;
    }
};





class Game {
private:
    int CursorX, CursorY;
    int Selected_X, Selected_Y;
    bool Is_Selected, Attack_Time, Move_Time, Spawn_Time;
    bool player_turn;  // true : player1, false : player2

    vector<vector<Object*>> board; // 12 * 7


public:
    Game() : CursorX(0), CursorY(0), Selected_X(0), Selected_Y(0), Is_Selected(false), Attack_Time(flase), Move_Time(false), Spawn_Time(flase)
    {
        board.resize(BOARD_HEIGHT, vector<Object*>(BOARD_WIDTH, nullptr));
        board[0][3] = new CommandCenter(0, 3);
        board[11][3] = new CommandCenter(11, 3);
    }


    void Draw_Board()
    {
        system("cls");
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);


        for (int i = 0; i < BOARD_HEIGHT; i++)
        {
            for (int j = 0; j < BOARD_WIDTH; j++) {
                if (i == CursorY && j == CursorX) {
                    SetConsoleTextAttribute(console, 0x0F);
                    cout << "[]";
                    SetConsoleTextAttribute(console, 0x07);
                }
                else if (Is_Selected && i == Selected_Y && j == Selected_X) {
                    SetConsoleTextAttribute(console, 0x70);
                    if (board[i][j] != nullptr)
                        cout << board[i][j]->getName() << " ";
                    else
                        cout << ". ";
                    SetConsoleTextAttribute(console, 0x07);
                }
                else if (find(possible_range.begin(), possible_range.end(),
                    make_pair(i, j)) != possible_range.end()) {
                    SetConsoleTextAttribute(console, 0x20);
                    cout << ". ";
                    SetConsoleTextAttribute(console, 0x07);
                }
                else if (board[i][j] != nullptr)
                    cout << board[i][j]->getName() << " ";
                else
                    cout << ". ";
            }
            cout << endl;
        }
    }


    void Check_KeyInput()
    {
        if (_kbhit())
        {
            int key = _getch();

            switch (key)
            {
            case 72: // 상
                if (CursorY > 0) CursorY--;
                break;
            case 80: //하
                if (CursorY < BOARD_HEIGHT - 1) CursorY++;
                break;
            case 75: //좌
                if (CursorX > 0) CursorX--;
                break;
            case 77: //우
                if (CursorX < BOARD_WIDTH - 1) CursorX++;
                break;
            case 'a':
            case 'A':
                Attack_Time = !Attack_Time;
                Move_Time = Spawn_Time = false;
                break;
            case 'v':     // 일단 M 은 77이랑 겹침  --> 수정
            case 'V':
                Move_Time = !Move_Time;
                Attack_Time = Spawn_Time = false;
                break;
            case 's':
            case 'S':
                Spawn_Time = !Spawn_Time;
                Attack_Time = Move_Time = false;
                break;
            case 't':
            case 'T':
                player_turn = !player_turn;
                break;
            }
        }
    }
};



/*
    void handleEnterKey() {
        if (!Is_Selected) {
            if (board[CursorY][CursorX] != nullptr) {
                Selected_X = CursorX;
                Selected_Y = CursorY;
                Is_Selected = true;
            }
        }
        else {
            vector<pair<int, int>> possible_range;

            if (Attack_Time) {
                possible_range = getPossibleRange("attack");
                if (find(possible_range.begin(), possible_range.end(),
                    make_pair(CursorY, CursorX)) != possible_range.end()) {
                    Attacking_Object* attacker = dynamic_cast<Attacking_Object*>(board[Selected_Y][Selected_X]);
                    if (attacker && board[CursorY][CursorX] != nullptr) {
                        attacker->attack(board[CursorY][CursorX]);
                    }
                }
            }
            else if (Move_Time) {
                possible_range = getPossibleRange("move");
                if (find(possible_range.begin(), possible_range.end(),
                    make_pair(CursorY, CursorX)) != possible_range.end()) {
                    board[CursorY][CursorX] = board[Selected_Y][Selected_X];
                    board[Selected_Y][Selected_X] = nullptr;
                }
            }
            else if (Spawn_Time) {
                possible_range = getPossibleRange("spawn");
                if (find(possible_range.begin(), possible_range.end(),
                    make_pair(CursorY, CursorX)) != possible_range.end()) {
                    CommandCenter* cc = dynamic_cast<CommandCenter*>(board[Selected_Y][Selected_X]);
                    if (cc) {
                        board[CursorY][CursorX] = cc->spawn("Tank", CursorY, CursorX);
                    }
                }
            }

            Is_Selected = false;
            Attack_Time = Move_Time = Spawn_Time = false;
        }
    }


*/



int main()
{
    Game test_board;
}
