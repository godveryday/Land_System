// 2025/02/05 16:49 기준 update
// 이 코드 시나리오상 가상 문제는 attack_position을 for문 밖에서 색 설정하면, 그 위치에 상대방 object가 있을 때 색 적용이 안됌
#include"Game.h"

Game::Game() : CursorX(0), CursorY(0), Selected_X(0), Selected_Y(0),
    isSelected(false), attackMode(false), moveMode(false), spawnMode(false) // attackMode, moveMode, spawnMode를 enum바꿀 생각하기
{
  // playerTurn = 0 or 1 동전던지기 로직 여기 넣기 
  cout << "동전던지기를 실행합니다";

  board.resize(BOARD_HEIGHT, vector<Object*>(BOARD_WIDTH, nullptr));
  playerA.resize(MAX_OBJECT_CNT, nullptr);
  playerB.resize(MAX_OBJECT_CNT, nullptr);

  console = GetStdHandle(STD_OUTPUT_HANDLE);

  // 초기 지휘소 생성
  // playerA[0] = 
  // playerB[0]] =

  // 밑에 구문 없어도 되나? ㄴㄴ 해야됌, 생성, 소멸 시 보드와, 플레이어에 모두 적용해야됌
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
        /*
        setconsoleText(원상복구)
        for(int k=0; k<MAX_OBJECT_CNT, k++)
        {
            // 수정사항, 이동, 공격, 생성 범위 사항을 여기에 추가함, 이 로직은 생각해보기
            // 여기부터 이동, 공격, 생성 범위 출력에 대한 내용
            //playerA 객체에 대한 범위 업데이트
            if(selectedX == player[A].positionY && selectedY == player[k].positionX)
            {
                switch(mode)
                {
                    case attackMode:
                        attack_position = cout << playerA[k].range() // 범위를 여기서 받아서 업데이트 하고, for문 밖에서 출력 --> 수정
                    case moveMode:
                    case spawnMode:
                }
            }

            // playerB 객체에 대한 범위 업데이트
            // .. 위와 동일
        
            if(player[A]!=nullptr || playerB[k] != nullptr)
            {
                // playerA의 생성된 객체 출력
                if(i == playerA[k].positionY && j == playerA[k].positionX)
                {
                    setconsoleText(RED)
                    // playerA[k]의 객체 state에 따라서
                    // setconsoleText(배경색 설정)
                    cout << playerA[k].UnitType.str(0,1) // 이런식으로
                    setconsoleText(원상복구)
                }

                // playerB의 생성된 객체 출력
                // 똑같이
                if(i == player .. )
                
            }    
        } // end for문

        // 생성 대기 중 객체 출력
        if(playerA[0]->spawnlist != nullptr || playerB[0]->spawnlist !=nullptr )
        {
            if(i == player[0]->spawnlist.positionY && j == playerA[0]->spawnlist.positionX)
            {
                cout << playerA[0]->spawnlist.remainingTurn
            }
        }
        
        // **** 여기서 출력
        // 공격 가능범위, 이동가능범위, 생성가능범위를 표시할대 색상 설정만해줌
        for(auto it : attack_position)
        {
            if(i == it.y && j == it.x && attackMode)
            {
                setconsoleText()
            }
        }

        3. 커서 위치 색넣기
        if(i == CursorY && j )
        {
            setconsoleText()
        }

        2. nullptr이면 "."출력
        if(board[CursorY][CursorX] == nullptr)
        {
            cout << ".";
        }

        */
    }
}

// void Game::Check_KeyInput() 일단 그대로



    
void Game::Run()
{
  PrintRule();
}
