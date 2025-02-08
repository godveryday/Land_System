#include<cstdlib>
#include<ctime>
#include"Game.h"
//#include"Draw_Title.h"

Game::Game() : CursorX(0), CursorY(0), Selected_X(0), Selected_Y(0),
isSelected(false), attackMode(false), moveMode(false), spawnMode(false)
{
    // Test용으로 임시 주석 ON/OFF
    //Draw_Title();
    Sleep(500);
    PrintRule();

    // 플레이어 턴 동전던지기
    cout << endl << "동전 던지기를 진행합니다" << endl;
    playerTurn = CoinToss();

    // Test용으로 임시 주석 ON/OFF
    // playerTurn = RED;

    if (playerTurn == RED)
    {
        cout << "PlayerA : 선 플레이어";
    }
    else
    {
        cout << "PlayerB : 선 플레이어";
    }
    Sleep(800);

    //초기화
    board.resize(BOARD_HEIGHT, vector<Object*>(BOARD_WIDTH, nullptr));
    playerA.resize(MAX_OBJECT_CNT, nullptr);
    playerB.resize(MAX_OBJECT_CNT, nullptr);

    console = GetStdHandle(STD_OUTPUT_HANDLE);

    // 초기 지휘소 생성
    // owner, position, damage, defense, healthPoints, moveDistance
    playerA[0] = new Command_Object_Type("PlayerA", { 3,0 }, 0, 10, 100, 1);
    playerB[0] = new Command_Object_Type("PlayerB", { 3,11 }, 0, 10, 100, 1);

    // 보드 위에 지휘소 객체 생성
    board[0][3] = playerA[0];
    board[11][3] = playerB[0];
}

// 동전 던지기
int Game::CoinToss()
{
    srand(time(0));
    int result = rand() % 2;

    return result;
}

// 규칙 설명
void Game::PrintRule()
{
    cout << endl;
    cout << "**** 게임 조작 방법 ****" << endl;
    cout << "화살표 키 : 커서 이동 " << endl;
    cout << "A키 : 공격 모드 전환" << endl;
    cout << "V키 : 이동 모드 전환" << endl;
    cout << "S키 : 유닛 소환 모드 전환" << endl;
    cout << "T키 : 턴 전환" << endl;
    cout << "스페이스바 : 선택/행동 수행" << endl;
    cout << "게임을 시작하려면 아무 키나 누르세요..." << endl;
    _getch(); // 사용자가 아무 키나 누를 때까지 대기
}

