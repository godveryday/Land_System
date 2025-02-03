// game_classes.h
#ifndef GAME_CLASSES_H
#define GAME_CLASSES_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <ctime>
#include <cstdlib>

#define BOARD_WIDTH 11
#define BOARD_HEIGHT 12

enum Object_state { before_action, after_attack, after_move };

struct Position {
    int x;
    int y;
};

// 기본 Object 클래스
class Object {
protected:
    std::string owner;
    Position position;
    int damage;
    int defense;
    float health_points;
    float max_health_points;
    int move_distance;
    Object_state state;
    std::string unit_type;
    bool has_acted_this_turn;
    std::string target_type;

public:
    Object(std::string owner, Position pos, int dmg, int def, float hp, int move_dist, std::string unit_type);
    virtual ~Object() = default;

    virtual void move(int target_x, int target_y, const std::vector<std::vector<Object*>>& board);
    bool can_move(int target_x, int target_y, const std::vector<std::vector<Object*>>& board) const;
    std::string Object_Info() const;
    std::vector<Position> get_movable_positions(const std::vector<std::vector<Object*>>& board) const;

    // Getters and Setters
    int getDefense() const;
    void setHealthPoints(float hp);
    float getHealthPoints() const;
    float getMaxHealthPoints() const;
    Position getPosition() const;
    std::string getName() const;
    std::string getUnitType() const;
    void setPosition(const Position& pos);
    int get_move_distance() const;
    bool hasActedThisTurn() const;
    void resetTurnAction();
    void setActedThisTurn();
    float getHealthPercentage() const;
};

// 공격 가능한 유닛 클래스
class Attack_Object_Type : public Object {
private:
    int attack_range;
    int calculateBonusDamage(const Object& target) const;

public:
    Attack_Object_Type(std::string owner, Position pos, int dmg, int def, float hp,
        int move_dist, int attack_range, std::string unit_type);
    void attack(Object& target);
    bool can_attack(const Object& target) const;
    std::vector<Position> get_attackable_positions(const std::vector<std::vector<Object*>>& board) const;
    int getAttackRange() const;
};

// 지휘소 클래스
struct SpawnRequest {
    std::string unit_type;
    Position spawn_pos;
    std::string new_owner;
    int remaining_turns;
};

class Command_Object_Type : public Object {
private:
    std::unordered_map<std::string, int> unit_spawn_time = {
        {"Soldier", 1},
        {"RedBack", 2},
        {"Tank", 2},
        {"K9", 3}
    };

public:
    Command_Object_Type(std::string owner, Position pos, int dmg, int def, float hp, int move_dist);
    void request_spawn(const std::string& unit_type, const Position& spawn_pos,
        const std::vector<std::vector<Object*>>& board, const std::string& new_owner);
    bool can_spawn(const Position& spawn_pos, const std::vector<std::vector<Object*>>& board) const;
    void update_spawn_requests(std::vector<std::vector<Object*>>& board);
    Object* spawn(const std::string& unit_type, int y, int x);
    std::vector<SpawnRequest> spawn_requests;
    int getSpawnTime(const std::string& unit_type) const {
        auto it = unit_spawn_time.find(unit_type);
        return it != unit_spawn_time.end() ? it->second : 0;
    }
};

// Game 클래스
class Game {
private:
    HANDLE console;
    int CursorX, CursorY;
    int Selected_X, Selected_Y;
    bool Is_Selected;
    bool Attack_Time, Move_Time, Spawn_Time;
    bool player_turn;
    bool hasAvailableActions() const;
    int turn_count;
    std::vector<std::vector<Object*>> board;
    std::vector<SpawnRequest> pending_spawns;
    const int MAX_UNITS_PER_PLAYER = 10;

    int countPlayerUnits(const std::string& player);
    void handleSpaceKey();
    bool can_attack_position(int from_y, int from_x, int to_y, int to_x);
    bool can_move_to_position(int from_y, int from_x, int to_y, int to_x);
    void updatePendingSpawns();
    void removeDeadUnits();
    void PrintInstructions();

public:
    Game();
    ~Game();
    void Draw_Board();
    void Check_KeyInput();
    void Run();
    void nextTurn();
};

