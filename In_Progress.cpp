// 1. Turn 개념 추가하기
// 2. 지금은 Object의 Position x y 좌표를 활용하지 않는 것 같음 --> 수정해야함


#include<iostream>
#include<windows.h>
#include<vector>
#include<string>
#include<conio.h>
#include<math.h>
#include"Draw_Title.h"
using namespace std;

#define BOARD_HEIGHT 12
#define BOARD_WIDTH 7

class Object {
protected:
   struct Position {
       int x;
       int y;
   }position;
   
   int damage;
   int defense;
   int hp;
   int move_distance;
   string name;

public:
   Object(int x, int y, int dmg, int def, int health, int move, string n) {
       position.x = x;
       position.y = y;
       damage = dmg;
       defense = def;
       hp = health;
       move_distance = move;
       name = n;
   }

   virtual ~Object() {}
   
   Position getPosition() { return position; }
   string getName() { return name; }
   bool isAlive() { return hp > 0; }
   int getMoveDistance() { return move_distance; }

   void takeDamage(int dmg) {
       hp -= max(0, dmg - defense);
   }
};

class Attacking_Object : public Object {
protected:
   int attack_range;

public:
   Attacking_Object(int x, int y, int dmg, int def, int hp, int move, int range, string name)
       : Object(x, y, dmg, def, hp, move, name), attack_range(range) {}
   
   virtual void move(int new_x, int new_y) {
       position.x = new_x;
       position.y = new_y;
   }
   
   virtual void attack(Object* target) {
       if(target) target->takeDamage(damage);
   }

   int getAttackRange() { return attack_range; }
};

class Command_Object : public Object {
protected:
   int spawn_range;

public:
   Command_Object(int x, int y, int def, int hp, int move, int range, string name)
       : Object(x, y, 0, def, hp, move, name), spawn_range(range) {}
   
   virtual void move(int new_x, int new_y) {
       position.x = new_x;
       position.y = new_y;
   }
   
   virtual Object* spawn(string unit_type, int x, int y) = 0;
   int getSpawnRange() { return spawn_range; }
};

class Tank : public Attacking_Object {
public:
   Tank(int x, int y) : Attacking_Object(x, y, 50, 30, 200, 2, 3, "T") {}
};

class K9 : public Attacking_Object {
public:
   K9(int x, int y) : Attacking_Object(x, y, 40, 20, 150, 3, 4, "K") {}
};

class Redback : public Attacking_Object {
public:
   Redback(int x, int y) : Attacking_Object(x, y, 30, 15, 100, 4, 2, "R") {}
};

class Soldier : public Attacking_Object {
public:
   Soldier(int x, int y) : Attacking_Object(x, y, 20, 10, 80, 5, 1, "S") {}
};

class CommandCenter : public Command_Object {
public:
   CommandCenter(int x, int y) : Command_Object(x, y, 40, 300, 0, 2, "C") {}
   
   Object* spawn(string unit_type, int x, int y) override {
       if(unit_type == "Tank") return new Tank(x, y);
       if(unit_type == "K9") return new K9(x, y);
       if(unit_type == "Redback") return new Redback(x, y);
       if(unit_type == "Soldier") return new Soldier(x, y);
       return nullptr;
   }
};

class Game {
private:
   int CursorX, CursorY;
   int Selected_X, Selected_Y;
   bool Is_Selected, Attack_Time, Move_Time, Spawn_Time;
   bool player_turn;
   
   vector<vector<Object*>> board;

public:
   Game() : CursorX(0), CursorY(0), Selected_X(0), Selected_Y(0), 
            Is_Selected(false), Attack_Time(false), Move_Time(false), Spawn_Time(false),
            player_turn(true)
   {
       board.resize(BOARD_HEIGHT, vector<Object*>(BOARD_WIDTH, nullptr));
       board[0][3] = new CommandCenter(0, 3);
       board[11][3] = new CommandCenter(11, 3);
   }