// 보드 출력
void Game::Draw_Board()
{
    system("cls");

    // 공격/이동/생성 가능 범위를 저장
    vector<Position> range_positions;

    if (isSelected)
    {
        for (int k = 0; k < MAX_OBJECT_CNT; k++)
        {
            Object* currentObject = (playerTurn == RED) ? playerA[k] : playerB[k];
            if (currentObject)
            {
                Position position = currentObject->getPosition();
                if (position.x == Selected_X && position.y == Selected_Y)
                {
                    if (attackMode)
                    {
                        Attack_Object_Type* attacker = dynamic_cast<Attack_Object_Type*>(currentObject);
                        range_positions = attacker->getAttackablePositions(board);
                        //  break 넣을지 말지 생각해서 결정 checkcheckcheckcheckcheckcheckcheck
                    }

                    // move 동작 얼추 검증완료
                    else if (moveMode)
                    {
                        range_positions = currentObject->getMovablePositions(board);
                    }

                    else if (spawnMode)
                    {
                        Command_Object_Type* commander = dynamic_cast<Command_Object_Type*>(currentObject);
                        // range_positions = commander->getSpawnablePositions();
                    }
                }
            }
        }
    }

    for (int i = 0; i < BOARD_HEIGHT; i++)
    {
        for (int j = 0; j < BOARD_WIDTH; j++)
        {
            bool isRangePosition = false;
            bool hasUnit = false;
            int unitColor = -1;
            string unitDisplay;

            // 현재 위치가 공격/이동/생성 범위에 해당하는지 판단
            for (auto it : range_positions)
            {
                if (i == it.y && j == it.x)
                {
                    isRangePosition = true;
                    break;
                }
            }

            // 현재 위치의 유닛 확인
            for (int k = 0; k < MAX_OBJECT_CNT; k++)
            {
                if (playerA[k])
                {
                    Position positionA = playerA[k]->getPosition();
                    if (i == positionA.y && j == positionA.x)
                    {
                        hasUnit = true;
                        unitColor = RED;
                        unitDisplay = playerA[k]->getUnitType().substr(0, 1);
                        break;
                    }
                }
                if (playerB[k])
                {
                    Position positionB = playerB[k]->getPosition();
                    if (i == positionB.y && j == positionB.x)
                    {
                        hasUnit = true;
                        unitColor = BLUE;
                        unitDisplay = playerB[k]->getUnitType().substr(0, 1);;
                        break;
                    }
                }
            }

            // 소환 대기 유닛 출력
            if (hasUnit)
            {
                Command_Object_Type* commandA = dynamic_cast<Command_Object_Type*>(playerA[0]);
                Command_Object_Type* commandB = dynamic_cast<Command_Object_Type*>(playerB[0]);

                // playerA의 소환 대기 유닛 확인
                if (!commandA->spawnRequests.empty() &&
                    commandA->spawnRequests[0].spawnPos.y == i &&
                    commandA->spawnRequests[0].spawnPos.x == j)
                {
                    SetConsoleTextAttribute(console, FOREGROUND_RED);
                    cout << commandA->spawnRequests[0].remainingTurns;
                    continue;
                }


                // playerB의 소환 대기 유닛 확인
                if (!commandB->spawnRequests.empty() &&
                    commandB->spawnRequests[0].spawnPos.y == i &&
                    commandB->spawnRequests[0].spawnPos.x == j)
                {
                    SetConsoleTextAttribute(console, FOREGROUND_BLUE);
                    cout << commandB->spawnRequests[0].remainingTurns;
                    continue;
                }
            }


            // START
            // 상태에 따른 색상 설정
            // 1. 현재 커서 배경 흰색
            if (i == CursorY && j == CursorX)
            {
                SetConsoleTextAttribute(console, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
            }
            // 2. 공격/이동/생성 범위 색 설정
            else if (isRangePosition)
            {
                // 해당 범위 내 유닛이 있을 때
                if (hasUnit)
                {
                    if (attackMode) // 공격범위 : 노란색
                    {
                        SetConsoleTextAttribute(console, BACKGROUND_RED | BACKGROUND_GREEN |    // 노란색 배경
                            (unitColor == RED ? FOREGROUND_RED : FOREGROUND_BLUE)); // 유닛 색상 유지
                    }

                    else if (moveMode) // 이동범위 : 초록색
                    {
                        SetConsoleTextAttribute(console, BACKGROUND_GREEN |  // 초록색 배경
                            (unitColor == RED ? FOREGROUND_RED : FOREGROUND_BLUE)); // 유닛 색상 유지

                    }

                    else if (spawnMode) // 생성범위 : 보라색
                    {
                        SetConsoleTextAttribute(console, BACKGROUND_RED | BACKGROUND_BLUE | // 보라색 배경
                            (unitColor == RED ? FOREGROUND_RED : FOREGROUND_BLUE));  // 유닛 색상 유지
                    }
                }

                // 해당 범위 내 유닛이 없을 때
                else
                {
                    if (attackMode) // 공격범위 : 노란색
                    {
                        SetConsoleTextAttribute(console, BACKGROUND_RED | BACKGROUND_GREEN); // 노란색 배경
                    }

                    else if (moveMode) // 이동범위 : 초록색
                    {
                        SetConsoleTextAttribute(console, BACKGROUND_GREEN);  // 초록색 배경
                    }

                    else if (spawnMode) // 생성범위 : 보라색
                    {
                        SetConsoleTextAttribute(console, BACKGROUND_RED | BACKGROUND_BLUE); // 보라색 배경
                    }
                }
            }

            else if (hasUnit)
            {
                if (unitColor == RED)
                {
                    SetConsoleTextAttribute(console, FOREGROUND_RED); // RED 색상
                }

                else
                {
                    SetConsoleTextAttribute(console, FOREGROUND_BLUE); // BLUE 색상
                }
            }

            else
            {
                SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            // END


            // 적절한 문자 출력
            if (hasUnit)
            {
                cout << unitDisplay;
            }
            else
            {
                cout << ".";
            }

            SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

        }
        cout << endl << endl;
    }

    cout << "이동/공격/생성 모드 : " << moveMode << attackMode << spawnMode << endl;
    cout << "턴 : " << turnCount << endl;
    cout << "플레이어 : " << playerTurn << endl;
    cout << "플레이어 카운트 : " << playerCount << endl;
    cout << "선택 : " << isSelected << endl;
    cout << "선택된 X : " << Selected_X << endl;
    cout << "선택된 Y : " << Selected_Y << endl;
    cout << "커서 X : " << CursorX << endl;
    cout << "커서 Y : " << CursorY << endl;

}


// 해당 커서 위 객체가 내 객체인지 파악
bool Game::Check_MyObject()
{
    if (board[CursorY][CursorX]->getName() == "PlayerA" && playerTurn == RED)
    {
        return true;
    }

    else if (board[CursorY][CursorX]->getName() == "PlayerB" && playerTurn == BLUE)
    {
        return true;
    }

    else
    {
        return false;
    }
}

// 객체의 상태를 파악
bool Game::Check_ObjectState()
{
    if (board[CursorY][CursorX]->getObjectState() == beforeAction)
    {
        return true;
    }

    else
    {
        return false;
    }
}

void Game::updatePlayerState()  
{
    for (int k = 0; k < MAX_OBJECT_CNT; k++)
    {
        if (playerA[k])
        {
            playerA[k]->setState(beforeAction);
        }

        if (playerB[k])
        {
            playerB[k]->setState(beforeAction);
        }
    }
}


// 키 입력 후 상태변경
void Game::Check_KeyInput()
{
    // ctrl+c 처리 구문 추가해야됌(항복)

    int key;
    int cnt = 0;

    vector<Object*> currentVector = (playerTurn == RED) ? playerA : playerB;
    for (auto it : currentVector)
    {
        if (it != nullptr && it->getObjectState() == beforeAction)
        {
            cnt++;
        }
    }

    if (cnt == 0)
    {
        key = 84; // t, T
        cout << "더 이상 할 수 있는 액션이 없습니다. 턴을 넘깁니다" << endl;
    }
    else
    {
        key = _getch();
    }

    switch (key) {

    case 72:  //상
        if (CursorY > 0) CursorY--;
        break;

    case 80:  //하
        if (CursorY < BOARD_HEIGHT - 1) CursorY++;
        break;

    case 75:  //좌
        if (CursorX > 0) CursorX--;
        break;

    case 77:  //우
        if (CursorX < BOARD_WIDTH - 1) CursorX++;
        break;

    case 'a': // 97
    case 'A': // 65
        if (board[CursorY][CursorX] != nullptr)
        {
            // 나의 객체인지 아닌지 파악
            if (Check_MyObject())
            {
                // 공격 객체인지 아닌지 파악
                if (dynamic_cast<Attack_Object_Type*>(board[CursorY][CursorX]))
                {
                    // 객체의 액션 전/후 상태 확인
                    if (Check_ObjectState())
                    {
                        Selected_X = CursorX;
                        Selected_Y = CursorY;
                        isSelected = true;
                        attackMode = !attackMode;
                        moveMode = spawnMode = false;
                    }

                    else
                    {
                        cout << "이미 액션을 수행한 객체입니다" << endl;
                    }
                }

                else
                {
                    cout << "공격 객체가 아닙니다" << endl;
                }
            }

            else
            {
                cout << "나의 객체가 아닙니다" << endl;
            }
        }
        break;

    case 'v': // 118
    case 'V': // 86
        if (board[CursorY][CursorX] != nullptr)
        {
            // 나의 객체인지 아닌지 파악
            if (Check_MyObject())
            {
                // 객체의 액션 전/후 상태 확인
                if (Check_ObjectState())
                {
                    Selected_X = CursorX;
                    Selected_Y = CursorY;
                    isSelected = true;
                    moveMode = !moveMode;
                    attackMode = spawnMode = false;
                }

                else
                {
                    cout << "이미 액션을 수행한 객체입니다" << endl;
                }
            }

            else
            {
                cout << "나의 객체가 아닙니다" << endl;
            }
        }
        break;

    case 's': // 115
    case 'S': // 83
        if (board[CursorY][CursorX] != nullptr)
        {
            // 나의 객체인지 아닌지 파악
            if (Check_MyObject())
            {
                // 지휘소 객체인지 아닌지 파악
                if (dynamic_cast<Command_Object_Type*>(board[CursorY][CursorX]))
                {
                    // 객체의 액션 전/후 상태 확인
                    if (Check_ObjectState())
                    {
                        Selected_X = CursorX;
                        Selected_Y = CursorY;
                        isSelected = true;
                        spawnMode = !spawnMode;
                        moveMode = attackMode = false;
                    }

                    else
                    {
                        cout << "이미 액션을 수행한 객체입니다" << endl;
                    }
                }

                else
                {
                    cout << "지휘소 객체가 아닙니다" << endl;
                }
            }

            else
            {
                cout << "나의 객체가 아닙니다" << endl;
            }
        }
        break;

    case 32: // 스페이스바
        handleSpaceKey();
        break;

    case 't': // 116
    case 'T': // 84
        if (playerTurn == RED)
        {
            playerTurn = BLUE;
            playerCount++;
        }

        else
        {
            playerTurn = RED;
            playerCount++;
        }

        if (playerCount == 2)
        {
            playerCount = 0;
            turnCount++;

            // 모든 객체 State를 BEFORE_ACTION으로 
            // 추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌추가됌
            updatePlayerState();
            
            // 턴 감소
            // dynamic_cast<Command_Object_Type*>(board[CursorY][CursorX]->updateSpawnRequests(board, playerA, playerB));
            // 
        }
        break;

    }
}


void Game::handleSpaceKey()
{
    if (!isSelected)
    {
        if (board[CursorY][CursorX] != nullptr)
        {
            Selected_X = CursorX;
            Selected_Y = CursorY;
            isSelected = true;
        }
    }
    else
    {
        if (attackMode)
        {
            Attack_Object_Type* attacker = dynamic_cast<Attack_Object_Type*>(board[Selected_Y][Selected_X]);
            if (attacker && board[CursorY][CursorX] != nullptr)
            {
                // 여기 알고리즘 can_attack 등으로 수정
                int dist = abs(CursorY - Selected_Y) + abs(CursorX - Selected_X);
                if (dist <= attacker->getAttackRange())
                {
                    attacker->attack(*board[CursorY][CursorX]);

                }
            }
        }

        // 이동 동작 검증 얼추 완료
        else if (moveMode)
        {
            cout << "Space Key3" << endl;
            Object* selected_obj = board[Selected_Y][Selected_X];
            if (selected_obj != nullptr)
            {
                selected_obj->move(CursorX, CursorY, board);
                //isSelected = false;
                //moveMode = false;
            }
        }

        // 수정 필요
        else if (spawnMode)
        {
            Command_Object_Type* command = dynamic_cast<Command_Object_Type*>(board[Selected_Y][Selected_X]);
            if (command && board[CursorY][CursorX] != nullptr)
            {
                if (command->canSpawn({ CursorX, CursorY }, board) && has_empty_slot(playerA))
                {
                    int choice;
                    cout << "생성할 유닛을 선택하세요" << endl;
                    cout << "1. Soldier (1 Turn)" << endl << "2. Redback (2 Turn)" << endl << "3. Tank (2 Turn)" << endl << "4. K9 (3 Turn)" << endl;
                    cout << "입력 : ";
                    cin >> choice;

                    switch (choice) {
                    case 1:
                        command->requestSpawn("Soldier", { CursorX, CursorY }, board, "PlayerA");
                        break;

                    case 2:
                        command->requestSpawn("Redback", { CursorX, CursorY }, board, "PlayerA");
                        break;

                    case 3:
                        command->requestSpawn("Tank", { CursorX, CursorY }, board, "PlayerA");
                        break;

                    case 4:
                        command->requestSpawn("K9", { CursorX, CursorY }, board, "PlayerA");
                        break;

                    default:
                        cout << "잘못된 선택입니다" << endl;
                        break;

                    }
                }
            }
        }

        isSelected = false;
        attackMode = moveMode = spawnMode = false;
    }
}


bool Game::has_empty_slot(vector<Object*>& player)
{
    for (auto& obj : player)
    {
        if (obj == nullptr)
        {
            cout << "dd";
            return true;
        }
    }
    cout << "full";
    return false;
}


void Game::Run()
{
    while (true)
    {
        Draw_Board();
        Check_KeyInput();
    }
}