#endif




// game_classes.cpp
#include "game_classes.h"
#include <sstream>
#include <cmath>

// Object 클래스 구현
Object::Object(std::string owner, Position pos, int dmg, int def, float hp, int move_dist, std::string unit_type)
    : owner(owner), position(pos), damage(dmg), defense(def), health_points(hp),
    max_health_points(hp), move_distance(move_dist), state(before_action),
    unit_type(unit_type), has_acted_this_turn(false) {
    if (unit_type == "Soldier") target_type = "K9";
    else if (unit_type == "K9") target_type = "Tank";
    else if (unit_type == "Tank") target_type = "RedBack";
    else if (unit_type == "RedBack") target_type = "Soldier";
}

void Object::move(int target_x, int target_y, const std::vector<std::vector<Object*>>& board) {
    if (can_move(target_x, target_y, board) && !has_acted_this_turn) {
        position = { target_x, target_y };
        state = after_move;
        has_acted_this_turn = true;
        std::cout << getUnitType() << " moved to position (" << target_x << ", " << target_y << ")\n";
    }
    else {
        std::cout << "Cannot move to the specified position!\n";
    }
}

bool Object::can_move(int target_x, int target_y, const std::vector<std::vector<Object*>>& board) const {
    if (target_x < 0 || target_x >= board[0].size() || target_y < 0 || target_y >= board.size()) {
        return false;
    }

    int distance = std::abs(target_x - position.x) + std::abs(target_y - position.y);
    if (distance > move_distance) {
        return false;
    }

    // 자주포 이동 제한 검사
    if (unit_type == "K9") {
        bool near_command = false;
        for (int i = 0; i < board.size(); i++) {
            for (int j = 0; j < board[0].size(); j++) {
                if (board[i][j] && board[i][j]->getUnitType() == "Command" &&
                    board[i][j]->getName() == owner) {
                    int dist_to_command = std::abs(target_x - j) + std::abs(target_y - i);
                    if (dist_to_command <= 3) {
                        near_command = true;
                    }
                }
            }
        }
        if (!near_command) return false;
    }

    return board[target_y][target_x] == nullptr;
}

// Object의 Getter/Setter 구현
int Object::getDefense() const { return defense; }
void Object::setHealthPoints(float hp) { health_points = hp; }
float Object::getHealthPoints() const { return health_points; }
float Object::getMaxHealthPoints() const { return max_health_points; }
Position Object::getPosition() const { return position; }
std::string Object::getName() const { return owner; }
std::string Object::getUnitType() const { return unit_type; }
void Object::setPosition(const Position& pos) { position = pos; }
int Object::get_move_distance() const { return move_distance; }
bool Object::hasActedThisTurn() const { return has_acted_this_turn; }
void Object::resetTurnAction() { has_acted_this_turn = false; }
void Object::setActedThisTurn() { has_acted_this_turn = true; }
float Object::getHealthPercentage() const { return (health_points / max_health_points) * 100.0f; }

// Attack_Object_Type 클래스 구현
Attack_Object_Type::Attack_Object_Type(std::string owner, Position pos, int dmg, int def, float hp,
    int move_dist, int attack_range, std::string unit_type)
    : Object(owner, pos, dmg, def, hp, move_dist, unit_type), attack_range(attack_range) {
}

int Attack_Object_Type::calculateBonusDamage(const Object& target) const {
    if (target.getUnitType() == target_type) {
        return damage * 0.5; // 50% 추가 데미지
    }
    return 0;
}

