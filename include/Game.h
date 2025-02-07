#ifndef GAME_H
#define GAME_H

#include<vector>
#include<string>
#include<windows.h>
#include<iostream>
#include<conio.h>
#include"Object.h"
#include"Attack_Object_Type.h"
#include"Command_Object_Type.h"

#define BOARD_HEIGHT 12
#define BOARD_WIDTH 7
#define MAX_OBJECT_CNT 10

#define RED 0
#define BLUE 1

class Game{
private:
    int CursorX, CursorY;
    int Selected_X, Selected_Y;
    bool isSelected, attackMode, moveMode, spawnMode;
    int playerTurn;
    int playerCount;
    int turnCount;

    vector<vector<Object*>> board;
    vector<Object*> playerA;
    vector<Object*> playerB;

    HANDLE console;
public:
    Game();
    int CoinToss();
    void PrintRule();
    void Draw_Board();
    bool Check_MyObject();
    bool Check_ObjectState();
    void Check_KeyInput();
    void handleSpaceKey();
    void Run();
}
