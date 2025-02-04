#include"Game.h"

Game::Game() : CursorX(0), CursorY(0), Selected_X(0), Selected_Y(0),
isSelected(false), attackMode(false), moveMode(false), spawnMode(false)
{
  // playerTurn = 0 or 1 동전던지기 로직 여기 넣기 
  cout << "동전던지기를 실행합니다";

  board.resize(BOARD_HEIGHT, vector<Object*>(BOARD_WIDTH, nullptr));
  playerA.resize(MAX_OBJECT_CNT, nullptr);
  playerB.resize(MAX_OBJECT_CNT, nullptr);

  console = GetStdHandle(STD_OUTPUT_HANDLE);

  // 초기 지휘소 생성
  // board[0][3] =
  // board[11][3] = 
}

void Game::PrintRule()
{
  cout << endl;
  cout << "**** 게임 조작 방법 ****" << endl;
  cout << "화살표 키 : 커서 이동 " << endl;
  cout << "A키 : "
  // ...
  _getch();
}

void Draw_Board()
{
  system("cls");
  for(int i = 0; i < BOARD_HEIGHT; i++)
    {
      for (int j = 0; j < BOARD_WIDTH; j++)
        {
          // 수도코드로 작성해보자
          /*
            1. playerA, playerB vector != nullptr이면 그 벡터를 확인하면서 보드에 출력
            ex) for(int k=0; k < MAX_OBJECT_CNT; k++)
            {
              여기서 playerA이면 출력할때 RED , playerB이면 출력할때 BLUE
              상태가 before_action이면 배경색 넣기
            }
            2. nullptr이면 . 출력
            3. 커서 위치 색 넣기
            4. 공격 가능범위, 이동가능범위, 소환가능범위 색 넣기
            5. 남은 턴 표시해야됌 (항상) --> spawn list확인?
          */
    }
}

void Game::Run()
{
  PrintRule();
}