void Attack_Object_Type::attack(Object& target) {
    if (can_attack(target) && !has_acted_this_turn) {
        int bonus_damage = calculateBonusDamage(target);
        int actual_damage = (damage + bonus_damage) - target.getDefense();
        float new_health = target.getHealthPoints() - actual_damage;
        target.setHealthPoints(new_health);

        state = after_attack;
        has_acted_this_turn = true;

        std::cout << owner << "의 " << getUnitType() << "이(가) "
            << target.getUnitType() << "을(를) 공격! "
            << actual_damage << " 데미지";

        if (bonus_damage > 0) {
            std::cout << " (상성 보너스: " << bonus_damage << ")";
        }
        std::cout << "\n대상 남은 체력: " << new_health << "/"
            << target.getMaxHealthPoints() << " ("
            << target.getHealthPercentage() << "%)\n";
    }
    else {
        std::cout << "공격할 수 없습니다!";
        if (has_acted_this_turn) {
            std::cout << " 이미 이 턴에 행동했습니다.";
        }
        else {
            std::cout << " 대상이 사정거리 밖에 있습니다.";
        }
        std::cout << std::endl;
    }
}

bool Attack_Object_Type::can_attack(const Object& target) const {
    if (owner == target.getName() || has_acted_this_turn) {
        return false;
    }

    // 보병만 지휘소 공격 가능
    if (target.getUnitType() == "Command" && getUnitType() != "Soldier") {
        return false;
    }

    Position target_position = target.getPosition();
    int distance = std::abs(target_position.x - position.x) +
        std::abs(target_position.y - position.y);

    // 자주포는 2칸 이상, 4칸 이하만 공격 가능
    if (getUnitType() == "K9") {
        return distance >= 2 && distance <= 4;
    }

    return distance <= attack_range;
}

int Attack_Object_Type::getAttackRange() const {
    return attack_range;
}

// Command_Object_Type 클래스 구현
Command_Object_Type::Command_Object_Type(std::string owner, Position pos, int dmg, int def, float hp, int move_dist)
    : Object(owner, pos, dmg, def, hp, move_dist, "Command") {
}

bool Command_Object_Type::can_spawn(const Position& spawn_pos, const std::vector<std::vector<Object*>>& board) const {
    return (spawn_pos.y == 0 || spawn_pos.y == board.size() - 1) &&
        board[spawn_pos.y][spawn_pos.x] == nullptr;
}

void Command_Object_Type::request_spawn(const std::string& unit_type, const Position& spawn_pos,
    const std::vector<std::vector<Object*>>& board,
    const std::string& new_owner) {
    if (can_spawn(spawn_pos, board)) {
        int spawn_time = unit_spawn_time[unit_type];
        spawn_requests.push_back({ unit_type, spawn_pos, new_owner, spawn_time });
    }
    else {
        std::cout << "Cannot spawn at the specified position!\n";
    }
}

Object* Command_Object_Type::spawn(const std::string& unit_type, int y, int x) {
    Position pos = { x, y };
    if (unit_type == "Soldier") {
        return new Attack_Object_Type(owner, pos, 100, 10, 150, 1, 1, "Soldier");
    }
    else if (unit_type == "RedBack") {
        return new Attack_Object_Type(owner, pos, 130, 20, 200, 2, 2, "RedBack");
    }
    else if (unit_type == "Tank") {
        return new Attack_Object_Type(owner, pos, 150, 15, 250, 2, 2, "Tank");
    }
    else if (unit_type == "K9") {
        return new Attack_Object_Type(owner, pos, 170, 25, 280, 2, 4, "K9");
    }
    return nullptr;
}

// Game 클래스 구현
Game::Game() : CursorX(0), CursorY(0), Selected_X(0), Selected_Y(0),
Is_Selected(false), Attack_Time(false), Move_Time(false), Spawn_Time(false),
turn_count(1)
{
    console = GetStdHandle(STD_OUTPUT_HANDLE);  // console 초기화
    // 보드 초기화
    board.resize(BOARD_HEIGHT, std::vector<Object*>(BOARD_WIDTH, nullptr));

    // 동전 던지기로 시작 플레이어 결정
    srand(time(nullptr));
    player_turn = rand() % 2 == 0;

    // 지휘소 초기 배치 (양 끝 행의 가운데에 위치)
    board[0][3] = new Command_Object_Type("Player1", { 3, 0 }, 0, 50, 250, 1);
    board[11][3] = new Command_Object_Type("Player2", { 3, 11 }, 0, 50, 250, 1);

    PrintInstructions();
}

Game::~Game() {
    // 동적 할당된 객체들 해제
    for (auto& row : board) {
        for (auto& obj : row) {
            delete obj;
        }
    }
}

