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
        for(int i=0; i<BOARD_HEIGHT; i++)
            {
                for(int j=0; j<BOARD_WIDTH; j++)
                    {
                        if(array[i][j] == nullptr)
                        {
                            cout << '.' << " ";
                        }
                        /*
                        else
                        {
                            cout << <Dynamic>array.name;
                        }
                        */
                    }
            }
    }
    void Check_KeyInput()
    {
        if(_kbhit())
        {
            int key = _getch();
            
            switch(key)
            {
                case 72: // 상
                    if(CursorY > 0) CursorY--;
                    break;
                case 80: //하
                    if (CursorY < BOARD_HEIGHT -1) CursorY++;
                    break;
                case 75: //좌
                    if(CursorX > 0) CursorX--;
                    break;
                case 77: //우
                    if(CursorX < BOARD_WIDTH -1) CursorX++;
                    break;
                case (A number) :
                    if(!Is_Selected && array[CursorX][CursorY] != nullptr)
                    {
                        // possibleAttacks = ..
                    }
            }
        }
    }
};


int main()
{
    Game test_board;
}
