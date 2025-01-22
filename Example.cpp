// 기본 유닛 클래스
class Unit {
public:
    int x, y;
    int hp;
    string type;
    
    virtual void showInfo() = 0;  // 순수 가상 함수
};

// 구체적인 유닛 클래스들
class Tank : public Unit {
public:
    int armor;
    
    Tank(int _x, int _y) {
        x = _x;
        y = _y;
        hp = 150;
        armor = 30;
        type = "Tank";
    }
    
    void showInfo() override {
        cout << "Tank 정보:" << endl;
        cout << "HP: " << hp << endl;
        cout << "Armor: " << armor << endl;
    }
};

class Redback : public Unit {
public:
    int attackPower;
    
    Redback(int _x, int _y) {
        x = _x;
        y = _y;
        hp = 100;
        attackPower = 40;
        type = "Redback";
    }
    
    void showInfo() override {
        cout << "Redback 정보:" << endl;
        cout << "HP: " << hp << endl;
        cout << "Attack: " << attackPower << endl;
    }
};

class GameBoard {
private:
    vector<vector<Unit*>> board;
    
public:
    void selectUnit(int x, int y) {
        Unit* selected = board[y][x];
        if(selected) {
            // dynamic_cast로 구체적인 유닛 타입 확인
            if(Tank* tank = dynamic_cast<Tank*>(selected)) {
                cout << "탱크가 선택됨!" << endl;
                tank->showInfo();
            }
            else if(Redback* redback = dynamic_cast<Redback*>(selected)) {
                cout << "레드백이 선택됨!" << endl;
                redback->showInfo();
            }
        }
    }
};