void Game::PrintInstructions() {
    std::cout << "=== 게임 조작 방법 ===" << std::endl;
    std::cout << "방향키: 커서 이동" << std::endl;
    std::cout << "A 키: 공격 모드 전환" << std::endl;
    std::cout << "V 키: 이동 모드 전환" << std::endl;
    std::cout << "S 키: 유닛 생성 모드 전환" << std::endl;
    std::cout << "T 키: 턴 종료" << std::endl;
    std::cout << "스페이스바: 선택/행동 실행" << std::endl;
    std::cout << "\n게임을 시작하려면 아무 키나 누르세요..." << std::endl;
    _getch();
    system("cls");
}

void Game::Draw_Board() {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD cursorPos = { 0, 0 };
    SetConsoleCursorPosition(console, cursorPos);

    // 게임 상태 표시
    std::cout << "===== Turn " << turn_count << " =====" << std::endl;
    std::cout << "현재 플레이어: " << (player_turn ? "Player 1" : "Player 2") << "\n\n";

    // 보드 그리기
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            SetConsoleTextAttribute(console, 7); // 기본 색상 설정

            // 공격 범위 표시
            if (Attack_Time && board[Selected_Y][Selected_X] != nullptr) {
                Attack_Object_Type* attacker = dynamic_cast<Attack_Object_Type*>(board[Selected_Y][Selected_X]);
                if (attacker) {
                    int distance = std::abs(i - Selected_Y) + std::abs(j - Selected_X);
                    if (attacker->getUnitType() == "K9") {
                        if (distance >= 2 && distance <= 4) {
                            SetConsoleTextAttribute(console, BACKGROUND_RED);
                        }
                    }
                    else if (distance <= attacker->getAttackRange()) {
                        SetConsoleTextAttribute(console, BACKGROUND_RED);
                    }
                }
            }

            // 이동 가능 범위 표시
            if (Move_Time && board[Selected_Y][Selected_X] != nullptr) {
                int distance = std::abs(i - Selected_Y) + std::abs(j - Selected_X);
                if (distance <= board[Selected_Y][Selected_X]->get_move_distance() &&
                    board[i][j] == nullptr) {
                    // 자주포 이동 제한 확인
                    bool can_move = true;
                    if (board[Selected_Y][Selected_X]->getUnitType() == "K9") {
                        bool near_command = false;
                        for (int y = 0; y < BOARD_HEIGHT; y++) {
                            for (int x = 0; x < BOARD_WIDTH; x++) {
                                if (board[y][x] && board[y][x]->getUnitType() == "Command" &&
                                    board[y][x]->getName() == board[Selected_Y][Selected_X]->getName()) {
                                    int dist_to_command = std::abs(j - x) + std::abs(i - y);
                                    if (dist_to_command <= 3) {
                                        near_command = true;
                                        break;
                                    }
                                }
                            }
                        }
                        can_move = near_command;
                    }
                    if (can_move) {
                        SetConsoleTextAttribute(console, BACKGROUND_GREEN);
                    }
                }
            }

            // 생성 가능 위치 표시
            if (Spawn_Time && board[Selected_Y][Selected_X] != nullptr) {
                Command_Object_Type* cc = dynamic_cast<Command_Object_Type*>(board[Selected_Y][Selected_X]);
                if (cc && cc->can_spawn({ j, i }, board)) {
                    SetConsoleTextAttribute(console, BACKGROUND_RED | BACKGROUND_BLUE);
                }
            }

            // 선택된 유닛 하이라이트
            if (Is_Selected && i == Selected_Y && j == Selected_X) {
                SetConsoleTextAttribute(console, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
            }

            // 커서 위치 표시
            if (i == CursorY && j == CursorX) {
                SetConsoleTextAttribute(console,
                    player_turn ? BACKGROUND_BLUE | BACKGROUND_INTENSITY
                    : BACKGROUND_RED | BACKGROUND_INTENSITY);
            }

            // 유닛 표시
            if (board[i][j] != nullptr) {
                Object* obj = board[i][j];
                std::string display = obj->getUnitType().substr(0, 1);

                // 체력에 따른 색상 변경
                float health_percent = obj->getHealthPercentage();
                if (health_percent <= 30) {
                    SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
                }

                // 행동 완료한 유닛 표시
                if (obj->hasActedThisTurn()) {
                    SetConsoleTextAttribute(console, FOREGROUND_INTENSITY);
                }

                std::cout << display << " ";
            }
            else {
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }

    // 게임 상태 정보 표시
    std::cout << "\n현재 모드: " << (Attack_Time ? "공격"
        : Move_Time ? "이동"
        : Spawn_Time ? "생성" : "대기") << "\n";

    // 유닛 수 표시
    int p1_units = countPlayerUnits("Player1");
    int p2_units = countPlayerUnits("Player2");
    std::cout << "유닛 수 - Player 1: " << p1_units << "/10, Player 2: "
        << p2_units << "/10\n";

    // 생성 대기 중인 유닛 표시
    if (!pending_spawns.empty()) {
        std::cout << "\n생성 대기 중인 유닛:\n";
        for (const auto& spawn : pending_spawns) {
            std::cout << spawn.unit_type << " (" << spawn.spawn_pos.x << ","
                << spawn.spawn_pos.y << ") - " << spawn.remaining_turns
                << "턴 후 생성\n";
        }
    }
}

void Game::Check_KeyInput() {
    if (_kbhit()) {
        int key = _getch();

        switch (key) {
        case 72: // 위
            if (CursorY > 0) CursorY--;
            break;
        case 80: // 아래
            if (CursorY < BOARD_HEIGHT - 1) CursorY++;
            break;
        case 75: // 왼쪽
            if (CursorX > 0) CursorX--;
            break;
        case 77: // 오른쪽
            if (CursorX < BOARD_WIDTH - 1) CursorX++;
            break;
        case 'a':
        case 'A':
            if (board[CursorY][CursorX] != nullptr &&
                board[CursorY][CursorX]->getName() == (player_turn ? "Player1" : "Player2")) {
                if (dynamic_cast<Attack_Object_Type*>(board[CursorY][CursorX])) {
                    Selected_X = CursorX;
                    Selected_Y = CursorY;
                    Is_Selected = true;
                    Attack_Time = !Attack_Time;
                    Move_Time = Spawn_Time = false;
                }
            }
            break;
        case 'v':
        case 'V':
            if (board[CursorY][CursorX] != nullptr &&
                board[CursorY][CursorX]->getName() == (player_turn ? "Player1" : "Player2")) {
                Selected_X = CursorX;
                Selected_Y = CursorY;
                Is_Selected = true;
                Move_Time = !Move_Time;
                Attack_Time = Spawn_Time = false;
            }
            break;
        case 's':
        case 'S':
            if (board[CursorY][CursorX] != nullptr &&
                board[CursorY][CursorX]->getName() == (player_turn ? "Player1" : "Player2")) {
                if (dynamic_cast<Command_Object_Type*>(board[CursorY][CursorX])) {
                    Selected_X = CursorX;
                    Selected_Y = CursorY;
                    Is_Selected = true;
                    Spawn_Time = !Spawn_Time;
                    Attack_Time = Move_Time = false;
                }
            }
            break;
        case 't':
        case 'T':
            nextTurn();
            break;
        case 32: // 스페이스바
            handleSpaceKey();
            break;
        }


    }

    if (!hasAvailableActions()) {
        std::cout << "\n더 이상 행동할 수 있는 유닛이 없습니다. 턴을 넘깁니다...\n";
        Sleep(1500);
        nextTurn();
    }
}

void Game::handleSpaceKey() {
    if (!Is_Selected) {
        if (board[CursorY][CursorX] != nullptr &&
            board[CursorY][CursorX]->getName() == (player_turn ? "Player1" : "Player2")) {
            Selected_X = CursorX;
            Selected_Y = CursorY;
            Is_Selected = true;
        }
    }
    else {
        if (Attack_Time) {
            Attack_Object_Type* attacker = dynamic_cast<Attack_Object_Type*>(board[Selected_Y][Selected_X]);
            if (attacker && board[CursorY][CursorX] != nullptr) {
                attacker->attack(*board[CursorY][CursorX]);
                if (board[CursorY][CursorX]->getHealthPoints() <= 0) {
                    removeDeadUnits();
                }
            }
        }
        else if (Move_Time) {
            if (board[Selected_Y][Selected_X] &&
                can_move_to_position(Selected_Y, Selected_X, CursorY, CursorX)) {
                board[CursorY][CursorX] = board[Selected_Y][Selected_X];
                board[Selected_Y][Selected_X] = nullptr;
                board[CursorY][CursorX]->setPosition({ CursorX, CursorY });
                board[CursorY][CursorX]->setActedThisTurn();
            }
        }
        else if (Spawn_Time) {
            Command_Object_Type* cc = dynamic_cast<Command_Object_Type*>(board[Selected_Y][Selected_X]);
            if (cc && cc->can_spawn({ CursorX, CursorY }, board)) {
                if (countPlayerUnits(player_turn ? "Player1" : "Player2") < MAX_UNITS_PER_PLAYER) {
                    SetConsoleTextAttribute(console, 7);
                    std::cout << "\n생성할 유닛을 선택하세요:\n";
                    std::cout << "1. Soldier (1턴) - 공격력:100 방어력:10 체력:150\n";
                    std::cout << "2. RedBack (2턴) - 공격력:130 방어력:20 체력:200\n";
                    std::cout << "3. Tank (2턴) - 공격력:150 방어력:15 체력:250\n";
                    std::cout << "4. K9 (3턴) - 공격력:170 방어력:25 체력:280\n";
                    std::cout << "선택 (1-4): ";

                    int choice;
                    std::cin >> choice;

                    std::string unit_type;
                    switch (choice) {
                    case 1: unit_type = "Soldier"; break;
                    case 2: unit_type = "RedBack"; break;
                    case 3: unit_type = "Tank"; break;
                    case 4: unit_type = "K9"; break;
                    default: unit_type = ""; break;
                    }

                    if (!unit_type.empty()) {
                        SpawnRequest req = {
                            unit_type,
                            {CursorX, CursorY},
                            player_turn ? "Player1" : "Player2",
                             cc->getSpawnTime(unit_type)
                        };
                        pending_spawns.push_back(req);
                    }
                }
                else {
                    std::cout << "최대 유닛 수(10개)에 도달했습니다!\n";
                    Sleep(1000);
                }
            }
        }

        Is_Selected = false;
        Attack_Time = Move_Time = Spawn_Time = false;
    }
}


int Game::countPlayerUnits(const std::string& player) {
    int count = 0;
    for (const auto& row : board) {
        for (const auto& obj : row) {
            if (obj && obj->getName() == player) {
                count++;
            }
        }
    }
    return count;
}

bool Game::hasAvailableActions() const {
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (board[i][j] &&
                board[i][j]->getName() == (player_turn ? "Player1" : "Player2") &&
                !board[i][j]->hasActedThisTurn()) {
                return true;
            }
        }
    }
    return false;
}

