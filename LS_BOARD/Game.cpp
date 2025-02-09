#include<cstdlib>
#include<ctime>
#include"Game.h"
//#include"Draw_Title.h"

Game::Game() : CursorX(0), CursorY(0), Selected_X(0), Selected_Y(0),
isSelected(false), attackMode(false), moveMode(false), spawnMode(false)
{
    // Test������ �ӽ� �ּ� ON/OFF
    //Draw_Title();
    // Sleep(500);
    PrintRule();

    // �÷��̾� �� ����������
    cout << endl << "���� �����⸦ �����մϴ�" << endl;
    playerTurn = CoinToss();

    // Test������ �ӽ� �ּ� ON/OFF
    // playerTurn = RED;

    if (playerTurn == RED)
    {
        cout << "�� �÷��̾� : PlayerA";
    }
    else
    {
        cout << "�� �÷��̾� : PlayerB";
    }
    Sleep(1500);

    //�ʱ�ȭ
    board.resize(BOARD_HEIGHT, vector<Object*>(BOARD_WIDTH, nullptr));
    playerA.resize(MAX_OBJECT_CNT, nullptr);
    playerB.resize(MAX_OBJECT_CNT, nullptr);

    console = GetStdHandle(STD_OUTPUT_HANDLE);

    // �ʱ� ���ּ� ����
    // owner, position, damage, defense, healthPoints, moveDistance
    playerA[0] = new Command_Object_Type("PlayerA", { 3,0 }, 0, 10, 100, 1);
    playerB[0] = new Command_Object_Type("PlayerB", { 3,11 }, 0, 10, 100, 1);

    // ���� ���� ���ּ� ��ü ����
    board[0][3] = playerA[0];
    board[11][3] = playerB[0];
}

// ���� ������
int Game::CoinToss()
{
    srand(time(0));
    int result = rand() % 2;

    return result;
}

// ��Ģ ����
void Game::PrintRule()
{
    cout << endl;
    cout << "**** ���� ���� ��� ****" << endl;
    cout << "ȭ��ǥ Ű : Ŀ�� �̵� " << endl;
    cout << "AŰ : ���� ��� ��ȯ" << endl;
    cout << "VŰ : �̵� ��� ��ȯ" << endl;
    cout << "SŰ : ���� ��ȯ ��� ��ȯ" << endl;
    cout << "TŰ : �� ��ȯ" << endl;
    cout << "�����̽��� : ����/�ൿ ����" << endl;
    cout << "Ctrl + C : �׺�" << endl << endl;
    cout << "������ �����Ϸ��� �ƹ� Ű�� ��������..." << endl;
    _getch(); // ����ڰ� �ƹ� Ű�� ���� ������ ���
}


// ���� �������
void Game::PrintCurrent()
{
    cout << "�÷��̾� : ";
    if (playerTurn == 0) cout << "RED";
    else if (playerTurn == 1) cout << "BLUE";
    cout << endl;

    cout << "���� ��� : ";
    if (moveMode) cout << "�̵�";
    else if (attackMode) cout << "����";
    else if (spawnMode) cout << "����";
    else cout << "���";
    cout << endl;

    cout << "���� : ";
    if (isSelected) cout << "���� O";
    else cout << "���� X";
    cout << endl;

    cout << "���õ� X : " << Selected_X << endl;
    cout << "���õ� Y : " << Selected_Y << endl;
    cout << "Ŀ�� X : " << CursorX << endl;
    cout << "Ŀ�� Y : " << CursorY << endl;

    cout << "�÷��̾� ī��Ʈ : " << playerCount << endl;
    cout << "�� : " << turnCount << endl;
}

