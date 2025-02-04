#ifndef GAME_H
#define GAME_H

#include<vector>
#include<string>
#include<windows.h>
#include<conio.h>
#include"Object.h"
#include"Attack_Object_Type.h"
#include"Command_Object_Type.h"

#define BOARD_HEIGHT 12
#define BOARD_WIDTH 7
#define MAX_OBJECT_CNT 10

// 누구 턴인지 알기 쉽게하기 위해 정의
#define RED 0
#define BLUE 1

using namespace std;

class Game{
private:
  int CursorX, CursorY;
  int Selected_X, Selected_Y;
  bool isSelected, attackMode, moveMode, spawnMode;
  int playerTurn;

  vector<vector<Object*>> board;
  vector<Object*> playerA;
  vector<Object*> playerB;

  // setconsoleTextAttribute에 사용하기 위해 정의
  HANDLE console;

public:
  Game();
  void PrintRule();
  void Draw_Board();
  void Check_KeyInput();
  void handleSpaceKey();
  void Run();
  // void checkPlayerObject();
};

#endif







