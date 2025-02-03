#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <windows.h>
#include <cmath>

#define BOARD_WIDTH 11
#define BOARD_HEIGHT 12

class Object {
protected:
    struct Position {
        int x;
        int y;
    } position;
    
    std::string owner;
    int damage;
    int defense;
    float health_points;
    int move_distance;
    std::string unit_type;
    bool has_acted;  // 턴 시스템을 위한 새로운 변수

public:
    Object(std::string owner, Position pos, int dmg, int def, float hp, int move_dist, std::string type)
        : owner(owner), damage(dmg), defense(def), health_points(hp), 
          move_distance(move_dist), unit_type(type), has_acted(false) {
        position = pos;
    }

    virtual ~Object() = default;

    bool canMove(int target_x, int target_y, const std::vector<std::vector<Object*>>& board) const {
        int distance = abs(target_x - position.x) + abs(target_y - position.y);
        if (distance <= move_distance && target_x >= 0 && target_x < board[0].size() 
            && target_y >= 0 && target_y < board.size()) {
            return board[target_y][target_x] == nullptr;
        }
        return false;
    }

    bool isAlive() const { return health_points > 0; }
    void takeDamage(int dmg) { health_points -= std::max(0.0f, static_cast<float>(dmg - defense)); }
    Position getPosition() const { return position; }
    std::string getOwner() const { return owner; }
    std::string getUnitType() const { return unit_type; }
    int getMoveDistance() const { return move_distance; }
    bool hasActed() const { return has_acted; }
    void setHasActed(bool acted) { has_acted = acted; }
    void setPosition(Position pos) { position = pos; }
    float getHealth() const { return health_points; }
};

class AttackingObject : public Object {
protected:
    int attack_range;

public:
    AttackingObject(std::string owner, Position pos, int dmg, int def, float hp, 
                   int move_dist, int range, std::string type)
        : Object(owner, pos, dmg, def, hp, move_dist, type), attack_range(range) {}

    bool canAttack(const Object& target) const {
        if (owner == target.getOwner()) return false;
        
        Position target_pos = target.getPosition();
        int distance = abs(target_pos.x - position.x) + abs(target_pos.y - position.y);
        return distance <= attack_range;
    }

    void attack(Object& target) {
        if (canAttack(target)) {
            target.takeDamage(damage);
            has_acted = true;
        }
    }

    int getAttackRange() const { return attack_range; }
};

class CommandCenter : public Object {
private:
    struct SpawnInfo {
        std::string type;
        int turns_required;
        int damage;
        int defense;
        float health;
        int move_dist;
        int attack_range;
    };

    std::vector<SpawnInfo> available_units = {
        {"Soldier", 1, 20, 10, 80.0f, 5, 1},
        {"Redback", 2, 30, 15, 100.0f, 4, 2},
        {"Tank", 2, 50, 30, 200.0f, 2, 3},
        {"K9", 3, 40, 20, 150.0f, 3, 4}
    };

public:
    CommandCenter(std::string owner, Position pos)
        : Object(owner, pos, 0, 40, 300.0f, 0, "CommandCenter") {}

    bool canSpawn(const Position& spawn_pos, const std::vector<std::vector<Object*>>& board) const {
        return (spawn_pos.y == 0 || spawn_pos.y == board.size() - 1) 
               && board[spawn_pos.y][spawn_pos.x] == nullptr;
    }

    AttackingObject* spawn(const std::string& unit_type, Position spawn_pos) {
        for (const auto& unit : available_units) {
            if (unit.type == unit_type) {
                return new AttackingObject(owner, spawn_pos, unit.damage, unit.defense,
                    unit.health, unit.move_dist, unit.attack_range, unit_type);
            }
        }
        return nullptr;
    }
};

class Game {
private:
    int cursor_x, cursor_y;
    int selected_x, selected_y;
    bool is_selected;
    bool attack_mode, move_mode, spawn_mode;
    bool player1_turn;
    std::vector<std::vector<Object*>> board;