   void Draw_Board() {
       system("cls");
       HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

       for(int i = 0; i < BOARD_HEIGHT; i++) {
           for(int j = 0; j < BOARD_WIDTH; j++) {
               SetConsoleTextAttribute(console, 7);

               if(Attack_Time && board[Selected_Y][Selected_X] != nullptr) {
                   Attacking_Object* attacker = dynamic_cast<Attacking_Object*>(board[Selected_Y][Selected_X]);
                   if(attacker && abs(i - Selected_Y) + abs(j - Selected_X) <= attacker->getAttackRange()) {
                       SetConsoleTextAttribute(console, BACKGROUND_RED);
                   }
               }
               
               if(Move_Time && board[Selected_Y][Selected_X] != nullptr) {
                   if(abs(i - Selected_Y) + abs(j - Selected_X) <= board[Selected_Y][Selected_X]->getMoveDistance() 
                      && board[i][j] == nullptr) {
                       SetConsoleTextAttribute(console, BACKGROUND_GREEN);
                   }
               }
               
               if(Spawn_Time && board[Selected_Y][Selected_X] != nullptr) {
                   CommandCenter* cc = dynamic_cast<CommandCenter*>(board[Selected_Y][Selected_X]);
                   if(cc && ((player_turn && i == 0) || (!player_turn && i == 11)) 
                      && abs(j - Selected_X) <= cc->getSpawnRange() && board[i][j] == nullptr) {
                       SetConsoleTextAttribute(console, BACKGROUND_RED | BACKGROUND_BLUE);
                   }
               }

               if(Is_Selected && i == Selected_Y && j == Selected_X) {
                   SetConsoleTextAttribute(console, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
               }
               
               if(i == CursorY && j == CursorX) {
                   SetConsoleTextAttribute(console, BACKGROUND_BLUE | BACKGROUND_INTENSITY);
               }

               if(board[i][j] != nullptr) {
                   cout << board[i][j]->getName() << " ";
               } else {
                   cout << ". ";
               }
           }
           cout << endl;
       }
   }

   void Check_KeyInput() {
       if(_kbhit()) {
           int key = _getch();
           
           switch(key) {
               case 72:
                   if(CursorY > 0) CursorY--;
                   break;
               case 80:
                   if(CursorY < BOARD_HEIGHT - 1) CursorY++;
                   break;
               case 75:
                   if(CursorX > 0) CursorX--;
                   break;
               case 77:
                   if(CursorX < BOARD_WIDTH - 1) CursorX++;
                   break;
               case 'a':
               case 'A':
                    // 여기에 어떤 플레이어의 턴인지 확인해서
                    // 내 객체인지 아닌지 판단 후 표시해야함
                   if(board[CursorY][CursorX] != nullptr) {
                       if(dynamic_cast<Attacking_Object*>(board[CursorY][CursorX])) {
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
                   if(board[CursorY][CursorX] != nullptr) {
                       Selected_X = CursorX;
                       Selected_Y = CursorY;
                       Is_Selected = true;
                       Move_Time = !Move_Time;
                       Attack_Time = Spawn_Time = false;
                   }
                   break;
               case 's':
               case 'S':
                   if(board[CursorY][CursorX] != nullptr) {
                       if(dynamic_cast<CommandCenter*>(board[CursorY][CursorX])) {
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
                   player_turn = !player_turn;
                   break;
               case 32:
                   handleSpaceKey();
                   break;
           }
       }
   }

   void handleSpaceKey() {
       if(!Is_Selected) {
           if(board[CursorY][CursorX] != nullptr) {
               Selected_X = CursorX;
               Selected_Y = CursorY;
               Is_Selected = true;
           }
       }
       else {
           if(Attack_Time) {
               Attacking_Object* attacker = dynamic_cast<Attacking_Object*>(board[Selected_Y][Selected_X]);
               if(attacker && board[CursorY][CursorX] != nullptr) {
                   int dist = abs(CursorY - Selected_Y) + abs(CursorX - Selected_X);
                   if(dist <= attacker->getAttackRange()) {
                       attacker->attack(board[CursorY][CursorX]);
                   }
               }
           }
           else if(Move_Time) {
               int dist = abs(CursorY - Selected_Y) + abs(CursorX - Selected_X);
               if(dist <= board[Selected_Y][Selected_X]->getMoveDistance() && 
                  board[CursorY][CursorX] == nullptr) {
                   board[CursorY][CursorX] = board[Selected_Y][Selected_X];
                   board[Selected_Y][Selected_X] = nullptr;
               }
           }
           else if(Spawn_Time) {
               CommandCenter* cc = dynamic_cast<CommandCenter*>(board[Selected_Y][Selected_X]);
               if(cc && board[CursorY][CursorX] == nullptr) {
                   int dist = abs(CursorY - Selected_Y) + abs(CursorX - Selected_X);
                   if(dist <= cc->getSpawnRange()) 
                   {
                       int choice;
                       cout << "생성할 유닛을 선택하시오" << endl;
                       cout << "1. Soldier (1 Turn)" << endl << "2. Redback (2 Turn)" << endl << "3. Tank (2 Turn)" << endl << "4. K9 (3 Turn)" << endl;
                       cout << "입력 : ";
                       cin >> choice;
                       switch (choice)
                       {
                           // 생성 할 때 턴 개념 적용해야함
                        case 1:
                            board[CursorY][CursorX] = cc->spawn("Soldier", CursorY, CursorX);
                            break;
                        case 2:
                            board[CursorY][CursorX] = cc->spawn("Redback", CursorY, CursorX);
                            break;
                        case 3:
                            board[CursorY][CursorX] = cc->spawn("Tank", CursorY, CursorX);
                            break;
                        case 4:
                            board[CursorY][CursorX] = cc->spawn("K9", CursorY, CursorX);
                            break;
                       }
                       board[CursorY][CursorX] = cc->spawn("Tank", CursorY, CursorX);
                   }
               }
           }
           
           Is_Selected = false;
           Attack_Time = Move_Time = Spawn_Time = false;
       }
   }

   void Run() {
       while(true) {
           Draw_Board();
           Check_KeyInput();
           Sleep(100);
       }
   }
};

int main() {
   Draw_Title();
   Sleep(2500);
   Game game;
   game.Run();
   return 0;
}