void Game::updatePendingSpawns() {
    for (auto it = pending_spawns.begin(); it != pending_spawns.end();) {
        it->remaining_turns--;

        if (it->remaining_turns <= 0) {
            if (countPlayerUnits(it->new_owner) < MAX_UNITS_PER_PLAYER) {
                if (board[it->spawn_pos.y][it->spawn_pos.x] == nullptr) {
                    Command_Object_Type* command = dynamic_cast<Command_Object_Type*>(
                        board[it->spawn_pos.y][it->spawn_pos.x]);

                    // 새로운 유닛 생성
                    Object* new_unit = nullptr;
                    if (it->unit_type == "Soldier") {
                        new_unit = new Attack_Object_Type(it->new_owner, it->spawn_pos, 100, 10, 150, 1, 1, "Soldier");
                    }
                    else if (it->unit_type == "RedBack") {
                        new_unit = new Attack_Object_Type(it->new_owner, it->spawn_pos, 130, 20, 200, 2, 2, "RedBack");
                    }
                    else if (it->unit_type == "Tank") {
                        new_unit = new Attack_Object_Type(it->new_owner, it->spawn_pos, 150, 15, 250, 2, 2, "Tank");
                    }
                    else if (it->unit_type == "K9") {
                        new_unit = new Attack_Object_Type(it->new_owner, it->spawn_pos, 170, 25, 280, 2, 4, "K9");
                    }

                    if (new_unit) {
                        board[it->spawn_pos.y][it->spawn_pos.x] = new_unit;
                        std::cout << it->new_owner << "의 " << it->unit_type << " 유닛이 생성되었습니다.\n";
                    }
                }
            }
            else {
                std::cout << "최대 유닛 수 초과로 " << it->unit_type << " 생성이 취소되었습니다.\n";
            }
            it = pending_spawns.erase(it);
        }
        else {
            ++it;
        }
    }
}