    void drawBoard() {
        system("cls");
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

        for (int i = 0; i < BOARD_HEIGHT; i++) {
            for (int j = 0; j < BOARD_WIDTH; j++) {
                SetConsoleTextAttribute(console, 7); // Default color

                // 선택된 유닛의 행동 범위 표시
                if (is_selected && board[selected_y][selected_x] != nullptr) {
                    if (attack_mode) {
                        AttackingObject* attacker = dynamic_cast<AttackingObject*>(board[selected_y][selected_x]);
                        if (attacker && abs(i - selected_y) + abs(j - selected_x) <= attacker->getAttackRange()) {
                            SetConsoleTextAttribute(console, BACKGROUND_RED);
                        }
                    }
                    else if (move_mode && !board[selected_y][selected_x]->hasActed()) {
                        if (abs(i - selected_y) + abs(j - selected_x) <= board[selected_y][selected_x]->getMoveDistance()
                            && board[i][j] == nullptr) {
                            SetConsoleTextAttribute(console, BACKGROUND_GREEN);
                        }
                    }
                    else if (spawn_mode) {
                        CommandCenter* cc = dynamic_cast<CommandCenter*>(board[selected_y][selected_x]);
                        if (cc && cc->canSpawn({j, i}, board)) {
                            SetConsoleTextAttribute(console, BACKGROUND_RED | BACKGROUND_BLUE);
                        }
                    }
                }

                // 선택된 유닛 하이라이트
                if (is_selected && i == selected_y && j == selected_x) {
                    SetConsoleTextAttribute(console, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
                }

                // 커서 위치 표시
                if (i == cursor_y && j == cursor_x) {
                    SetConsoleTextAttribute(console, 
                        player1_turn ? BACKGROUND_BLUE | BACKGROUND_INTENSITY 
                                   : BACKGROUND_RED | BACKGROUND_INTENSITY);
                }

                // 유닛 또는 빈 칸 표시
                if (board[i][j] != nullptr) {
                    Object* obj = board[i][j];
                    std::cout << obj->getUnitType().substr(0, 2) << " ";
                    
                    // 체력이 낮은 유닛 표시
                    if (obj->getHealth() < 50.0f) {
                        SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
                    }
                } else {
                    std::cout << ". ";
                }
            }
            std::cout << std::endl;
        }

        // 게임 상태 정보 표시
        std::cout << "\n현재 턴: " << (player1_turn ? "Player 1" : "Player 2") << std::endl;
        std::cout << "모드: " << (attack_mode ? "공격" : move_mode ? "이동" : spawn_mode ? "생성" : "대기") << std::endl;
    }

    void handleInput() {
        if (_kbhit()) {
            int key = _getch();
            
            switch (key) {
                case 72: if (cursor_y > 0) cursor_y--; break;                // Up
                case 80: if (cursor_y < BOARD_HEIGHT - 1) cursor_y++; break; // Down
                case 75: if (cursor_x > 0) cursor_x--; break;                // Left
                case 77: if (cursor_x < BOARD_WIDTH - 1) cursor_x++; break;  // Right
                
                case 'a': case 'A': // Attack mode
                    if (board[cursor_y][cursor_x] != nullptr && 
                        dynamic_cast<AttackingObject*>(board[cursor_y][cursor_x])) {
                        selected_x = cursor_x;
                        selected_y = cursor_y;
                        is_selected = true;
                        attack_mode = !attack_mode;
                        move_mode = spawn_mode = false;
                    }
                    break;

                case 'v': case 'V': // Move mode
                    if (board[cursor_y][cursor_x] != nullptr && 
                        !board[cursor_y][cursor_x]->hasActed()) {
                        selected_x = cursor_x;
                        selected_y = cursor_y;
                        is_selected = true;
                        move_mode = !move_mode;
                        attack_mode = spawn_mode = false;
                    }
                    break;

                case 's': case 'S': // Spawn mode
                    if (board[cursor_y][cursor_x] != nullptr && 
                        dynamic_cast<CommandCenter*>(board[cursor_y][cursor_x])) {
                        selected_x = cursor_x;
                        selected_y = cursor_y;
                        is_selected = true;
                        spawn_mode = !spawn_mode;
                        attack_mode = move_mode = false;
                    }
                    break;

                case 't': case 'T': // End turn
                    endTurn();
                    break;

                case 32: // Space - confirm action
                    handleAction();
                    break;
            }
        }
    }

    void handleAction() {
        if (!is_selected) {
            if (board[cursor_y][cursor_x] != nullptr) {
                selected_x = cursor_x;
                selected_y = cursor_y;
                is_selected = true;
            }
            return;
        }

        if (attack_mode) {
            AttackingObject* attacker = dynamic_cast<AttackingObject*>(board[selected_y][selected_x]);
            if (attacker && board[cursor_y][cursor_x] != nullptr) {
                attacker->attack(*board[cursor_y][cursor_x]);
                if (board[cursor_y][cursor_x]->getHealth() <= 0) {
                    delete board[cursor_y][cursor_x];
                    board[cursor_y][cursor_x] = nullptr;
                }
            }
        }
        else if (move_mode) {
            Object* unit = board[selected_y][selected_x];
            if (unit && unit->canMove(cursor_x, cursor_y, board)) {
                board[cursor_y][cursor_x] = unit;
                board[selected_y][selected_x] = nullptr;
                unit->setPosition({cursor_x, cursor_y});
                unit->setHasActed(true);
            }
        }
        else if (spawn_mode) {
            CommandCenter* cc = dynamic_cast<CommandCenter*>(board[selected_y][selected_x]);
            if (cc && cc->canSpawn({cursor_x, cursor_y}, board)) {
                std::cout << "\n생성할 유닛을 선택하세요:\n";
                std::cout << "1. Soldier (1 Turn)\n2. Redback (2 Turn)\n";
                std::cout << "3. Tank (2 Turn)\n4. K9 (3 Turn)\n선택: ";
                
                int choice;
                std::cin >> choice;
                
                std::string unit_type;
                switch (choice) {
                    case 1: unit_type = "Soldier"; break;
                    case 2: unit_type = "Redback"; break;
                    case 3: unit_type = "Tank"; break;
                    case 4: unit_type = "K9"; break;
                    default: unit_type = "Soldier";
                }
                
                board[cursor_y][cursor_x] = cc->spawn(unit_type, {cursor_x, cursor_y});
            }
        }

        is_selected = false;
        attack_mode = move_mode = spawn_mode = false;
    }

    void endTurn() {
        player1_turn = !player1_turn;
        // Reset all units' action flags
        for (auto& row : board) {
            for (auto& unit : row) {
                if (unit) unit->setHasActed(false);
            }
        }
    }

public:
    Game() : cursor_x(0), cursor_y(0), selected_x(0), selected_y(0),
             is_selected(false), attack_mode(false), move_mode(false), 
             spawn_mode(false), player1_turn(true) {
        // Initialize board
        board.resize(BOARD_HEIGHT, std::vector<Object*>(BOARD_WIDTH, nullptr));
        
        // Place command centers
        board[0][5] = new CommandCenter("Player1", {5, 0});
        board[BOARD_HEIGHT-1][5] = new CommandCenter("Player2", {5, BOARD_HEIGHT-1});
        
        printInstructions();
    }

    void printInstructions() {
        std::cout << "=== 전략 게임 조작 방법 ===\n";
        std::cout << "방향키: 커서 이동\n";
        std::cout << "A: 공격 모드\n";
        std::cout << "V: 이동 모드\n";
        std::cout << "S: 유닛 생성 모드\n";
        std::cout << "T: 턴 종료\n";
        std::cout << "스페이스바: 선택/행동\n\n";
        std::cout << "게임을 시작하