// ���� ���
void Game::Draw_Board()
{
    system("cls");

    // ����/�̵�/���� ���� ������ ����
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
                        //  break ������ ���� �����ؼ� ���� checkcheckcheckcheckcheckcheckcheck
                    }

                    // move ���� ���� �����Ϸ�
                    else if (moveMode)
                    {
                        range_positions = currentObject->getMovablePositions(board);
                    }

                    else if (spawnMode)
                    {
                        Command_Object_Type* commander = dynamic_cast<Command_Object_Type*>(currentObject);
                        range_positions = commander->getSpawnablePositions(board);
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

            // ���� ��ġ�� ����/�̵�/���� ������ �ش��ϴ��� �Ǵ�
            for (auto it : range_positions)
            {
                if (i == it.y && j == it.x)
                {
                    isRangePosition = true;
                    break;
                }
            }

            // ���� ��ġ�� ���� Ȯ��
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

            // ��ȯ ��� ���� ���
            if (!hasUnit)
            {
                Command_Object_Type* commandA = dynamic_cast<Command_Object_Type*>(playerA[0]);
                Command_Object_Type* commandB = dynamic_cast<Command_Object_Type*>(playerB[0]);

                // playerA�� ��ȯ ��� ���� Ȯ��
                if (!commandA->spawnRequests.empty() &&
                    commandA->spawnRequests[0].spawnPos.y == i &&
                    commandA->spawnRequests[0].spawnPos.x == j)
                {
                    SetConsoleTextAttribute(console, FOREGROUND_RED);
                    cout << commandA->spawnRequests[0].remainingTurns;
                    continue;
                }


                // playerB�� ��ȯ ��� ���� Ȯ��
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
            // ���¿� ���� ���� ����
            // 1. ���� Ŀ�� ��� ���
            if (i == CursorY && j == CursorX)
            {
                SetConsoleTextAttribute(console, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
            }
            // 2. ����/�̵�/���� ���� �� ����
            else if (isRangePosition)
            {
                // �ش� ���� �� ������ ���� ��
                if (hasUnit)
                {
                    if (attackMode) // ���ݹ��� : �����
                    {
                        SetConsoleTextAttribute(console, BACKGROUND_RED | BACKGROUND_GREEN |    // ����� ���
                            (unitColor == RED ? FOREGROUND_RED : FOREGROUND_BLUE)); // ���� ���� ����
                    }

                    else if (moveMode) // �̵����� : �ʷϻ�
                    {
                        SetConsoleTextAttribute(console, BACKGROUND_GREEN |  // �ʷϻ� ���
                            (unitColor == RED ? FOREGROUND_RED : FOREGROUND_BLUE)); // ���� ���� ����

                    }

                    else if (spawnMode) // �������� : �����
                    {
                        SetConsoleTextAttribute(console, BACKGROUND_RED | BACKGROUND_BLUE | // ����� ���
                            (unitColor == RED ? FOREGROUND_RED : FOREGROUND_BLUE));  // ���� ���� ����
                    }
                }

                // �ش� ���� �� ������ ���� ��
                else
                {
                    if (attackMode) // ���ݹ��� : �����
                    {
                        SetConsoleTextAttribute(console, BACKGROUND_RED | BACKGROUND_GREEN); // ����� ���
                    }

                    else if (moveMode) // �̵����� : �ʷϻ�
                    {
                        SetConsoleTextAttribute(console, BACKGROUND_GREEN);  // �ʷϻ� ���
                    }

                    else if (spawnMode) // �������� : �����
                    {
                        SetConsoleTextAttribute(console, BACKGROUND_RED | BACKGROUND_BLUE); // ����� ���
                    }
                }
            }

            else if (hasUnit)
            {
                if (unitColor == RED)
                {
                    SetConsoleTextAttribute(console, FOREGROUND_RED); // RED ����
                }

                else
                {
                    SetConsoleTextAttribute(console, FOREGROUND_BLUE); // BLUE ����
                }
            }

            else
            {
                SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            // END


            // ������ ���� ���
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
        cout << endl;
    }

    cout << endl;
    PrintCurrent();
}


// �ش� Ŀ�� �� ��ü�� �� ��ü���� �ľ�
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

// ��ü�� ���¸� �ľ�
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


// Ű �Է� �� ���º���
void Game::Check_KeyInput()
{
    // ctrl+c ó�� ���� �߰��ؾ߉�(�׺�)

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
        cout << "�� �̻� �� �� �ִ� �׼��� �����ϴ�. ���� �ѱ�ϴ�" << endl;
    }
    else
    {
        key = _getch();
    }

    switch (key) {

    case 72:  //��
        if (CursorY > 0) CursorY--;
        break;

    case 80:  //��
        if (CursorY < BOARD_HEIGHT - 1) CursorY++;
        break;

    case 75:  //��
        if (CursorX > 0) CursorX--;
        break;

    case 77:  //��
        if (CursorX < BOARD_WIDTH - 1) CursorX++;
        break;

    case 'a': // 97
    case 'A': // 65
        if (board[CursorY][CursorX] != nullptr)
        {
            // ���� ��ü���� �ƴ��� �ľ�
            if (Check_MyObject())
            {
                // ���� ��ü���� �ƴ��� �ľ�
                if (dynamic_cast<Attack_Object_Type*>(board[CursorY][CursorX]))
                {
                    // ��ü�� �׼� ��/�� ���� Ȯ��
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
                        cout << "�̹� �׼��� ������ ��ü�Դϴ�" << endl;
                    }
                }

                else
                {
                    cout << "���� ��ü�� �ƴմϴ�" << endl;
                }
            }

            else
            {
                cout << "���� ��ü�� �ƴմϴ�" << endl;
            }
        }
        break;

    case 'v': // 118
    case 'V': // 86
        if (board[CursorY][CursorX] != nullptr)
        {
            // ���� ��ü���� �ƴ��� �ľ�
            if (Check_MyObject())
            {
                // ��ü�� �׼� ��/�� ���� Ȯ��
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
                    cout << "�̹� �׼��� ������ ��ü�Դϴ�" << endl;
                }
            }

            else
            {
                cout << "���� ��ü�� �ƴմϴ�" << endl;
            }
        }
        break;

    case 's': // 115
    case 'S': // 83
        if (board[CursorY][CursorX] != nullptr)
        {
            // ���� ��ü���� �ƴ��� �ľ�
            if (Check_MyObject())
            {
                // ���ּ� ��ü���� �ƴ��� �ľ�
                if (dynamic_cast<Command_Object_Type*>(board[CursorY][CursorX]))
                {
                    // ��ü�� �׼� ��/�� ���� Ȯ��
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
                        cout << "�̹� �׼��� ������ ��ü�Դϴ�" << endl;
                    }
                }

                else
                {
                    cout << "���ּ� ��ü�� �ƴմϴ�" << endl;
                }
            }

            else
            {
                cout << "���� ��ü�� �ƴմϴ�" << endl;
            }
        }
        break;

    case 32: // �����̽���
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

            // ��� ��ü State�� BEFORE_ACTION���� 
            updatePlayerState();
            
            // �� ����
            dynamic_cast<Command_Object_Type*>(playerA[0])->updateSpawnRequests(board, playerA);
            dynamic_cast<Command_Object_Type*>(playerB[0])->updateSpawnRequests(board, playerB);

        }
        break;

    case 3:  // ctrl+c
        if (playerTurn == 0)
            cout << "RED�� �׺��߽��ϴ�" << endl;
        else
            cout << "BLUE�� �׺��߽��ϴ�" << endl;
        exit(0);
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
                attacker->attack(*board[CursorY][CursorX]);
            }
        }
        
        // �̵� ���� ���� ���� �Ϸ�
        else if (moveMode)
        {
            Object* selected_obj = board[Selected_Y][Selected_X];
            if (selected_obj != nullptr)
            {
                selected_obj->move(CursorX, CursorY, board);
            }
        }

        else if (spawnMode)
        { 
            Command_Object_Type* command = dynamic_cast<Command_Object_Type*>(board[Selected_Y][Selected_X]);
            vector<Position> spawnable_positions = command->getSpawnablePositions(board);
            bool check_spawnable = false;

            // owner�� Ȯ��, owner�� ����, playerName, player�� ���� (���� �Ű������� ��)
            string playerName = command->getName();
            vector<Object*> player = (playerName == "PlayerA") ? playerA : playerB;
            

            // ������� �Ʒ� �Լ��� ��� can_spawn()�Լ��� ����־�߉� //
            for (auto it : spawnable_positions)
            {
                if (CursorX == it.x && CursorY == it.y)
                {
                    check_spawnable = true;
                    break;
                }
            }

            // ���� ��ġ��, ���������� ��ġ���� �Ǵ�
            if (check_spawnable)
            {
                // �ش� ��ġ�� ��ü�� �ִ����� �Ǵ�
                if (command && board[CursorY][CursorX] == nullptr)
                {
                    // player ��ü�� 10�� �������� �Ǵ�
                    if (has_empty_slot(player))
                    {
                        // Ŀ�ǵ��� ���� ������ ����ִ��� �Ǵ�
                        if (command->spawnRequests.empty())
                        {
                            int choice;
                            cout << "������ ������ �����ϼ���" << endl;
                            cout << "1. Soldier (1 Turn)" << endl << "2. Redback (2 Turn)" << endl << "3. Tank (2 Turn)" << endl << "4. K9 (3 Turn)" << endl;
                            cout << "�Է� : ";
                            cin >> choice;

                            switch (choice) {
                            case 1:
                                command->requestSpawn("Soldier", { CursorX, CursorY }, board, playerName);
                                break;

                            case 2:
                                command->requestSpawn("RedBack", { CursorX, CursorY }, board, playerName);
                                break;

                            case 3:
                                command->requestSpawn("Tank", { CursorX, CursorY }, board, playerName);
                                break;

                            case 4:
                                command->requestSpawn("K9", { CursorX, CursorY }, board, playerName);
                                break;

                            default:
                                cout << "�߸��� �����Դϴ�" << endl;
                                break;

                            }
                        }
                        else
                        {
                            cout << "�̹� ���� ��� ���� ��ü�� �ֽ��ϴ�" << endl;;
                        }
                    }
                    else
                    {
                        cout << "������ �� �ִ� �ִ� ���� ������ 10���Դϴ�" << endl;
                    }
                }
                else
                {
                    cout << "�ش� ��ġ�� �̹� ��ü�� �����մϴ�" << endl;;
                }
            }
            else
            {
                cout << "���� ������ ��ġ�� �ƴմϴ�" << endl;
            }

        } // END else if(spawnMode)


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
            return true;
        }
    }
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