void Game::removeDeadUnits() {
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (board[i][j] && board[i][j]->getHealthPoints() <= 0) {
                // 지휘소가 파괴되면 게임 종료
                if (board[i][j]->getUnitType() == "Command") {
                    std::cout << "\n=== 게임 종료 ===\n";
                    std::cout << board[i][j]->getName() << "의 지휘소가 파괴되었습니다!\n";
                    std::cout << (board[i][j]->getName() == "Player1" ? "Player 2" : "Player 1")
                        << "의 승리!\n";
                    Sleep(3000);
                    exit(0);
                }

                // 일반 유닛 제거
                std::cout << board[i][j]->getName() << "의 "
                    << board[i][j]->getUnitType() << " 유닛이 파괴되었습니다.\n";
                delete board[i][j];
                board[i][j] = nullptr;
            }
        }
    }
}

bool Game::can_attack_position(int from_y, int from_x, int to_y, int to_x) {
    Attack_Object_Type* attacker = dynamic_cast<Attack_Object_Type*>(board[from_y][from_x]);
    if (!attacker) return false;

    int distance = std::abs(to_y - from_y) + std::abs(to_x - from_x);

    // 자주포는 2칸 이상, 4칸 이하만 공격 가능
    if (attacker->getUnitType() == "K9") {
        return distance >= 2 && distance <= 4;
    }

    return distance <= attacker->getAttackRange();
}

