#include<iostream>
#include<windows.h>
#include<vector>
#include<string>

using namespace std;

#define BOARD_HEIGHT 12
#define BOARD_WIDTH 7

class Object{
private:
    struct Position{
        int x;
        int y;
    }position;
    
    int damage;
    int defense;
    int hp;
    int move_distance;
};

class Game{
private:
    int CursorX, CursorY;
    int Selected_X, Selected_Y;
    bool Is_Selected, Attack_Time, Move_Time, Spawn_Time;
    
    vector<vector<Object*>> array; // 12 * 7
public:
    Game() : CursorX(0), CursorY(0), Selected_X(0), Selected_Y(0), Is_Selected(false), Attack_Time(flase), Move_Time(false), Spawn_Time(flase)
    {
        array.resize(BOARD_HEIGHT, vector<Object*>(BOARD_WIDTH, nullptr));
    }
    
    
    
    void Draw_Board()
    {
        //if(array)
    }
    void Check_KeyInput();
};


int main()
{
    Game test_board;
}