bool Game::can_move_to_position(int from_y, int from_x, int to_y, int to_x) {
    if (!board[from_y][from_x] || board[to_y][to_x] != nullptr) {
        return false;
    }

    int distance = std::abs(to_y - from_y) + std::abs(to_x - from_x);
    if (distance > board[from_y][from_x]->get_move_distance()) {
        return false;
    }

    // 자주포 이동 제한 확인
    if (board[from_y][from_x]->getUnitType() == "K9") {
        bool near_command = false;
        for (int i = 0; i < BOARD_HEIGHT; i++) {
            for (int j = 0; j < BOARD_WIDTH; j++) {
                if (board[i][j] && board[i][j]->getUnitType() == "Command" &&
                    board[i][j]->getName() == board[from_y][from_x]->getName()) {
                    int dist_to_command = std::abs(to_x - j) + std::abs(to_y - i);
                    if (dist_to_command <= 3) {
                        near_command = true;
                        break;
                    }
                }
            }
            if (near_command) break;
        }
        if (!near_command) return false;
    }

    return true;
}

void Game::Run() {
    while (true) {
        Draw_Board();
        Check_KeyInput();
        Sleep(100);  // CPU 사용량 감소를 위한 짧은 대기
    }
}

void Game::nextTurn() {
    player_turn = !player_turn;
    turn_count++;

    // 모든 유닛의 행동 초기화
    for (auto& row : board) {
        for (auto& obj : row) {
            if (obj) {
                obj->resetTurnAction();
            }
        }
    }

    // 대기 중인 유닛 생성 처리
    updatePendingSpawns();

    // 사망한 유닛 제거
    removeDeadUnits();

    std::cout << "\n=== Turn " << turn_count << " ===\n";
    std::cout << (player_turn ? "Player 1" : "Player 2") << "의 턴입니다.\n";
    Sleep(1000);
}
